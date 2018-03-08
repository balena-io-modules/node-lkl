#include <linux/cpu.h>
#include <linux/cpuidle.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tick.h>
#include <asm/host_ops.h>
#include <asm/cpu.h>
#include <asm/thread_info.h>
#include <asm/unistd.h>
#include <asm/sched.h>
#include <asm/syscalls.h>


/*
 * This structure is used to get access to the "LKL CPU" that allows us to run
 * Linux code. Because we have to deal with various synchronization requirements
 * between idle thread, system calls, interrupts, "reentrancy", CPU shutdown,
 * imbalance wake up (i.e. acquire the CPU from one thread and release it from
 * another), we can't use a simple synchronization mechanism such as (recursive)
 * mutex or semaphore. Instead, we use a mutex and a bunch of status data plus a
 * semaphore.
 */
struct lkl_cpu {
	/* lock that protects the CPU status data */
	struct lkl_mutex *lock;
	/*
	 * Since we must free the cpu lock during shutdown we need a
	 * synchronization algorithm between lkl_cpu_shutdown() and the CPU
	 * access functions since lkl_cpu_get() gets called from thread
	 * destructor callback functions which may be scheduled after
	 * lkl_cpu_shutdown() has freed the cpu lock.
	 *
	 * An atomic counter is used to keep track of the number of running
	 * CPU access functions and allow the shutdown function to wait for
	 * them.
	 *
	 * The shutdown functions adds MAX_THREADS to this counter which allows
	 * the CPU access functions to check if the shutdown process has
	 * started.
	 *
	 * This algorithm assumes that we never have more the MAX_THREADS
	 * requesting CPU access.
	 */
	#define MAX_THREADS 1000000
	unsigned int shutdown_gate;
	bool irqs_pending;
	/* no of threads waiting the CPU */
	unsigned int sleepers;
	/* no of times the current thread got the CPU */
	unsigned int count;
	/* current thread that owns the CPU */
	lkl_thread_t owner;
	/* semaphore for threads waiting the CPU */
	struct lkl_sem *sem;
	/* semaphore for the idle thread */
	struct lkl_sem *idle_sem;
	/* if the idle thread is pending */
	bool idle_pending;
	/* jmp_buf used for idle thread to restart */
	struct lkl_jmp_buf idle_jb;
	/* semaphore used for shutdown */
	struct lkl_sem *shutdown_sem;
} cpu;

static int __cpu_try_get_lock(int n)
{
	lkl_thread_t self;

	if (__sync_fetch_and_add(&cpu.shutdown_gate, n) >= MAX_THREADS)
		return -2;

	lkl_ops->mutex_lock(cpu.lock);

	if (cpu.shutdown_gate >= MAX_THREADS)
		return -1;

	self = lkl_ops->thread_self();

	if (cpu.owner && !lkl_ops->thread_equal(cpu.owner, self))
		return 0;

	cpu.owner = self;
	cpu.count++;

	return 1;
}

static void __cpu_try_get_unlock(int lock_ret, int n)
{
	if (lock_ret >= -1)
		lkl_ops->mutex_unlock(cpu.lock);
	__sync_fetch_and_sub(&cpu.shutdown_gate, n);
}

void lkl_cpu_change_owner(lkl_thread_t owner)
{
	lkl_ops->mutex_lock(cpu.lock);
	if (cpu.count > 1)
		lkl_bug("bad count while changing owner\n");
	cpu.owner = owner;
	lkl_ops->mutex_unlock(cpu.lock);
}

int lkl_cpu_get(void)
{
	int ret;

	ret = __cpu_try_get_lock(1);

	while (ret == 0) {
		cpu.sleepers++;
		__cpu_try_get_unlock(ret, 0);
		lkl_ops->sem_down(cpu.sem);
		ret = __cpu_try_get_lock(0);
	}

	__cpu_try_get_unlock(ret, 1);

	return ret;
}

void lkl_cpu_put(void)
{
	lkl_ops->mutex_lock(cpu.lock);

	if (!cpu.count || !cpu.owner ||
	    !lkl_ops->thread_equal(cpu.owner, lkl_ops->thread_self()))
		lkl_bug("%s: unbalanced put\n", __func__);

	while (cpu.irqs_pending && !irqs_disabled()) {
		cpu.irqs_pending = false;
		lkl_ops->mutex_unlock(cpu.lock);
		run_irqs();
		lkl_ops->mutex_lock(cpu.lock);
	}

	if (need_resched() && cpu.count == 1) {
		if (in_interrupt())
			lkl_bug("%s: in interrupt\n", __func__);
		lkl_ops->mutex_unlock(cpu.lock);
		thread_sched_jb();
		return;
	}

	if (--cpu.count > 0) {
		lkl_ops->mutex_unlock(cpu.lock);
		return;
	}

	if (cpu.sleepers) {
		cpu.sleepers--;
		lkl_ops->sem_up(cpu.sem);
	}

	cpu.owner = 0;

	lkl_ops->mutex_unlock(cpu.lock);
}

int lkl_cpu_try_run_irq(int irq)
{
	int ret;

	ret = __cpu_try_get_lock(1);
	if (!ret) {
		set_irq_pending(irq);
		cpu.irqs_pending = true;
	}
	__cpu_try_get_unlock(ret, 1);

	return ret;
}

void lkl_cpu_shutdown(void)
{
	__sync_fetch_and_add(&cpu.shutdown_gate, MAX_THREADS);
}

void lkl_cpu_wait_shutdown(void)
{
	lkl_ops->sem_down(cpu.shutdown_sem);
	lkl_ops->sem_free(cpu.shutdown_sem);
}

static void lkl_cpu_cleanup(bool shutdown)
{
	while (__sync_fetch_and_add(&cpu.shutdown_gate, 0) > MAX_THREADS)
		;

	if (shutdown)
		lkl_ops->sem_up(cpu.shutdown_sem);
	else if (cpu.shutdown_sem)
		lkl_ops->sem_free(cpu.shutdown_sem);
	if (cpu.idle_sem)
		lkl_ops->sem_free(cpu.idle_sem);
	if (cpu.sem)
		lkl_ops->sem_free(cpu.sem);
	if (cpu.lock)
		lkl_ops->mutex_free(cpu.lock);
}

void arch_cpu_idle(void)
{
	if (cpu.shutdown_gate >= MAX_THREADS) {

		lkl_ops->mutex_lock(cpu.lock);
		while (cpu.sleepers--)
			lkl_ops->sem_up(cpu.sem);
		lkl_ops->mutex_unlock(cpu.lock);

		lkl_cpu_cleanup(true);

		lkl_ops->thread_exit();
	}
	/* enable irqs now to allow direct irqs to run */
	local_irq_enable();

	if (need_resched())
		return;

	cpu.idle_pending = true;
	lkl_cpu_put();

	lkl_ops->sem_down(cpu.idle_sem);

	cpu.idle_pending = false;
	/* to match that of schedule_preempt_disabled() */
	preempt_disable();
	lkl_ops->jmp_buf_longjmp(&cpu.idle_jb, 1);
}

void arch_cpu_idle_prepare(void)
{
	set_ti_thread_flag(current_thread_info(), TIF_IDLE);
	/*
	 * We hijack the idle loop here so that we can let the idle thread
	 * jump back to the beginning.
	 */
	while (1)
		lkl_ops->jmp_buf_set(&cpu.idle_jb, cpu_idle_loop);
}

void lkl_cpu_wakeup_idle(void)
{
	lkl_ops->sem_up(cpu.idle_sem);
}

int lkl_cpu_init(void)
{
	cpu.lock = lkl_ops->mutex_alloc(0);
	cpu.sem = lkl_ops->sem_alloc(0);
	cpu.idle_sem = lkl_ops->sem_alloc(0);
	cpu.shutdown_sem = lkl_ops->sem_alloc(0);

	if (!cpu.lock || !cpu.sem || !cpu.idle_sem || !cpu.shutdown_sem) {
		lkl_cpu_cleanup(false);
		return -ENOMEM;
	}

	return 0;
}

/*
 * Simulate the exit path of idle loop so that we can schedule when LKL is
 * in idle.
 * It's just a duplication of those in idle.c so a better way is to refactor
 * idle.c to expose such function.
 */
void lkl_idle_tail_schedule(void)
{

	if (!cpu.idle_pending ||
		!test_bit(TIF_IDLE, &current_thread_info()->flags))
		lkl_bug("%s: not in idle\n", __func__);

	start_critical_timings();
	__current_set_polling();

	if (WARN_ON_ONCE(irqs_disabled()))
		local_irq_enable();

	rcu_idle_exit();
	arch_cpu_idle_exit();
	preempt_set_need_resched();
	tick_nohz_idle_exit();
	__current_clr_polling();

	/*
	 * memory barrier copied from idle.c
	 */
	smp_mb__after_atomic();

	/*
	 * Didn't find a way to include kernel/sched/sched.h for
	 * sched_ttwu_pending().
	 * Anyway, it's no op when not CONFIG_SMP.
	 */

	schedule_preempt_disabled();
}

int lkl_cpu_idle_pending(void)
{
	return cpu.idle_pending;
}

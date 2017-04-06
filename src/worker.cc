#include "async.h"
#include "worker.h"

uv_sem_t worker_ready_sem;
uv_loop_t worker_loop;
uv_thread_t worker_thread;
uv_async_t worker_async;
call_info_t worker_info;


void worker_loop_entry(uv_async_t* handle) {
	call_info_t *info = (call_info_t*) handle->data;
	info->fn(info->args);
}

void worker_thread_start(void *arg) {
	int ret;
	ret = uv_loop_init(&worker_loop);
	if (ret) {
		abort();
	}
	ret = uv_async_init(&worker_loop, &worker_async, worker_loop_entry);
	if (ret) {
		abort();
	}
	uv_sem_post(&worker_ready_sem);
	ret = uv_run(&worker_loop, UV_RUN_DEFAULT);
	if (ret) {
		abort();
	}
}

void run_on_worker_loop(void (*func)(void*), void* args) {
	worker_info.fn = func;
	worker_info.args = args;
	int ret = uv_async_send(&worker_async);
	if (ret) {
		abort();
	}
}

void init_worker() {
	int ret;
	uv_sem_init(&worker_ready_sem, 0);
	worker_async.data = &worker_info;
	ret = uv_thread_create(&worker_thread, worker_thread_start, NULL);
	if (ret) {
		abort();
	}
	uv_sem_wait(&worker_ready_sem);
}

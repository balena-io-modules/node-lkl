'use strict';

const bindings = require('bindings')('bindings');
const constants = require('./constants');
const queue = require('./queue');

function syscall() {
	const args = [];
	let i;
	// copy all given arguments except the last one (the callback)
	for (i = 0; i < arguments.length - 1; i++) {
		args[i] = arguments[i];
	}
	// fill with zeros
	for (i; i < 7; i++) {
		args[i] = 0;
	}
	// add the callback as last argument
	args[7] = arguments[arguments.length - 1];
	queue.addOperation(bindings.syscall, args)
}

module.exports = {
	access:        syscall.bind(null, constants.SYS_access),
	chdir:         syscall.bind(null, constants.SYS_chdir),
	chmod:         syscall.bind(null, constants.SYS_chmod),
	chroot:        syscall.bind(null, constants.SYS_chroot),
	clock_settime: syscall.bind(null, constants.SYS_clock_settime),
	close:         syscall.bind(null, constants.SYS_close),
	fchdir:        syscall.bind(null, constants.SYS_fchdir),
	fchmod:        syscall.bind(null, constants.SYS_fchmod),
	fchownat:      syscall.bind(null, constants.SYS_fchownat),
	fchown:        syscall.bind(null, constants.SYS_fchown),
	fcntl:         syscall.bind(null, constants.SYS_fcntl),
	fdatasync:     syscall.bind(null, constants.SYS_fdatasync),
	fsync:         syscall.bind(null, constants.SYS_fsync),
	fstat:         syscall.bind(null, constants.SYS_fstat),
	getcwd:        syscall.bind(null, constants.SYS_getcwd),
	getdents:      syscall.bind(null, constants.SYS_getdents),
	getdents64:    syscall.bind(null, constants.SYS_getdents64),
	getxattr:      syscall.bind(null, constants.SYS_getxattr),
	lchown:        syscall.bind(null, constants.SYS_lchown),
	linkat:        syscall.bind(null, constants.SYS_linkat),
	listxattr:     syscall.bind(null, constants.SYS_listxattr),
	mkdirat:       syscall.bind(null, constants.SYS_mkdirat),
	mknodat:       syscall.bind(null, constants.SYS_mknodat),
	openat:        syscall.bind(null, constants.SYS_openat),
	pread64:       syscall.bind(null, constants.SYS_pread64),
	pwrite64:      syscall.bind(null, constants.SYS_pwrite64),
	read:          syscall.bind(null, constants.SYS_read),
	readlinkat:    syscall.bind(null, constants.SYS_readlinkat),
	removexattr:   syscall.bind(null, constants.SYS_removexattr),
	renameat2:     syscall.bind(null, constants.SYS_renameat2),
	setxattr:      syscall.bind(null, constants.SYS_setxattr),
	symlinkat:     syscall.bind(null, constants.SYS_symlinkat),
	sync:          syscall.bind(null, constants.SYS_sync),
	truncate:      syscall.bind(null, constants.SYS_truncate),
	unlinkat:      syscall.bind(null, constants.SYS_unlinkat),
	write:         syscall.bind(null, constants.SYS_write)
};

// Unimplemented syscalls

// async faccessat
// async fallocate
// async fchmodat
// async flock
// async fstatfs
// async ftruncate
// async futimesat
// async inotifyAddWatch
// async inotifyInit1
// async inotifyInit
// async inotifyRmWatch
// async lstat
// async readlen
// async seek
// async sendfile
// async statfs
// async stat
// async syncFileRange
// async utimensat
// async utime
// async utimes
// async writelen

// // io
// 
// Dup3
// Dup
// Splice
// Tee
// pipe2
// pipe

// # other
// Acct
// Adjtimex
// EpollCreate1
// EpollCreate
// EpollCtl
// EpollWait
// Exit
// exitThread
// Getegid
// Geteuid
// Getgid
// getgroups
// getpeername
// Getpgid
// Getpid
// Getppid
// Getpriority
// Getrlimit
// Getrusage
// Gettid
// Getuid
// Kill
// Klogctl
// Mlockall
// Mlock
// mmap
// mount
// Mprotect
// Munlockall
// Munlock
// munmap
// Nanosleep
// Pause
// PivotRoot
// prlimit
// ptrace
// reboot
// recvfrom
// recvmsg
// Select
// sendmsg
// sendto
// Setdomainname
// Setfsgid
// Setfsuid
// setgroups
// Sethostname
// Setpgid
// Setpriority
// Setregid
// Setresgid
// Setresuid
// Setreuid
// Setrlimit
// Setsid
// Settimeofday
// Shutdown
// Sysinfo
// Tgkill
// Times
// Umask
// Uname
// Unmount
// Unshare
// Madvise
// Ustat
// wait4

'use strict';

const bindings = require('bindings')('bindings');
const constants = require('./constants');

function syscall0(nr, callback) {
	return bindings.syscall(nr, 0, 0, 0, 0, 0, 0, callback);
}

function syscall1(nr, a, callback) {
	return bindings.syscall(nr, a, 0, 0, 0, 0, 0, callback);
}

function syscall2(nr, a, b, callback) {
	return bindings.syscall(nr, a, b, 0, 0, 0, 0, callback);
}

function syscall3(nr, a, b, c, callback) {
	return bindings.syscall(nr, a, b, c, 0, 0, 0, callback);
}

function syscall4(nr, a, b, c, d, callback) {
	return bindings.syscall(nr, a, b, c, d, 0, 0, callback);
}

function syscall5(nr, a, b, c, d, e, callback) {
	return bindings.syscall(nr, a, b, c, d, e, 0, callback);
}

module.exports = {
	access:       syscall2.bind(null, constants.SYS_access),
	chdir:        syscall1.bind(null, constants.SYS_chdir),
	chmod:        syscall2.bind(null, constants.SYS_chmod),
	chroot:       syscall1.bind(null, constants.SYS_chroot),
	close:        syscall1.bind(null, constants.SYS_close),
	fchdir:       syscall1.bind(null, constants.SYS_fchdir),
	fchmod:       syscall2.bind(null, constants.SYS_fchmod),
	fchownat:     syscall5.bind(null, constants.SYS_fchownat),
	fchown:       syscall3.bind(null, constants.SYS_fchown),
	fcntl:        syscall3.bind(null, constants.SYS_fcntl),
	fdatasync:    syscall1.bind(null, constants.SYS_fdatasync),
	fsync:        syscall1.bind(null, constants.SYS_fsync),
	fstat:        syscall2.bind(null, constants.SYS_fstat),
	getcwd:       syscall1.bind(null, constants.SYS_getcwd),
	getdents:     syscall2.bind(null, constants.SYS_getdents),
	getdents64:   syscall3.bind(null, constants.SYS_getdents64),
	getxattr:     syscall3.bind(null, constants.SYS_getxattr),
	lchown:       syscall3.bind(null, constants.SYS_lchown),
	linkat:       syscall5.bind(null, constants.SYS_linkat),
	listxattr:    syscall2.bind(null, constants.SYS_listxattr),
	mkdirat:      syscall3.bind(null, constants.SYS_mkdirat),
	mknodat:      syscall4.bind(null, constants.SYS_mknodat),
	openat:       syscall4.bind(null, constants.SYS_openat),
	pread64:      syscall4.bind(null, constants.SYS_pread64),
	pwrite64:     syscall4.bind(null, constants.SYS_pwrite64),
	read:         syscall3.bind(null, constants.SYS_read),
	readlinkat:   syscall3.bind(null, constants.SYS_readlinkat),
	removexattr:  syscall2.bind(null, constants.SYS_removexattr),
	renameat2:    syscall5.bind(null, constants.SYS_renameat2),
	setxattr:     syscall4.bind(null, constants.SYS_setxattr),
	symlinkat:    syscall3.bind(null, constants.SYS_symlinkat),
	sync:         syscall0.bind(null, constants.SYS_sync),
	truncate:     syscall2.bind(null, constants.SYS_truncate),
	unlinkat:     syscall3.bind(null, constants.SYS_unlinkat),
	write:        syscall3.bind(null, constants.SYS_write)
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

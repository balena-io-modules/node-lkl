extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "zsyscall.h"

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(chdir) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);

	long ret = lkl_sys_chdir(*path);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(chroot) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);

	long ret = lkl_sys_chroot(*path);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(close) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	long ret = lkl_sys_close(fd);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fchdir) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	long ret = lkl_sys_fchdir(fd);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fchmod) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	uint mode = info[1]->Uint32Value();

	long ret = lkl_sys_fchmod(fd, mode);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fchownat) {
	if (info.Length() < 5) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	int uid = info[2]->Uint32Value();
	int gid = info[3]->Uint32Value();
	int flags = info[4]->Uint32Value();

	long ret = lkl_sys_fchownat(dirfd, *path, uid, gid, flags);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fchown) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	int uid = info[1]->Uint32Value();
	int gid = info[2]->Uint32Value();

	long ret = lkl_sys_fchown(fd, uid, gid);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fcntl) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	int cmd = info[1]->Uint32Value();
	int arg = info[2]->Uint32Value();

	long ret = lkl_sys_fcntl(fd, cmd, arg);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fdatasync) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	long ret = lkl_sys_fdatasync(fd);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(fsync) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	long ret = lkl_sys_fsync(fd);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(getcwd) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	void *buf = node::Buffer::Data(info[0]->ToObject());
	int buf_len = node::Buffer::Length(info[0]->ToObject());

	long ret = lkl_sys_getcwd(buf, buf_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(getdents) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *buf = node::Buffer::Data(info[1]->ToObject());
	int buf_len = node::Buffer::Length(info[1]->ToObject());

	long ret = lkl_sys_getdents(fd, buf, buf_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(getxattr) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String attr(info[1]);
	void *dest = node::Buffer::Data(info[2]->ToObject());
	int dest_len = node::Buffer::Length(info[2]->ToObject());

	long ret = lkl_sys_getxattr(*path, *attr, dest, dest_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(lchown) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	int uid = info[1]->Uint32Value();
	int gid = info[2]->Uint32Value();

	long ret = lkl_sys_lchown(*path, uid, gid);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(linkat) {
	if (info.Length() < 5) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int olddirfd = info[0]->Uint32Value();
	Nan::Utf8String oldpath(info[1]);
	int newdirfd = info[2]->Uint32Value();
	Nan::Utf8String newpath(info[3]);
	int flags = info[4]->Uint32Value();

	long ret = lkl_sys_linkat(olddirfd, *oldpath, newdirfd, *newpath, flags);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(listxattr) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	void *dest = node::Buffer::Data(info[1]->ToObject());
	int dest_len = node::Buffer::Length(info[1]->ToObject());

	long ret = lkl_sys_listxattr(*path, dest, dest_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(mkdirat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	uint mode = info[2]->Uint32Value();

	long ret = lkl_sys_mkdirat(dirfd, *path, mode);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(mknodat) {
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	uint mode = info[2]->Uint32Value();
	int dev = info[3]->Uint32Value();

	long ret = lkl_sys_mknodat(dirfd, *path, mode, dev);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(openat) {
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	int flags = info[2]->Uint32Value();
	uint mode = info[3]->Uint32Value();

	long ret = lkl_sys_openat(dirfd, *path, flags, mode);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(pread64) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());
	int offset = info[2]->Uint32Value();

	long ret = lkl_sys_pread64(fd, p, p_len, offset);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(pwrite64) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());
	int offset = info[2]->Uint32Value();

	long ret = lkl_sys_pwrite64(fd, p, p_len, offset);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(read) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());

	long ret = lkl_sys_read(fd, p, p_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(readlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	void *buf = node::Buffer::Data(info[2]->ToObject());
	int buf_len = node::Buffer::Length(info[2]->ToObject());

	long ret = lkl_sys_readlinkat(dirfd, *path, buf, buf_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(removexattr) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String attr(info[1]);

	long ret = lkl_sys_removexattr(*path, *attr);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(renameat2) {
	if (info.Length() < 5) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int olddirfd = info[0]->Uint32Value();
	Nan::Utf8String oldpath(info[1]);
	int newdirfd = info[2]->Uint32Value();
	Nan::Utf8String newpath(info[3]);
	int flags = info[4]->Uint32Value();

	long ret = lkl_sys_renameat2(olddirfd, *oldpath, newdirfd, *newpath, flags);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(setxattr) {
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String attr(info[1]);
	void *data = node::Buffer::Data(info[2]->ToObject());
	int data_len = node::Buffer::Length(info[2]->ToObject());
	int flags = info[3]->Uint32Value();

	long ret = lkl_sys_setxattr(*path, *attr, data, data_len, flags);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(symlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String oldpath(info[0]);
	int newdirfd = info[1]->Uint32Value();
	Nan::Utf8String newpath(info[2]);

	long ret = lkl_sys_symlinkat(*oldpath, newdirfd, *newpath);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(truncate) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	int length = info[1]->Uint32Value();

	long ret = lkl_sys_truncate(*path, length);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(unlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	int flags = info[2]->Uint32Value();

	long ret = lkl_sys_unlinkat(dirfd, *path, flags);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

// MACHINE GENERATED; DO NOT EDIT

NAN_METHOD(write) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());

	long ret = lkl_sys_write(fd, p, p_len);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}


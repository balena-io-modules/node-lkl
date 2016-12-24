extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "zsyscall.h"

// MACHINE GENERATED; DO NOT EDIT

class chdirWorker : public Nan::AsyncWorker {
	public:
		chdirWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path)
		: Nan::AsyncWorker(callback),path(path)		{
			SaveToPersistent(0u, info[0]);
		}

		~chdirWorker() {}

		void Execute () {
			ret = lkl_sys_chdir(path);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		int ret;
};

NAN_METHOD(chdir) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new chdirWorker(info, callback,*path));
	} else {
		long ret = lkl_sys_chdir(*path);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class chrootWorker : public Nan::AsyncWorker {
	public:
		chrootWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path)
		: Nan::AsyncWorker(callback),path(path)		{
			SaveToPersistent(0u, info[0]);
		}

		~chrootWorker() {}

		void Execute () {
			ret = lkl_sys_chroot(path);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		int ret;
};

NAN_METHOD(chroot) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new chrootWorker(info, callback,*path));
	} else {
		long ret = lkl_sys_chroot(*path);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class closeWorker : public Nan::AsyncWorker {
	public:
		closeWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd)
		: Nan::AsyncWorker(callback),fd(fd)		{
					}

		~closeWorker() {}

		void Execute () {
			ret = lkl_sys_close(fd);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int ret;
};

NAN_METHOD(close) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new closeWorker(info, callback,fd));
	} else {
		long ret = lkl_sys_close(fd);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fchdirWorker : public Nan::AsyncWorker {
	public:
		fchdirWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd)
		: Nan::AsyncWorker(callback),fd(fd)		{
					}

		~fchdirWorker() {}

		void Execute () {
			ret = lkl_sys_fchdir(fd);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int ret;
};

NAN_METHOD(fchdir) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new fchdirWorker(info, callback,fd));
	} else {
		long ret = lkl_sys_fchdir(fd);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fchmodWorker : public Nan::AsyncWorker {
	public:
		fchmodWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, uint mode)
		: Nan::AsyncWorker(callback),fd(fd), mode(mode)		{
					}

		~fchmodWorker() {}

		void Execute () {
			ret = lkl_sys_fchmod(fd, mode);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		uint mode;
		int ret;
};

NAN_METHOD(fchmod) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	uint mode = info[1]->Uint32Value();

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new fchmodWorker(info, callback,fd, mode));
	} else {
		long ret = lkl_sys_fchmod(fd, mode);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fchownatWorker : public Nan::AsyncWorker {
	public:
		fchownatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, int uid, int gid, int flags)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), uid(uid), gid(gid), flags(flags)		{
			SaveToPersistent(1u, info[1]);
		}

		~fchownatWorker() {}

		void Execute () {
			ret = lkl_sys_fchownat(dirfd, path, uid, gid, flags);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		int uid;
		int gid;
		int flags;
		int ret;
};

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

	if (info[5]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[5].As<v8::Function>());
		Nan::AsyncQueueWorker(new fchownatWorker(info, callback,dirfd, *path, uid, gid, flags));
	} else {
		long ret = lkl_sys_fchownat(dirfd, *path, uid, gid, flags);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fchownWorker : public Nan::AsyncWorker {
	public:
		fchownWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, int uid, int gid)
		: Nan::AsyncWorker(callback),fd(fd), uid(uid), gid(gid)		{
					}

		~fchownWorker() {}

		void Execute () {
			ret = lkl_sys_fchown(fd, uid, gid);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int uid;
		int gid;
		int ret;
};

NAN_METHOD(fchown) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	int uid = info[1]->Uint32Value();
	int gid = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new fchownWorker(info, callback,fd, uid, gid));
	} else {
		long ret = lkl_sys_fchown(fd, uid, gid);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fcntlWorker : public Nan::AsyncWorker {
	public:
		fcntlWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, int cmd, int arg)
		: Nan::AsyncWorker(callback),fd(fd), cmd(cmd), arg(arg)		{
					}

		~fcntlWorker() {}

		void Execute () {
			ret = lkl_sys_fcntl(fd, cmd, arg);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int cmd;
		int arg;
		int ret;
};

NAN_METHOD(fcntl) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	int cmd = info[1]->Uint32Value();
	int arg = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new fcntlWorker(info, callback,fd, cmd, arg));
	} else {
		long ret = lkl_sys_fcntl(fd, cmd, arg);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fdatasyncWorker : public Nan::AsyncWorker {
	public:
		fdatasyncWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd)
		: Nan::AsyncWorker(callback),fd(fd)		{
					}

		~fdatasyncWorker() {}

		void Execute () {
			ret = lkl_sys_fdatasync(fd);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int ret;
};

NAN_METHOD(fdatasync) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new fdatasyncWorker(info, callback,fd));
	} else {
		long ret = lkl_sys_fdatasync(fd);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class fsyncWorker : public Nan::AsyncWorker {
	public:
		fsyncWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd)
		: Nan::AsyncWorker(callback),fd(fd)		{
					}

		~fsyncWorker() {}

		void Execute () {
			ret = lkl_sys_fsync(fd);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		int ret;
};

NAN_METHOD(fsync) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new fsyncWorker(info, callback,fd));
	} else {
		long ret = lkl_sys_fsync(fd);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class getcwdWorker : public Nan::AsyncWorker {
	public:
		getcwdWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,void *buf, int buf_len)
		: Nan::AsyncWorker(callback),buf(buf),
					buf_len(buf_len)		{
			SaveToPersistent(0u, info[0]);
		}

		~getcwdWorker() {}

		void Execute () {
			ret = lkl_sys_getcwd(buf, buf_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		void *buf;
		int buf_len;
		int ret;
};

NAN_METHOD(getcwd) {
	if (info.Length() < 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	void *buf = node::Buffer::Data(info[0]->ToObject());
	int buf_len = node::Buffer::Length(info[0]->ToObject());

	if (info[1]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
		Nan::AsyncQueueWorker(new getcwdWorker(info, callback,buf, buf_len));
	} else {
		long ret = lkl_sys_getcwd(buf, buf_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class getdentsWorker : public Nan::AsyncWorker {
	public:
		getdentsWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, void *buf, int buf_len)
		: Nan::AsyncWorker(callback),fd(fd), buf(buf),
					buf_len(buf_len)		{
			SaveToPersistent(1u, info[1]);
		}

		~getdentsWorker() {}

		void Execute () {
			ret = lkl_sys_getdents(fd, buf, buf_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		void *buf;
		int buf_len;
		int ret;
};

NAN_METHOD(getdents) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *buf = node::Buffer::Data(info[1]->ToObject());
	int buf_len = node::Buffer::Length(info[1]->ToObject());

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new getdentsWorker(info, callback,fd, buf, buf_len));
	} else {
		long ret = lkl_sys_getdents(fd, buf, buf_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class getxattrWorker : public Nan::AsyncWorker {
	public:
		getxattrWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, char *attr, void *dest, int dest_len)
		: Nan::AsyncWorker(callback),path(path), attr(attr), dest(dest),
					dest_len(dest_len)		{
			SaveToPersistent(0u, info[0]);
			SaveToPersistent(1u, info[1]);
			SaveToPersistent(2u, info[2]);
		}

		~getxattrWorker() {}

		void Execute () {
			ret = lkl_sys_getxattr(path, attr, dest, dest_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		char *attr;
		void *dest;
		int dest_len;
		int ret;
};

NAN_METHOD(getxattr) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String attr(info[1]);
	void *dest = node::Buffer::Data(info[2]->ToObject());
	int dest_len = node::Buffer::Length(info[2]->ToObject());

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new getxattrWorker(info, callback,*path, *attr, dest, dest_len));
	} else {
		long ret = lkl_sys_getxattr(*path, *attr, dest, dest_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class lchownWorker : public Nan::AsyncWorker {
	public:
		lchownWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, int uid, int gid)
		: Nan::AsyncWorker(callback),path(path), uid(uid), gid(gid)		{
			SaveToPersistent(0u, info[0]);
		}

		~lchownWorker() {}

		void Execute () {
			ret = lkl_sys_lchown(path, uid, gid);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		int uid;
		int gid;
		int ret;
};

NAN_METHOD(lchown) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	int uid = info[1]->Uint32Value();
	int gid = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new lchownWorker(info, callback,*path, uid, gid));
	} else {
		long ret = lkl_sys_lchown(*path, uid, gid);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class linkatWorker : public Nan::AsyncWorker {
	public:
		linkatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int olddirfd, char *oldpath, int newdirfd, char *newpath, int flags)
		: Nan::AsyncWorker(callback),olddirfd(olddirfd), oldpath(oldpath), newdirfd(newdirfd), newpath(newpath), flags(flags)		{
			SaveToPersistent(1u, info[1]);
			SaveToPersistent(3u, info[3]);
		}

		~linkatWorker() {}

		void Execute () {
			ret = lkl_sys_linkat(olddirfd, oldpath, newdirfd, newpath, flags);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int olddirfd;
		char *oldpath;
		int newdirfd;
		char *newpath;
		int flags;
		int ret;
};

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

	if (info[5]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[5].As<v8::Function>());
		Nan::AsyncQueueWorker(new linkatWorker(info, callback,olddirfd, *oldpath, newdirfd, *newpath, flags));
	} else {
		long ret = lkl_sys_linkat(olddirfd, *oldpath, newdirfd, *newpath, flags);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class listxattrWorker : public Nan::AsyncWorker {
	public:
		listxattrWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, void *dest, int dest_len)
		: Nan::AsyncWorker(callback),path(path), dest(dest),
					dest_len(dest_len)		{
			SaveToPersistent(0u, info[0]);
			SaveToPersistent(1u, info[1]);
		}

		~listxattrWorker() {}

		void Execute () {
			ret = lkl_sys_listxattr(path, dest, dest_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		void *dest;
		int dest_len;
		int ret;
};

NAN_METHOD(listxattr) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	void *dest = node::Buffer::Data(info[1]->ToObject());
	int dest_len = node::Buffer::Length(info[1]->ToObject());

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new listxattrWorker(info, callback,*path, dest, dest_len));
	} else {
		long ret = lkl_sys_listxattr(*path, dest, dest_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class mkdiratWorker : public Nan::AsyncWorker {
	public:
		mkdiratWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, uint mode)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), mode(mode)		{
			SaveToPersistent(1u, info[1]);
		}

		~mkdiratWorker() {}

		void Execute () {
			ret = lkl_sys_mkdirat(dirfd, path, mode);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		uint mode;
		int ret;
};

NAN_METHOD(mkdirat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	uint mode = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new mkdiratWorker(info, callback,dirfd, *path, mode));
	} else {
		long ret = lkl_sys_mkdirat(dirfd, *path, mode);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class mknodatWorker : public Nan::AsyncWorker {
	public:
		mknodatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, uint mode, int dev)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), mode(mode), dev(dev)		{
			SaveToPersistent(1u, info[1]);
		}

		~mknodatWorker() {}

		void Execute () {
			ret = lkl_sys_mknodat(dirfd, path, mode, dev);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		uint mode;
		int dev;
		int ret;
};

NAN_METHOD(mknodat) {
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	uint mode = info[2]->Uint32Value();
	int dev = info[3]->Uint32Value();

	if (info[4]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[4].As<v8::Function>());
		Nan::AsyncQueueWorker(new mknodatWorker(info, callback,dirfd, *path, mode, dev));
	} else {
		long ret = lkl_sys_mknodat(dirfd, *path, mode, dev);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class openatWorker : public Nan::AsyncWorker {
	public:
		openatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, int flags, uint mode)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), flags(flags), mode(mode)		{
			SaveToPersistent(1u, info[1]);
		}

		~openatWorker() {}

		void Execute () {
			ret = lkl_sys_openat(dirfd, path, flags, mode);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		int flags;
		uint mode;
		int ret;
};

NAN_METHOD(openat) {
	if (info.Length() < 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	int flags = info[2]->Uint32Value();
	uint mode = info[3]->Uint32Value();

	if (info[4]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[4].As<v8::Function>());
		Nan::AsyncQueueWorker(new openatWorker(info, callback,dirfd, *path, flags, mode));
	} else {
		long ret = lkl_sys_openat(dirfd, *path, flags, mode);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class pread64Worker : public Nan::AsyncWorker {
	public:
		pread64Worker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, void *p, int p_len, int offset)
		: Nan::AsyncWorker(callback),fd(fd), p(p),
					p_len(p_len), offset(offset)		{
			SaveToPersistent(1u, info[1]);
		}

		~pread64Worker() {}

		void Execute () {
			ret = lkl_sys_pread64(fd, p, p_len, offset);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		void *p;
		int p_len;
		int offset;
		int ret;
};

NAN_METHOD(pread64) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());
	int offset = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new pread64Worker(info, callback,fd, p, p_len, offset));
	} else {
		long ret = lkl_sys_pread64(fd, p, p_len, offset);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class pwrite64Worker : public Nan::AsyncWorker {
	public:
		pwrite64Worker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, void *p, int p_len, int offset)
		: Nan::AsyncWorker(callback),fd(fd), p(p),
					p_len(p_len), offset(offset)		{
			SaveToPersistent(1u, info[1]);
		}

		~pwrite64Worker() {}

		void Execute () {
			ret = lkl_sys_pwrite64(fd, p, p_len, offset);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		void *p;
		int p_len;
		int offset;
		int ret;
};

NAN_METHOD(pwrite64) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());
	int offset = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new pwrite64Worker(info, callback,fd, p, p_len, offset));
	} else {
		long ret = lkl_sys_pwrite64(fd, p, p_len, offset);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class readWorker : public Nan::AsyncWorker {
	public:
		readWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, void *p, int p_len)
		: Nan::AsyncWorker(callback),fd(fd), p(p),
					p_len(p_len)		{
			SaveToPersistent(1u, info[1]);
		}

		~readWorker() {}

		void Execute () {
			ret = lkl_sys_read(fd, p, p_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		void *p;
		int p_len;
		int ret;
};

NAN_METHOD(read) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new readWorker(info, callback,fd, p, p_len));
	} else {
		long ret = lkl_sys_read(fd, p, p_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class readlinkatWorker : public Nan::AsyncWorker {
	public:
		readlinkatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, void *buf, int buf_len)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), buf(buf),
					buf_len(buf_len)		{
			SaveToPersistent(1u, info[1]);
			SaveToPersistent(2u, info[2]);
		}

		~readlinkatWorker() {}

		void Execute () {
			ret = lkl_sys_readlinkat(dirfd, path, buf, buf_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		void *buf;
		int buf_len;
		int ret;
};

NAN_METHOD(readlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	void *buf = node::Buffer::Data(info[2]->ToObject());
	int buf_len = node::Buffer::Length(info[2]->ToObject());

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new readlinkatWorker(info, callback,dirfd, *path, buf, buf_len));
	} else {
		long ret = lkl_sys_readlinkat(dirfd, *path, buf, buf_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class removexattrWorker : public Nan::AsyncWorker {
	public:
		removexattrWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, char *attr)
		: Nan::AsyncWorker(callback),path(path), attr(attr)		{
			SaveToPersistent(0u, info[0]);
			SaveToPersistent(1u, info[1]);
		}

		~removexattrWorker() {}

		void Execute () {
			ret = lkl_sys_removexattr(path, attr);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		char *attr;
		int ret;
};

NAN_METHOD(removexattr) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String attr(info[1]);

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new removexattrWorker(info, callback,*path, *attr));
	} else {
		long ret = lkl_sys_removexattr(*path, *attr);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class renameat2Worker : public Nan::AsyncWorker {
	public:
		renameat2Worker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int olddirfd, char *oldpath, int newdirfd, char *newpath, int flags)
		: Nan::AsyncWorker(callback),olddirfd(olddirfd), oldpath(oldpath), newdirfd(newdirfd), newpath(newpath), flags(flags)		{
			SaveToPersistent(1u, info[1]);
			SaveToPersistent(3u, info[3]);
		}

		~renameat2Worker() {}

		void Execute () {
			ret = lkl_sys_renameat2(olddirfd, oldpath, newdirfd, newpath, flags);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int olddirfd;
		char *oldpath;
		int newdirfd;
		char *newpath;
		int flags;
		int ret;
};

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

	if (info[5]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[5].As<v8::Function>());
		Nan::AsyncQueueWorker(new renameat2Worker(info, callback,olddirfd, *oldpath, newdirfd, *newpath, flags));
	} else {
		long ret = lkl_sys_renameat2(olddirfd, *oldpath, newdirfd, *newpath, flags);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class setxattrWorker : public Nan::AsyncWorker {
	public:
		setxattrWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, char *attr, void *data, int data_len, int flags)
		: Nan::AsyncWorker(callback),path(path), attr(attr), data(data),
					data_len(data_len), flags(flags)		{
			SaveToPersistent(0u, info[0]);
			SaveToPersistent(1u, info[1]);
			SaveToPersistent(2u, info[2]);
		}

		~setxattrWorker() {}

		void Execute () {
			ret = lkl_sys_setxattr(path, attr, data, data_len, flags);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		char *attr;
		void *data;
		int data_len;
		int flags;
		int ret;
};

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

	if (info[4]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[4].As<v8::Function>());
		Nan::AsyncQueueWorker(new setxattrWorker(info, callback,*path, *attr, data, data_len, flags));
	} else {
		long ret = lkl_sys_setxattr(*path, *attr, data, data_len, flags);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class symlinkatWorker : public Nan::AsyncWorker {
	public:
		symlinkatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *oldpath, int newdirfd, char *newpath)
		: Nan::AsyncWorker(callback),oldpath(oldpath), newdirfd(newdirfd), newpath(newpath)		{
			SaveToPersistent(0u, info[0]);
			SaveToPersistent(2u, info[2]);
		}

		~symlinkatWorker() {}

		void Execute () {
			ret = lkl_sys_symlinkat(oldpath, newdirfd, newpath);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *oldpath;
		int newdirfd;
		char *newpath;
		int ret;
};

NAN_METHOD(symlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String oldpath(info[0]);
	int newdirfd = info[1]->Uint32Value();
	Nan::Utf8String newpath(info[2]);

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new symlinkatWorker(info, callback,*oldpath, newdirfd, *newpath));
	} else {
		long ret = lkl_sys_symlinkat(*oldpath, newdirfd, *newpath);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class truncateWorker : public Nan::AsyncWorker {
	public:
		truncateWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,char *path, int length)
		: Nan::AsyncWorker(callback),path(path), length(length)		{
			SaveToPersistent(0u, info[0]);
		}

		~truncateWorker() {}

		void Execute () {
			ret = lkl_sys_truncate(path, length);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		char *path;
		int length;
		int ret;
};

NAN_METHOD(truncate) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	Nan::Utf8String path(info[0]);
	int length = info[1]->Uint32Value();

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new truncateWorker(info, callback,*path, length));
	} else {
		long ret = lkl_sys_truncate(*path, length);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class unlinkatWorker : public Nan::AsyncWorker {
	public:
		unlinkatWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int dirfd, char *path, int flags)
		: Nan::AsyncWorker(callback),dirfd(dirfd), path(path), flags(flags)		{
			SaveToPersistent(1u, info[1]);
		}

		~unlinkatWorker() {}

		void Execute () {
			ret = lkl_sys_unlinkat(dirfd, path, flags);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int dirfd;
		char *path;
		int flags;
		int ret;
};

NAN_METHOD(unlinkat) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int dirfd = info[0]->Uint32Value();
	Nan::Utf8String path(info[1]);
	int flags = info[2]->Uint32Value();

	if (info[3]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[3].As<v8::Function>());
		Nan::AsyncQueueWorker(new unlinkatWorker(info, callback,dirfd, *path, flags));
	} else {
		long ret = lkl_sys_unlinkat(dirfd, *path, flags);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

// MACHINE GENERATED; DO NOT EDIT

class writeWorker : public Nan::AsyncWorker {
	public:
		writeWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback,int fd, void *p, int p_len)
		: Nan::AsyncWorker(callback),fd(fd), p(p),
					p_len(p_len)		{
			SaveToPersistent(1u, info[1]);
		}

		~writeWorker() {}

		void Execute () {
			ret = lkl_sys_write(fd, p, p_len);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

			if (callback) {
				if (ret < 0) {
					v8::Local<v8::Value> argv[] = {
						Nan::New<v8::Number>(-ret)
					};
					callback->Call(1, argv);
				} else {
					v8::Local<v8::Value> argv[] = {
						Nan::Null(),
						Nan::New<v8::Number>(ret)
					};
					callback->Call(2, argv);
				}
			}
		}

	private:
		int fd;
		void *p;
		int p_len;
		int ret;
};

NAN_METHOD(write) {
	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	int fd = info[0]->Uint32Value();
	void *p = node::Buffer::Data(info[1]->ToObject());
	int p_len = node::Buffer::Length(info[1]->ToObject());

	if (info[2]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[2].As<v8::Function>());
		Nan::AsyncQueueWorker(new writeWorker(info, callback,fd, p, p_len));
	} else {
		long ret = lkl_sys_write(fd, p, p_len);

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}


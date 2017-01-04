extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "node_lkl.h"

#define LKL_MEMORY 100 * 1024 * 1024

unsigned int disk_id;

NAN_METHOD(mount) {
	bool ro;
	char mpoint[32];
	struct lkl_disk disk;
	long ret;

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	lkl_host_ops.print = NULL;

	disk.fd = info[0]->Uint32Value();
	ro = info[1]->BooleanValue();

	disk_id = lkl_disk_add(&disk);
	lkl_start_kernel(&lkl_host_ops, LKL_MEMORY, "");

	ret = lkl_mount_dev(disk_id, 0, "ext4", ro ? LKL_MS_RDONLY : 0, NULL, mpoint, sizeof(mpoint));

	lkl_sys_chroot(mpoint);
}

NAN_METHOD(umount) {
	lkl_sys_sync();
	lkl_umount_dev(disk_id, 0, 0, 1000);
	lkl_sys_halt();
}

class SyscallWorker : public Nan::AsyncWorker {
	public:
		SyscallWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback)
		: Nan::AsyncWorker(callback) {
			no = info[0]->IntegerValue();

			for (int i = 0; i < 6; i++) {
				if (node::Buffer::HasInstance(info[i + 1])) {
					params[i] = node::Buffer::Data(info[i + 1]->ToObject());
					params[i + 1] = node::Buffer::Length(info[i + 1]->ToObject());
					SaveToPersistent(i, info[i + 1]);
					i += 2;
				} if (info[i + 1]->IsString()) {
					Nan::Utf8String path(info[i + 1]);
					strncpy(paths[i], *path, LKL_PATH_MAX);
					params[i] = paths[i];
				} else {
					params[i] = info[i + 1]->IntegerValue();
				}
			}
		}

		~SyscallWorker() {}

		void Execute () {
			ret = lkl_syscall(no, params);
		}

		void HandleOKCallback () {
			Nan::HandleScope scope;

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

		long ret;
	private:
		long no;
		long params[6];
		char paths[6][LKL_PATH_MAX];
};

NAN_METHOD(syscall) {
	if (info.Length() != 8) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	if (info[7]->IsFunction()) {
		Nan::Callback *callback = new Nan::Callback(info[7].As<v8::Function>());
		Nan::AsyncQueueWorker(new SyscallWorker(info, callback));
	} else {
		SyscallWorker *worker = new SyscallWorker(info, NULL);
		worker->Execute();
		long ret = worker->ret;

		if (ret < 0) {
			Nan::ThrowError(Nan::ErrnoException(-ret));
		} else {
			info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
		}
	}
}

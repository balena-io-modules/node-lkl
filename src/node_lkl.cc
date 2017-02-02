extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "node_lkl.h"

NAN_METHOD(startKernel) {
	if (info.Length() != 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	lkl_host_ops.print = NULL;
	int memory = info[0]->Uint32Value();
	lkl_start_kernel(&lkl_host_ops, memory, "");
}

NAN_METHOD(haltKernel) {
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
					Nan::ErrnoException(-ret)
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

NAN_METHOD(parseDirent64) {
	// Args:
	// * Buffer buffer
	// * int nread
	// * Array result
	// Parses buffer (which should be a lkl_linux_dirent64*) and fills the
	// result array with filenames it finds.
	// Filenames are Buffers that need to be decoded by the caller.
	char* buf = (char*) node::Buffer::Data(info[0]);
	int nread = info[1]->IntegerValue();
	v8::Local<v8::Array> result = info[2].As<v8::Array>();

	int posInResult = result->Length();
	int bpos;
	lkl_linux_dirent64 *dir_entry;

	for (bpos = 0; bpos < nread;) {
		dir_entry = (lkl_linux_dirent64 *) (buf + bpos);
		if (
			(strcmp(dir_entry->d_name, ".") != 0) &&
			(strcmp(dir_entry->d_name, "..") != 0)
		) {
			result->Set(
				posInResult,
				Nan::CopyBuffer(
					dir_entry->d_name,
					strlen(dir_entry->d_name)
				).ToLocalChecked()
			);
			posInResult++;
		}
		bpos += dir_entry->d_reclen;
	}
}

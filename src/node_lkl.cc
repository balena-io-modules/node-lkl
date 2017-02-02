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

NAN_METHOD(mount) {
	bool ro;
	char mpoint[32];
	unsigned int disk_id;
	unsigned int part;
	struct lkl_disk disk;
	long ret;

	if (info.Length() != 4) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	disk.fd = info[0]->Uint32Value();
	ro = info[1]->BooleanValue();
	Nan::Utf8String fs_type(info[2]);
	part = info[3]->Uint32Value();

	disk_id = lkl_disk_add(&disk);

	ret = lkl_mount_dev(
		disk_id,
		part,
		*fs_type,
		ro ? LKL_MS_RDONLY : 0,
		NULL,
		mpoint,
		sizeof(mpoint)
	);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
		return;
	}

	v8::Local<v8::Object> ret2 = Nan::New<v8::Object>();
	Nan::Set(ret2, Nan::New<v8::String>("mountpoint").ToLocalChecked(),
				   Nan::New<v8::String>(mpoint).ToLocalChecked());
	Nan::Set(ret2, Nan::New<v8::String>("diskId").ToLocalChecked(),
				   Nan::New<v8::Number>(disk_id));
	info.GetReturnValue().Set(ret2);
}

NAN_METHOD(umount) {
	unsigned int disk_id;

	if (info.Length() != 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	disk_id = info[0]->BooleanValue();

	lkl_sys_sync();
	lkl_umount_dev(disk_id, 0, 0, 1000);
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



class ListdirWorker : public Nan::AsyncWorker {
	private:
		std::string path;
		enum Nan::Encoding enc;
		std::vector<std::string> result;
		v8::Local<v8::Array> result_;

	public:
		int ret = 0;

		ListdirWorker(char* path, enum Nan::Encoding enc, Nan::Callback *callback)
		: Nan::AsyncWorker(callback) {
			this->path = std::string(path);
			this->enc = enc;
		}

		void Execute () {
			lkl_dir *dir = lkl_opendir(this->path.c_str(), &(this->ret));
			if (this->ret < 0) {
				return;
			}

			lkl_linux_dirent64 *dir_entry = NULL;
			do {
				dir_entry = lkl_readdir(dir);
				if (dir_entry == NULL) {
					this->ret = lkl_errdir(dir);
					if (this->ret < 0) {
						lkl_closedir(dir);
						return;
					}
				} else if (
					(strcmp(dir_entry->d_name, ".") != 0) &&
					(strcmp(dir_entry->d_name, "..") != 0)
				) {
					this->result.push_back(std::string(dir_entry->d_name));
				}
			} while(dir_entry != NULL);
			this->ret = lkl_closedir(dir);
		}

		~ListdirWorker() {
		}

		v8::Local<v8::Array> GetResult() {
			return this->result_;
		}

		void PrepareResult() {
			this->result_ = Nan::New<v8::Array>();
			v8::Local<v8::Value> filename;
			for (uint i=0; i<this->result.size(); i++) {
				filename = Nan::Encode(
					this->result.at(i).c_str(),
					this->result.at(i).length(),
					this->enc
				);
				if (filename->ToString()->Length() == 0) {
				    this->ret = -22;
					return;
				}
				this->result_->Set(i, filename);
			}
		}

		v8::Local<v8::Value> GetError() {
			return Nan::ErrnoException(-this->ret);
		}

		void HandleErrorCallback(){}

		void HandleOKCallback () {
			this->PrepareResult();
			if (this->ret < 0) {
				v8::Local<v8::Value> argv[] = {this->GetError()};
				callback->Call(1, argv);
			} else {
				v8::Local<v8::Value> argv[] = {
					Nan::Null(),
					this->GetResult()
				};
				callback->Call(2, argv);
			}
		}
};


NAN_METHOD(listdir) {
	// args: path, encoding, callback
	Nan::Callback *callback = NULL;
	if (info[2]->IsFunction()) {
		callback = new Nan::Callback(info[2].As<v8::Function>());
	}

	// Check that path is a string or a buffer.
	if (!info[0]->IsString() && !node::Buffer::HasInstance(info[0])) {
		v8::Local<v8::Value> error = Nan::TypeError(
			"TypeError: Path must be a string or a Buffer."
		);
		if (callback != NULL) {
			v8::Local<v8::Value> argv[] = {error};
			callback->Call(1, argv);
		} else {
			Nan::ThrowError(error);
		}
		return;
	}

	Nan::Utf8String path(info[0]);
	Nan::Utf8String encoding(info[1]);
	enum Nan::Encoding enc = static_cast<Nan::Encoding>(
		ParseEncoding(info.GetIsolate(), info[1], node::BINARY)
	);

	if (callback != NULL) {
		Nan::AsyncQueueWorker(new ListdirWorker((char*)*path, enc, callback));
	} else {
		ListdirWorker *worker = new ListdirWorker((char*)*path, enc, NULL);
		worker->Execute();
		worker->PrepareResult();
		if (worker->ret < 0) {
			Nan::ThrowError(worker->GetError());
		} else {
			info.GetReturnValue().Set(worker->GetResult());
		}
	}
}

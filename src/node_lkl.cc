extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "node_lkl.h"
#include "async.h"

NAN_METHOD(startKernel) {
	// FIXME: we should prevent this from being called twice.
	if (info.Length() != 1) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	lkl_host_ops.print = NULL;
	int memory = info[0]->Uint32Value();
	init_async();
	lkl_start_kernel(&lkl_host_ops, memory, "");
}

NAN_METHOD(haltKernel) {
	// FIXME: we should prevent this from being called twice.
	lkl_sys_halt();
	close_async();
}

class SyscallWorker : public Nan::AsyncWorker {
	public:
		SyscallWorker(Nan::NAN_METHOD_ARGS_TYPE info, Nan::Callback *callback)
		: Nan::AsyncWorker(callback) {
			no = info[0]->IntegerValue();

			for (int i = 0; i < 6; i++) {
				if (node::Buffer::HasInstance(info[i + 1])) {
					params[i] = node::Buffer::Data(info[i + 1]);
				} else if (info[i + 1]->IsString()) {
					Nan::Utf8String path(info[i + 1]);
					assert(strlen(*path) <= LKL_PATH_MAX);
					strcpy(paths[i], *path);
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
	if (!info[7]->IsFunction()) {
		Nan::ThrowTypeError("A callback is required");
		return;
	}
	Nan::Callback *callback = new Nan::Callback(info[7].As<v8::Function>());
	Nan::AsyncQueueWorker(new SyscallWorker(info, callback));
}

NAN_METHOD(parseDirent64) {
	// Args:
	// * Buffer buffer
	// Parses buffer (which should be a lkl_linux_dirent64*) and returns an
	// array with the filenames it finds.
	// Filenames are Buffers that need to be decoded by the caller.
	char* buf = (char*) node::Buffer::Data(info[0]);
	unsigned int length = node::Buffer::Length(info[0]);
	v8::Local<v8::Array> result = Nan::New<v8::Array>();

	unsigned int posInResult = result->Length();
	unsigned int bpos;
	lkl_linux_dirent64 *dir_entry;

	for (bpos = 0; bpos < length;) {
		dir_entry = (lkl_linux_dirent64 *) (buf + bpos);
		if (
			(strcmp(dir_entry->d_name, ".") != 0) &&
			(strcmp(dir_entry->d_name, "..") != 0)
		) {
			result->Set(
				posInResult,
				Nan::CopyBuffer(
					dir_entry->d_name,
					strnlen(dir_entry->d_name, LKL_NAME_MAX)
				).ToLocalChecked()
			);
			posInResult++;
		}
		bpos += dir_entry->d_reclen;
	}
	info.GetReturnValue().Set(result);
}

NAN_METHOD(sizeOfStructLklStat) {
	auto size = Nan::New<v8::Uint32>(
		static_cast<uint32_t>(sizeof(struct lkl_stat))
	);
	info.GetReturnValue().Set(size);
}

v8::Local<v8::Value> castUint32(long unsigned int x) {
	return Nan::New<v8::Uint32>(static_cast<uint32_t>(x));
}

v8::Local<v8::Value> castInt32(long int x) {
	return Nan::New<v8::Int32>(static_cast<int32_t>(x));
}

v8::Local<v8::Value> timespecToMilliseconds(time_t seconds, long nanoseconds) {
    return Nan::New<v8::Number>(
        (static_cast<double>(seconds) * 1000) +
        (static_cast<double>(nanoseconds / 1000000))
    );
}

NAN_METHOD(parseLklStat) {
	// Args:
	// * Buffer buffer
	// * function Stats
	// Parses buffer (which should be a struct lkl_stat*) and returns a Stats
	// object.
	auto *s = reinterpret_cast<lkl_stat*>(node::Buffer::Data(info[0]));
	auto ctim_msec = timespecToMilliseconds(s->lkl_st_ctime, s->st_ctime_nsec);
	v8::Local<v8::Value> argv[] = {
		castInt32(s->st_dev),
		castInt32(s->st_mode),
		castInt32(s->st_nlink),
		castUint32(s->st_uid),
		castUint32(s->st_gid),
		castInt32(s->st_rdev),
		castInt32(s->st_blksize),
		castUint32(s->st_ino),
		castUint32(s->st_size),
		castUint32(s->st_blocks),
		timespecToMilliseconds(s->lkl_st_atime, s->st_atime_nsec),
		timespecToMilliseconds(s->lkl_st_mtime, s->st_mtime_nsec),
		ctim_msec,
		ctim_msec, // birthtim same as ctime (as in libuv)
	};
	auto stats = Nan::NewInstance(
		info[1].As<v8::Function>(),
		14,
		argv
	).ToLocalChecked();
	info.GetReturnValue().Set(stats);
}

NAN_METHOD(millisecondsToTimespec) {
	auto *ts = new struct timespec;
	auto milliseconds = info[0]->IntegerValue();
	ts->tv_sec = milliseconds / 1000;
	ts->tv_nsec = (milliseconds % 1000) * 1000000;
	auto result = Nan::NewBuffer(
		reinterpret_cast<char*>(ts),
		sizeof *ts
	).ToLocalChecked();
	info.GetReturnValue().Set(result);
}

NAN_METHOD(buffersToIoVecs) {
	auto arr = info[0].As<v8::Array>();
	auto len = arr->Length();
	auto *iovecs = new struct iovec[len];
	for (unsigned int i = 0; i < len; i++) {
		auto buf = arr->Get(i);
		iovecs[i].iov_base = reinterpret_cast<char*>(node::Buffer::Data(buf));
		iovecs[i].iov_len = node::Buffer::Length(buf);
	}
	auto result = Nan::NewBuffer(
		reinterpret_cast<char*>(iovecs),
		sizeof *iovecs
	).ToLocalChecked();
	info.GetReturnValue().Set(result);
}

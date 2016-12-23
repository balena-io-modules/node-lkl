extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "syscall.h"

NAN_METHOD(open) {
	if (info.Length() < 3) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	char *path = node::Buffer::Data(info[0]->ToObject());
	int flags = info[1]->Uint32Value();
	int mode = info[2]->Uint32Value();

	long ret = lkl_sys_open(path, flags, mode);

	if (ret < 0) {
		Nan::ThrowError(Nan::ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(Nan::New<v8::Number>(ret));
	}
}

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

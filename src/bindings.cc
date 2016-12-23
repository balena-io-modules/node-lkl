#include "syscall.h"
#include "node_lkl.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("mount").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(mount)).ToLocalChecked());
  Nan::Set(target, Nan::New("umount").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(umount)).ToLocalChecked());
  Nan::Set(target, Nan::New("open").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(open)).ToLocalChecked());
  Nan::Set(target, Nan::New("close").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(close)).ToLocalChecked());
}

NODE_MODULE(bindings, InitAll)

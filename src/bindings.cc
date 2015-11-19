#include "node_lkl.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("startKernel").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(startKernel)).ToLocalChecked());
}

NODE_MODULE(bindings, InitAll)

#include "node_lkl.h"

NAN_MODULE_INIT(InitAll) {
	NAN_EXPORT(target, startKernel);
	NAN_EXPORT(target, haltKernel);
	NAN_EXPORT(target, mount);
	NAN_EXPORT(target, umount);
	NAN_EXPORT(target, syscall);
	NAN_EXPORT(target, listdir);
}

NODE_MODULE(bindings, InitAll)

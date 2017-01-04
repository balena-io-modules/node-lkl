#include "node_lkl.h"

NAN_MODULE_INIT(InitAll) {
	NAN_EXPORT(target, mount);
	NAN_EXPORT(target, umount);
	NAN_EXPORT(target, syscall);
}

NODE_MODULE(bindings, InitAll)

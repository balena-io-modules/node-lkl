#include "async.h"
#include "node_lkl.h"
#include "disk.h"

NAN_MODULE_INIT(InitAll) {
	init_async();

	NAN_EXPORT(target, startKernel);
	NAN_EXPORT(target, haltKernel);
	NAN_EXPORT(target, mount);
	NAN_EXPORT(target, umount);
	NAN_EXPORT(target, syscall);
}

NODE_MODULE(bindings, InitAll)

#include "node_lkl.h"
#include "disk.h"

NAN_MODULE_INIT(InitAll) {
	NAN_EXPORT(target, startKernel);
	NAN_EXPORT(target, haltKernel);
	NAN_EXPORT(target, mount);
	NAN_EXPORT(target, umount);
	NAN_EXPORT(target, disk_add);
	NAN_EXPORT(target, disk_remove);
	NAN_EXPORT(target, syscall);
	NAN_EXPORT(target, parseDirent64);
	NAN_EXPORT(target, sizeOfStructLklStat);
	NAN_EXPORT(target, parseLklStat);
	NAN_EXPORT(target, millisecondsToTimespec);
	NAN_EXPORT(target, buffersToIoVecs);
}

NODE_MODULE(bindings, InitAll)

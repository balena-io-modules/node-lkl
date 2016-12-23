extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "node_lkl.h"

#define LKL_MEMORY 100 * 1024 * 1024

unsigned int disk_id;

NAN_METHOD(mount) {
	bool ro;
	char mpoint[32];
	struct lkl_disk disk;
	long ret;

	if (info.Length() < 2) {
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	lkl_host_ops.print = NULL;

	disk.fd = info[0]->Uint32Value();
	ro = info[1]->BooleanValue();

	disk_id = lkl_disk_add(&disk);
	lkl_start_kernel(&lkl_host_ops, LKL_MEMORY, "");

	ret = lkl_mount_dev(disk_id, 0, "ext4", ro ? LKL_MS_RDONLY : 0, NULL, mpoint, sizeof(mpoint));

	lkl_sys_chroot(mpoint);
}

NAN_METHOD(umount) {
	lkl_sys_sync();
	lkl_umount_dev(disk_id, 0, 0, 1000);
	lkl_sys_halt();
}

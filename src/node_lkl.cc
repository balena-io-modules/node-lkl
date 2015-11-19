extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include "node_lkl.h"

NAN_METHOD(startKernel) {
	unsigned int mem_size = info[0]->Uint32Value();
	lkl_start_kernel(&lkl_host_ops, mem_size, "");
}

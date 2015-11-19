var lkl = require('../');
var assert = require('assert');

describe('lkl', function() {
	// it('should create an ext4 backstore', function() {
	//   var backstore = lkl.backStoreSync('./fixtures/test.ext4');
	//   var diskId = lkl.diskAddSync(backstore);
	// });

	it('should start the kernel', function() {
	lkl.startKernel(10 * 1024 * 1024)
	});

	// lkl.startKernelSync(hostOps, memory, cmdline);
	// lkl.mountDevSync(diskId, "btrfs", lkl.LKL_O_RDONLY, null);
	// 
	// fd = lkl.fs.openSync('/foobar', 'r');
	// 
	// lkl.fs.readSync(fd, dest);
	// 
	// lkl.umountDevSync(diskId, lkl.LKL_O_RDONLY, timeout);
	// 
	// lkl.sysHalt();
});

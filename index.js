var lkl_bindings = require('bindings')('bindings');

// backstore = lkl_binding.backStoreSync('/path/to/my/disk.btrfs');
// 
// diskId = lkl_binding.diskAddSync(backstore);
// 
// lkl_binding.startKernelSync(hostOps, memory, cmdline);
// 
// lkl_binding.mountDevSync(diskId, "btrfs", lkl_binding.LKL_O_RDONLY, null);
// 
// fd = lkl_binding.fs.openSync('/foobar', 'r');
// 
// lkl_binding.fs.readSync(fd, dest);
// 
// lkl_binding.umountDevSync(diskId, lkl_binding.LKL_O_RDONLY, timeout);
// 
// lkl_binding.sysHalt();

module.exports = lkl_bindings;

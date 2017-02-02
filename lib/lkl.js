const bindings = require('bindings')('bindings');
const constants = process.binding('constants').fs;
const fs = require('fs');

const lkl = exports;

lkl.fs = require('./fs');

lkl.startKernelSync = function(memory) {
	return bindings.startKernel(memory);
};

lkl.haltKernelSync = function() {
	return bindings.haltKernel();
};

mounts = {};

lkl.mountSync = function(path, options) {
	let flags = options.readOnly ? constants.O_RDONLY : constants.O_RDWR;
	const partition = options.partition || 0;
	let fd = fs.openSync(path, flags);
	let ret = bindings.mount(
		fd,
		Boolean(options.readOnly),
		options.fsType,
		partition
	);

	mounts[ret.mountpoint] = {
		diskId: ret.diskId,
		fd: fd
	};

	return ret.mountpoint;
};

lkl.umountSync = function(mountpoint) {
	let info = mounts[mountpoint];
	bindings.umount(info.diskId);
	fs.closeSync(info.fd);
};

const bindings = require('bindings')('bindings');

const lkl = exports;

lkl.fs = require('./fs');
lkl.disk = require('./disk');

lkl.startKernelSync = function(memory) {
	return bindings.startKernel(memory);
};

const mounts = {};

lkl.mount = function(disk, opts, callback) {
	bindings.mount(
		opts.readOnly,
		opts.filesystem,
		opts.partition,
		disk.request.bind(disk),
		disk.getCapacity.bind(disk),
		function(err, info) {
			if (err) {
				return callback(err);
			}

			mounts[info.mountpoint] = {
				diskId: info.diskId,
				opts: opts
			};

			callback(null, info.mountpoint);
		}
	);
};

lkl.umountSync = function(mountpoint) {
	const info = mounts[mountpoint];
	bindings.umount(info.diskId);
	delete mounts[mountpoint];
};

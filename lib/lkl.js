const bindings = require('bindings')('bindings');

const lkl = exports;

lkl.fs = require('./fs');
lkl.disk = require('./disk');

lkl.startKernelSync = function(memory) {
	return bindings.startKernel(memory);
};

const mounts = {};

lkl.mount = function(disk, opts, callback) {
	const partition = opts.partition || 0;

	bindings.mount(
		opts.readOnly,
		opts.filesystem,
		partition,
		disk.request.bind(disk),
		disk.getCapacity.bind(disk),
		function(err, info) {
			if (err) {
				return callback(err);
			}

			mounts[info.mountpoint] = {
				diskId: info.diskId,
				partition: partition
			};

			callback(null, info.mountpoint);
		}
	);
};

lkl.umount = function(mountpoint, callback) {
	const info = mounts[mountpoint];
	bindings.umount(info.diskId, info.partition, function(err) {
		if (err) {
			return callback(err);
		}
		delete mounts[mountpoint];
		callback(null);
	});
};

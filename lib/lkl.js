'use strict';

const bindings = require('bindings')('bindings');
const binding = require('./binding');
const util = require('util');

const errnoException = util._errnoException;
const queue = require('./queue');

const lkl = exports;

lkl.fs = require('./fs');
lkl.disk = require('./disk');

lkl.startKernelSync = function(memory) {
	bindings.startKernel(memory);
	binding.clock_settime(null, function(){});
};

lkl.haltKernelSync = function() {
	return bindings.haltKernel();
};

const mounts = {};

function mount(disk, opts, callback) {
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

function umount(mountpoint, callback) {
	const info = mounts[mountpoint];
	bindings.umount(info.diskId, info.partition, function(err) {
		if (err) {
			return callback(err);
		}
		delete mounts[mountpoint];
		callback(null);
	});
};

lkl.mount = function(disk, opts, callback) {
	queue.addOperation(mount, [disk, opts, callback]);
}

lkl.umount = function(mountpoint, callback) {
	queue.addOperation(umount, [mountpoint, callback]);
}

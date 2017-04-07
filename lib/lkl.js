'use strict';

const bindings = require('bindings')('bindings');
const binding = require('./binding');
const util = require('util');

const errnoException = util._errnoException;
const queue = require('./queue');

const lkl = exports;

lkl.fs = require('./fs');
lkl.disk = require('./disk');
lkl.utils = require('./utils');

lkl.startKernelSync = function(memory) {
	bindings.startKernel(memory);
	binding.clock_settime(null, function(){});
};

lkl.haltKernelSync = function() {
	return bindings.haltKernel();
};

const mounts = {};

function mount(diskId, opts, callback) {
	const partition = opts.partition || 0;
	bindings.mount(
		diskId,
		opts.readOnly,
		opts.filesystem,
		partition,
		function(err, mountpoint) {
			if (err) {
				callback(err);
				return;
			}
			mounts[mountpoint] = { diskId: diskId, partition: partition };
			callback(null, mountpoint);
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

lkl.diskAdd = function(disk, callback) {
	queue.addOperation(
		bindings.disk_add,
		[ disk.request.bind(disk), disk.getCapacity.bind(disk), callback ]
	);
};

lkl.diskRemove = function(diskId, callback) {
	queue.addOperation(bindings.disk_remove, [ diskId, callback ]);
};

lkl.mount = function(diskId, opts, callback) {
	queue.addOperation(mount, [diskId, opts, callback]);
}

lkl.umount = function(mountpoint, callback) {
	queue.addOperation(umount, [mountpoint, callback]);
}

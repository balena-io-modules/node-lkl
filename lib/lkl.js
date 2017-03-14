'use strict';

const bindings = require('bindings')('bindings');
const binding = require('./binding');

const queue = require('./queue');
const constants = require('./constants');

const lkl = exports;

lkl.fs = require('./fs');
lkl.disk = require('./disk');

const SUPPORTED_FILESYSTEMS = [  // TODO: get this list from lkl
	'ext4', 'ext3', 'ext2', 'btrfs', 'xfs', 'vfat'
];

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
}

function mountGuessFS(disk, opts, callback, fsIndex) {
	fsIndex = (fsIndex === undefined) ? 0 : fsIndex;
	opts.filesystem = SUPPORTED_FILESYSTEMS[fsIndex];
	mount(disk, opts, function(err, mountpoint) {
		if (err) {
			if (
				(err.errno === constants.EINVAL) &&
				(fsIndex < SUPPORTED_FILESYSTEMS.length - 1)
			) {
				return mountGuessFS(disk, opts, callback, fsIndex + 1);
			}
			return callback(err);
		}
		return callback(null, mountpoint);
	});
}

lkl.mount = function(disk, opts, callback) {
	if ((callback === undefined) && (typeof opts === 'function')) {
		callback = opts;
		opts = {};
	}
	if (SUPPORTED_FILESYSTEMS.indexOf(opts.filesystem) !== -1) {
		queue.addOperation(mount, [disk, opts, callback]);
	} else {
		queue.addOperation(mountGuessFS, [disk, opts, callback]);
	}
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
}

lkl.umount = function(mountpoint, callback) {
	queue.addOperation(umount, [mountpoint, callback]);
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

lkl.utils = require('./utils');

'use strict';

const Promise = require('bluebird');

const lkl = Promise.promisifyAll(require('./lkl'));

exports.attachDisk = function(disk) {
	// Attaches a disk and detaches it when you're done using it.
	// `disk` must be a FileDisk or a S3Disk instance from file-disk.
	// Use it with Bluebird's `using`, example:
	// Promise.using(attachDisk(disk), function(diskId) {
	//   doSomething(diskId);
	// });
	return lkl.diskAddAsync(new lkl.disk.DiskWrapper(disk))
	.disposer(function(diskId) {
		return lkl.diskRemoveAsync(diskId);
	});
};

exports.mountPartition = function(diskId, options) {
	// Mounts a partition and umounts it when you're done using it.
	// `diskId` is a diskId returned by `lkl.diskAdd` or `attachDisk`
	// `options` is a dict with the following keys:
	// - `readOnly`: [boolean];
	// - `partition`: [integer] the partition number to use. If none is given
	//   the whole disk is considered as the partition;
	// - `filesystem`: [string] any supported filesystem (vfat, ext{2,3,4},
	//   btrfs or xfs).
	// Use it with Bluebird's `using`, example:
	// Promise.using(mountPartition(diskId, {filesystem: 'ext2', partition: 3}), function(mountpoint) {
	//   doSomething(mountpoint);
	// });
	return lkl.mountAsync(diskId, options)
	.disposer(function(mountpoint) {
		return lkl.umountAsync(mountpoint);
	});
};

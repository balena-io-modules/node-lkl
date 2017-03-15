const Promise = require('bluebird');
const fs = Promise.promisifyAll(require('fs'));
const path = require('path');
const assert = require('assert');
const constants = require('../lib/constants');
const lkl = Promise.promisifyAll(require('../'));
lkl.fs = Promise.promisifyAll(lkl.fs);

const RAW_FS_PATH = path.join(__dirname, 'fixtures/test.ext4');
const TMP_RAW_FS_PATH = path.join(__dirname, '.tmp-test.ext4');
const DISK_PATH = path.join(__dirname, 'fixtures/disk.img');

describe('node-lkl', function() {
	it('should start the kernel', function() {
		lkl.startKernelSync(20 * 1024 * 1024);
	});

	after('stop the kernel', function() {
		lkl.haltKernelSync();
	})

	describe('MBR disk image', function() {
		before(function() {
			this.disk = new lkl.disk.FileDisk(DISK_PATH);
		});

		it('should be able to mount 4 partitions', function(done) {
			const promises = [
				lkl.mountAsync(this.disk, { filesystem: 'vfat', partition: 1 }),
				lkl.mountAsync(this.disk, { filesystem: 'ext2', partition: 2 }),
				lkl.mountAsync(this.disk, { filesystem: 'ext4', partition: 3 }),
				lkl.mountAsync(this.disk, { filesystem: 'btrfs', partition: 5 }),
			];
			return Promise.all(promises)
			.then(function(mountpoints) {
				console.log('mounted', mountpoints);
				return mountpoints
			})
			.then(function(mountpoints) {
				const promises = mountpoints.map(function(mountpoint) {
					return lkl.umountAsync(mountpoint);
				});
				return Promise.all(promises);
			})
			.then(function() {
				done();
			});
		});
	});
});

const lkl = require('../');
const fs = require('fs');
const path = require('path');
const assert = require('assert');
const async = require('async');
const zlib = require('zlib');

let mountpoint;

describe('lkl', function() {
	before(function(done) {
		const self = this;
		this.disk = __dirname + '/tmp.ext4';  // ext4 partition
		this.partitionnedDisk = __dirname + '/tmp.img';  // whole disk
		this.partitions = [  // whole disk partitions
			{ number: 1, fsType: 'btrfs' },
			{ number: 2, fsType: 'ext2' },
			{ number: 3, fsType: 'ext4' },
			// 4 is an extended partition
			{ number: 5, fsType: 'vfat' },  // fat16
			{ number: 6, fsType: 'vfat' },  // fat32
			{ number: 7, fsType: 'xfs' },
		]

		// create a copy of the ext4 partition
		fs.createReadStream(__dirname + '/fixtures/test.ext4')
		.pipe(fs.createWriteStream(this.disk))
		.on('close', function() {
			// then create a copy of the partitionned disk.
			// The disk image is compressed in order to avoid github's file
			// size restrictions.
			fs.createReadStream(__dirname + '/fixtures/disk.img.gz')
			.pipe(zlib.createGunzip())
			.pipe(fs.createWriteStream(self.partitionnedDisk))
			.on('close', done);
		});
	});

	after(function(done) {
		const self = this;
		// umount all partitions of the partitionned disk
		for (let info of this.partitions) {
			lkl.umountSync(info.mountpoint)
		}
		// umount the ext4 partition
		lkl.umountSync(mountpoint)
		// delete the ext4 partition copy
		fs.unlink(this.disk, function() {
			// then delete the partitionned disk copy
			fs.unlink(self.partitionnedDisk, done);
		});
	});

	it('should start the kernel', function(done) {
		lkl.startKernelSync(10 * 1024 * 1024);

		// Mount the ext4 partition.
		mountpoint = lkl.mountSync(this.disk, {readOnly: false, fsType: 'ext4'});
		// Mount all the paritions of the partitionned disk.
		for (let info of this.partitions) {
			info.mountpoint = lkl.mountSync(
				this.partitionnedDisk,
				{ partition: info.number, fsType: info.fsType }
			)
		}

		function copyFile(source, destination, callback) {
			fs.createReadStream(source)
			.pipe(lkl.fs.createWriteStream(destination))
			.on('close', callback);
		}

		const sourceFile = __dirname + '/index.js';
		// Write a file on the ext4 partition.
		const calls = [];
		calls.push(function(callback) {
			copyFile(sourceFile, mountpoint + '/index.js', callback);
		});
		// Write a file on each partition of the partitionned drive.
		for (let info of this.partitions) {
			calls.push(function(callback) {
				copyFile(sourceFile, info.mountpoint + '/index.js', callback);
			});
		}
		// Wait for all writes to complete
		async.parallel(calls, done);
	});

	describe('fs', function() {
		describe('.access()', function() {
			let removeFile = function(file) {
				try {
					lkl.fs.unlinkSync(file);
				} catch (err) {
					// Ignore error
				}
			};

			let createFileWithPerms = function(file, mode) {
				removeFile(file);
				lkl.fs.writeFileSync(file, '');
				lkl.fs.chmodSync(file, mode);
			};

			before(function() {
				this.doesNotExist = path.join(mountpoint, '__this_should_not_exist');
				this.readOnlyFile = path.join(mountpoint, 'read_only_file');
				this.readWriteFile = path.join(mountpoint, 'read_write_file');

				createFileWithPerms(this.readOnlyFile, 0o444);
				createFileWithPerms(this.readWriteFile, 0o666);
			});

			after(function() {

			});

			it('non existent file', function(done) {
				lkl.fs.access(this.doesNotExist, function(err) {
					assert.notEqual(err, null, 'error should exist');
					// assert.strictEqual(err.code, 'ENOENT');
					// assert.strictEqual(err.path, doesNotExist);
					done();
				});
			});

			it('readonly file', function(done) {
				lkl.fs.access(this.readOnlyFile, fs.F_OK | fs.R_OK, function(err) {
					assert.strictEqual(err, null, 'error should not exist');
					done();
				});
			});

			it('readonly file 2', function(done) {
				fs.access(this.readOnlyFile, fs.W_OK, (err) => {
					assert.notEqual(err, null, 'error should exist');
					assert.strictEqual(err.path, this.readOnlyFile);
					done();
				});
			});
		});
	});
});

const lkl = require('../');
const fs = require('fs');
const path = require('path');
const assert = require('assert');
const async = require('async');
const zlib = require('zlib');


describe('lkl', function() {

	before('start the kernel', function() {
		lkl.startKernelSync(10 * 1024 * 1024);
	})

	after('stop the kernel', function() {
		lkl.haltKernelSync();
	})

	describe('.fs', function() {
		const ext4ImageCopy = __dirname + '/tmp.ext4';
		const diskImageCopy = __dirname + '/tmp.img';
		const partitions = [  // whole disk partitions
			{ number: 1, fsType: 'btrfs' },
			{ number: 2, fsType: 'ext2' },
			{ number: 3, fsType: 'ext4' },
			// 4 is an extended partition
			{ number: 5, fsType: 'vfat' },  // fat16
			{ number: 6, fsType: 'vfat' },  // fat32
			{ number: 7, fsType: 'xfs' },
		];
		let mountpoint;
		const testString = 'some string 😎\n';
		const testFilename = 'filename';

		before('copy disk images and mount them', function(done) {
			// Create a copy of the ext4 partition
			this.timeout(4000)
			fs.writeFileSync(
				ext4ImageCopy,
				fs.readFileSync(__dirname + '/fixtures/test.ext4')
			);
			// then mount it.
			mountpoint = lkl.mountSync(
				ext4ImageCopy,
				{readOnly: false, fsType: 'ext4'}
			);
			// Create a copy of the partitionned disk.
			// The disk image is compressed in order to avoid github's file
			// size restrictions.
			fs.createReadStream(__dirname + '/fixtures/disk.img.gz')
			.pipe(zlib.createGunzip())
			.pipe(fs.createWriteStream(diskImageCopy))
			.on('close', function() {
				// Mount all the paritions of the partitionned disk.
				for (let info of partitions) {
					info.mountpoint = lkl.mountSync(
						diskImageCopy,
						{ partition: info.number, fsType: info.fsType }
					);
				}
				done();
			});
		});

		after('umount disk images and remove copies', function(done) {
			this.timeout(3000);
			// Umount all partitions of the partitionned disk.
			for (let info of partitions) {
				lkl.umountSync(info.mountpoint)
			}
			// Umount the ext4 partition.
			lkl.umountSync(mountpoint)
			// Delete the ext4 partition copy.
			fs.unlinkSync(ext4ImageCopy);
			// Delete the partitionned disk copy.
			fs.unlinkSync(diskImageCopy);
			done();
		});

		describe('.createWriteStream()', function() {
			it('should write to files', function(done) {

				function writeToFile(s, destination, callback) {
					lkl.fs.createWriteStream(destination).write(s, callback);
				}

				// Write a file on the ext4 partition.
				const calls = [];
				calls.push(function(callback) {
					writeToFile(
						testString,
						mountpoint + '/' + testFilename,
						callback
					);
				});
				// Write a file on each partition of the partitionned drive.
				for (let info of partitions) {
					calls.push(function(callback) {
						writeToFile(
							testString,
							info.mountpoint + '/' + testFilename,
							callback
						);
					});
				}
				// Wait for all writes to complete
				async.parallel(calls, done);
			});
		});

		describe('.createReadStream()', function() {
			it('should read from files', function(done) {
				function checkFileContents(path, content, callback) {
					const input = lkl.fs.createReadStream(path);
					const chunks = [];
					let length = 0;
					input.on('data', function(d) {
						length += d.length;
						chunks.push(d);
					});
					input.on('end', function() {
						const buf = Buffer.concat(chunks, length)
						assert.strictEqual(buf.toString('utf8'), content);
						callback();
					});
				}

				// Read the file on the ext4 partition.
				const calls = [];
				calls.push(function(callback) {
					checkFileContents(
						mountpoint + '/' + testFilename,
						testString,
						callback
					)
				});
				// Read files on each partition of the partitionned drive.
				for (let info of partitions) {
					calls.push(function(callback) {
						checkFileContents(
							info.mountpoint + '/' + testFilename,
							testString,
							callback
						)
					});
				}
				// Wait for all reads to complete
				async.parallel(calls, done);
			});
		});

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

			it('should return an error for non existent files', function(done) {
				lkl.fs.access(this.doesNotExist, function(err) {
					assert.notEqual(err, null, 'error should exist');
					// assert.strictEqual(err.code, 'ENOENT');
					assert.strictEqual(err.path, this.doesNotExist);
					done();
				});
			});

			it('should not return an error for readonly files', function(done) {
				lkl.fs.access(this.readOnlyFile, fs.F_OK | fs.R_OK, function(err) {
					assert.strictEqual(err, null, 'error should not exist');
					done();
				});
			});

			it('should return an error for a write access to a readonly file', function(done) {
				fs.access(this.readOnlyFile, fs.W_OK, (err) => {
					assert.notEqual(err, null, 'error should exist');
					assert.strictEqual(err.path, this.readOnlyFile);
					done();
				});
			});
		});
	});
});

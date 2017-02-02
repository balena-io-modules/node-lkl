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
		// Please keep testFilename one character long: it will be encoded as
		// utf8 and invalid when read as utf16 (UCS-2).
		const testFilename = 'x'
		const ext4Files = [ testFilename, 'lost+found', 'petrosagg2' ].sort();
		const diskFiles = [ testFilename, 'some file.txt' ].sort()

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

		describe('.readdirSync()', function() {
			it('should list files', function(done) {
				// ext4 partition
				assert.deepEqual(
					lkl.fs.readdirSync(mountpoint).sort(),
					ext4Files
				);
				// all partitions of the disk
				const extFiles = diskFiles.concat([ 'lost+found' ]).sort()
				for (let info of partitions) {
					assert.deepEqual(
						lkl.fs.readdirSync(info.mountpoint).sort(),
						info.fsType.startsWith('ext') ? extFiles : diskFiles
					);
				}
				done()
			});

			it('should raise an error for non existent folders', function(done) {
				try {
					lkl.fs.readdirSync(mountpoint + '/no_such_folder');
					assert(0);
				} catch (err) {
					assert.strictEqual(err.errno, 2);
					assert.strictEqual(err.code, 'ENOENT');
					done();
				}
			});

			it('should raise an error if no path is given', function(done) {
				try {
					lkl.fs.readdirSync();
					assert(0);
				} catch (err) {
					assert.strictEqual(
						err.message,
						'TypeError: Path must be a string or a Buffer.'
					);
					done();
				}
			});

			it('should accept Buffer objects as path', function(done) {
				const buf = new Buffer.from(mountpoint, 'utf8');
				assert.deepEqual(lkl.fs.readdirSync(buf).sort(), ext4Files);
				done();
			});

			it('should respect the encoding option', function(done) {
				assert.deepEqual(
					lkl.fs.readdirSync(mountpoint, 'buffer').sort(),
					ext4Files.map(function(v) {
						return new Buffer.from(v, 'utf8');
					})
				);
				done();
			});

			it('should not accept a Number as path', function(done) {
				try {
					lkl.fs.readdirSync(42);
					assert(0);
				} catch (err) {
					assert.strictEqual(
						err.message,
						'TypeError: Path must be a string or a Buffer.'
					);
				}
				done();
			});

			it(
				"should raise an error if it can't read filenames with the requested encoding",
				function(done) {
					try {
						lkl.fs.readdirSync(mountpoint, 'ucs2')
						assert(0);
					} catch (err) {
						assert.strictEqual(err.errno, 22);
						assert.strictEqual(err.code, 'EINVAL');
					}
					done();
				}
			);
		});

		describe('.readdir()', function() {
			it('should list files', function(done) {
				// ext4 partition
				lkl.fs.readdir(mountpoint, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), ext4Files);
					// btrfs partition
					lkl.fs.readdir(partitions[0].mountpoint, {}, function(err, result) {
						assert.strictEqual(err, null);
						assert.deepEqual(result.sort(), diskFiles);
						done();
					});
				});
			});

			it('should raise an error for non existent folders', function(done) {
				const folder = mountpoint + '/no_such_folder'
				lkl.fs.readdir(folder, {}, function(err, result) {
					assert.strictEqual(err.errno, 2);
					assert.strictEqual(err.code, 'ENOENT');
					assert.strictEqual(result, undefined);
					done();
				});
			});

			it('should raise an error if no path is given', function(done) {
				lkl.fs.readdir(undefined, {}, function(err, result) {
					assert.strictEqual(
						err.message,
						'TypeError: Path must be a string or a Buffer.'
					);
					done();
				});
			});

			it('should not accept a Number as path', function(done) {
				lkl.fs.readdir(42, {}, function(err, result) {
					assert.strictEqual(result, undefined);
					assert.strictEqual(
						err.message,
						'TypeError: Path must be a string or a Buffer.'
					);
					done();
				});
			});

			it('should accept Buffer objects as path', function(done) {
				const buf = new Buffer.from(mountpoint, 'utf8');
				lkl.fs.readdir(buf, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), ext4Files);
					done();
				});
			});

			it('should respect the encoding option', function(done) {
				lkl.fs.readdir(mountpoint, 'buffer', function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(
						result.sort(),
						ext4Files.map(function(v) {
							return new Buffer.from(v, 'utf8');
						})
					);
					done();
				});
			});

			it(
				"should raise an error if it can't read filenames with the requested encoding",
				function(done) {
					lkl.fs.readdir(mountpoint, 'ucs2', function(err, result) {
					    assert.strictEqual(result, undefined);
						assert.strictEqual(err.errno, 22);
						assert.strictEqual(err.code, 'EINVAL');
					    done();
					});
				}
			);
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

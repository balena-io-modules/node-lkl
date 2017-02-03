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

	describe("disks", function() {
		describe('raw filesystem image', function() {
			before(function() {
				this.disk = new lkl.disk.FileDisk(RAW_FS_PATH);
			});

			it('should mount', function() {
				return lkl.mountAsync(this.disk, { readOnly: true, filesystem: 'ext4'})
				.then(lkl.umountAsync);
			});
		});

		describe('MBR disk image', function() {
			before(function() {
				this.disk = new lkl.disk.FileDisk(DISK_PATH);
			});

			it('should mount vfat', function() {
				return lkl.mountAsync(this.disk, {readOnly: true, filesystem: 'vfat', partition: 1})
				.then(lkl.umountAsync)
			});

			it('should mount ext2', function() {
				return lkl.mountAsync(this.disk, {readOnly: true, filesystem: 'ext2', partition: 2})
				.then(lkl.umountAsync)
			});

			it('should mount ext4', function() {
				return lkl.mountAsync(this.disk, {readOnly: true, filesystem: 'ext4', partition: 3})
				.then(lkl.umountAsync)
			});

			it('should mount btrfs', function() {
				return lkl.mountAsync(this.disk, {readOnly: true, filesystem: 'btrfs', partition: 5})
				.then(lkl.umountAsync)
			});

			it('should mount xfs', function() {
				return lkl.mountAsync(this.disk, {readOnly: true, filesystem: 'xfs', partition: 6})
				.then(lkl.umountAsync)
			});
		});
	});

	describe('fs', function() {
		before(function(done) {
			const self = this;

			fs.createReadStream(RAW_FS_PATH)
			.pipe(fs.createWriteStream(TMP_RAW_FS_PATH))
			.on('close', function() {
				const disk = new lkl.disk.FileDisk(TMP_RAW_FS_PATH);

				lkl.mountAsync(disk, {filesystem: 'ext4'})
				.then(function (mountpoint) {
					self.mountpoint = mountpoint;
				}).nodeify(done);
			});
		});

		after(function() {
			return lkl.umountAsync(this.mountpoint)
			.then(function() {
				return fs.unlinkAsync(TMP_RAW_FS_PATH)
			});
		});

		describe('.mkdirSync()', function() {
			it('should create folders', function(done) {
				lkl.fs.mkdirSync('/10K');
				done();
			});

			after(function(done) {
				lkl.fs.unlinkSync(mountpoint + '/' + testFilename);
				for (let info of partitions) {
					lkl.fs.unlinkSync(info.mountpoint + '/' + testFilename);
				}
				done();
			});
		});

		describe('.readdirSync()', function() {

			before(createTestFolders);
			after(deleteTestFolders);

			it('should list files', function(done) {
				// ext4 partition
				assert.deepEqual(
					lkl.fs.readdirSync(mountpoint + '/' + folder).sort(),
					files
				);
				// all partitions of the disk
				for (let info of partitions) {
					assert.deepEqual(
						lkl.fs.readdirSync(info.mountpoint + '/' + folder).sort(),
						files
					);
				}
				done()
			});

			it('should raise an error for non existent folders', function(done) {
				try {
					lkl.fs.readdirSync('/no_such_folder');
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
				const buf = new Buffer.from(mountpoint + '/' + folder, 'utf8');
				assert.deepEqual(lkl.fs.readdirSync(buf).sort(), files);
				done();
			});

			it('should respect the encoding option', function(done) {
				assert.deepEqual(
					lkl.fs.readdirSync(mountpoint + '/' + folder, 'buffer').sort(),
					files.map(function(v) {
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
						lkl.fs.readdirSync(mountpoint + '/' + folder, 'ucs2')
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

			before(createTestFolders);
			after(deleteTestFolders);

			it('should list files', function(done) {
				// ext4 partition
				lkl.fs.readdir(mountpoint + '/' + folder, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), files);
					// btrfs partition
					lkl.fs.readdir(partitions[0].mountpoint + '/' + folder, function(err, result) {
						assert.strictEqual(err, null);
						assert.deepEqual(result.sort(), files);
						done();
					});
				});
			});

			it('should raise an error for non existent folders', function(done) {
				lkl.fs.readdir('/no_such_folder', {}, function(err, result) {
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
				const buf = new Buffer.from(mountpoint + '/' + folder, 'utf8');
				lkl.fs.readdir(buf, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), files);
					done();
				});
			});

			it('should respect the encoding option', function(done) {
				lkl.fs.readdir(mountpoint + '/' + folder, 'buffer', function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(
						result.sort(),
						files.map(function(v) {
							return new Buffer.from(v, 'utf8');
						})
					);
					done();
				});
			});

			it(
				"should raise an error if it can't read filenames with the requested encoding",
				function(done) {
					lkl.fs.readdir(mountpoint + '/' + folder, 'ucs2', function(err, result) {
					    assert.strictEqual(result, undefined);
						assert.strictEqual(err.errno, 22);
						assert.strictEqual(err.code, 'EINVAL');
					    done();
					});
				}
			);
		});

		describe('.access()', function() {
			const createFileWithPerms = function(file, mode) {
				// FIXME: remove the catch clause once unlink gets implemented
				return lkl.fs.unlinkAsync(file).catch(function() {})
				.then(function() {
					return lkl.fs.writeFileAsync(file, '');
				}).then(function() {
					return lkl.fs.chmodAsync(file, mode);
				});
			};

			before(function() {
				this.doesNotExist = path.join(this.mountpoint, '__this_should_not_exist');
				this.readOnlyFile = path.join(this.mountpoint, 'read_only_file');
				this.readWriteFile = path.join(this.mountpoint, 'read_write_file');

				return Promise.all([
					createFileWithPerms(this.readOnlyFile, 0o444),
					createFileWithPerms(this.readWriteFile, 0o666)
				]);
			});

			it('non existent file', function(done) {
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

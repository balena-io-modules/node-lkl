'use strict';

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
			
			it('should be able to mount 4 partitions', function(done) {
				const promises = [
					lkl.mountAsync(this.disk, { readOnly: true, filesystem: 'vfat', partition: 1 }),
					lkl.mountAsync(this.disk, { readOnly: true, filesystem: 'ext2', partition: 2 }),
					lkl.mountAsync(this.disk, { readOnly: true, filesystem: 'ext4', partition: 3 }),
					lkl.mountAsync(this.disk, { readOnly: true, filesystem: 'btrfs', partition: 5 }),
				];
				return Promise.all(promises)
				.then(function(mountpoints) {
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

	describe('fs', function() {
		const folder = '/some folder';

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

		describe('.mkdir(), .rmdir()', function() {
			it('should create / delete folder', function(done) {
				lkl.fs.mkdir(folder, function(err) {
					assert.strictEqual(err, null);
					done();
				});
			});

			after(function(done) {
				lkl.fs.rmdir(folder, function(err) {
					assert.strictEqual(err, null);
					done();
				});
			});
		});

		describe('.readdir()', function() {

			const files = []
			for (let i = 0; i < 100; i++) {
				files.push('file' + i);
			}
			// Please keep the following filename one character long: it will be
			// encoded as utf8 and invalid when read as utf16 (UCS-2).
			files.push('x')
			files.sort();

			function touch(path) {
				return lkl.fs.openAsync(
					path,
					constants.O_WRONLY | constants.O_CREAT | constants.O_TRUNC
				)
				.then(lkl.fs.closeAsync)
			}

			function createFiles(path) {
				return Promise.each(files, function(f) {
					return touch(path + '/' + f)
				})
			}

			function deleteFiles(path) {
				return Promise.each(files, function(f) {
					return lkl.fs.unlinkAsync(path + '/' + f)
				})
			}

			before(function(done) {
				return lkl.fs.mkdirAsync(folder)
				.then(function() {
					return createFiles(folder);
				})
				.then(function() {
					done();
				});
			});

			after(function (done) {
				return deleteFiles(folder)
				.then(function() {
					return lkl.fs.rmdirAsync(folder);
				}).then(function() {
					done();
				});
			});

			it('should list files', function(done) {
				// ext4 partition
				lkl.fs.readdir(folder, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), files);
					done();
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
				const buf = new Buffer.from(folder, 'utf8');
				lkl.fs.readdir(buf, {}, function(err, result) {
					assert.strictEqual(err, null);
					assert.deepEqual(result.sort(), files);
					done();
				});
			});

			it('should respect the encoding option', function(done) {
				lkl.fs.readdir(folder, 'buffer', function(err, result) {
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
					lkl.fs.readdir(folder, 'ucs2', function(err, result) {
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
				// the catch is here in case the file doesn't exist
				return lkl.fs.unlinkAsync(file)
				.catch(function(err) {
					if (err) {
						assert.strictEqual(
							err.message,
							'ENOENT, No such file or directory'
						);
					}
				}).then(function() {
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
					assert.strictEqual(err.code, 'ENOENT');
					done();
				});
			});

			it('should not return an error for readonly files', function(done) {
				lkl.fs.access(this.readOnlyFile, fs.F_OK | fs.R_OK, function(err) {
					assert.strictEqual(err, null, 'error should not exist');
					done();
				});
			});
		});

		describe('.fstat()', function() {
			it('should return a Stats object', function(done) {
				let fd0, fd1;
				const self = this;
				lkl.fs.openAsync(path.join(self.mountpoint, 'petrosagg2'), constants.O_RDONLY)
				.then(function(fd) {
					fd0 = fd;
					return lkl.fs.fstatAsync(fd0)
				})
				.then(function(stats) {
					assert.strictEqual(stats.isFile(), true);
					assert.strictEqual(stats.isDirectory(), false);
					assert.strictEqual(stats.isBlockDevice(), false);
					assert.strictEqual(stats.isCharacterDevice(), false);
					assert.strictEqual(stats.isSymbolicLink(), false);
					assert.strictEqual(stats.isFIFO(), false);
					assert.strictEqual(stats.isSocket(), false);
					assert.strictEqual(stats.mode.toString(8), '100644');
					return lkl.fs.closeAsync(fd0);
				})
				.then(function() {
					return lkl.fs.openAsync(path.join(self.mountpoint, 'lost+found'), constants.O_RDONLY)
				})
				.then(function(fd) {
					fd1 = fd;
					return lkl.fs.fstatAsync(fd1)
				})
				.then(function(stats) {
					assert.strictEqual(stats.isFile(), false);
					assert.strictEqual(stats.isDirectory(), true);
					assert.strictEqual(stats.isBlockDevice(), false);
					assert.strictEqual(stats.isCharacterDevice(), false);
					assert.strictEqual(stats.isSymbolicLink(), false);
					assert.strictEqual(stats.isFIFO(), false);
					assert.strictEqual(stats.isSocket(), false);
					assert.strictEqual(stats.mode.toString(8), '40700');
					return lkl.fs.closeAsync(fd1);
				})
				.then(function() {
					done();
				})
			});
		});

		describe('.writeFile() and .readFile()', function() {
			it('should write and read files', function(done) {
				const fpath = path.join(this.mountpoint, 'this_is_a_filename');
				const content = 'some content 🗺';
				let fd;
				lkl.fs.writeFileAsync(fpath, content)
				.then(function() {
					return lkl.fs.readFileAsync(fpath, 'utf8');
				})
				.then(function(readContent) {
					assert.strictEqual(content, readContent, 'should read what it has written');
				})
				.then(function() {
					return lkl.fs.openAsync(fpath, constants.O_RDONLY);
				})
				.then(function(fd) {
					fd = fd;
					return lkl.fs.fstatAsync(fd);
				})
				.then(function(stats) {
					const now = Date.now();
					assert(now - stats.atime.getTime() < 10000, "access time is correct");
					assert(now - stats.mtime.getTime() < 10000, "modification time is correct");
					assert(now - stats.ctime.getTime() < 10000, "creation time is correct");
				})
				.then(function() {
					return lkl.fs.closeAsync(fd);
				})
				.then(done);
			});
		});
	});
});

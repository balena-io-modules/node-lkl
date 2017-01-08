const lkl = require('../');
const fs = require('fs');
const path = require('path');
const assert = require('assert');

let mountpoint;

describe('lkl', function() {
	before(function(done) {
		this.disk = __dirname + '/tmp.ext4';

		fs.createReadStream(__dirname + '/fixtures/test.ext4')
		.pipe(fs.createWriteStream(this.disk))
		.on('close', done);
	});

	after(function(done) {
		fs.unlink(this.disk, done)
	});

	it('should start the kernel', function(done) {
		lkl.startKernelSync(10 * 1024 * 1024);

		mountpoint = lkl.mountSync(this.disk, {readOnly: false, fsType: 'ext4'});

		let input = fs.createReadStream(__dirname + '/index.js');
		input.pipe(lkl.fs.createWriteStream(mountpoint + '/index.js'))
		.on('close', function () {
			// lkl.umountSync(mountpoint);
			done();
		});
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

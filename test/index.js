const lkl = require('../');
const fs = require('fs');

describe('lkl', function() {
	it('should start the kernel', function(done) {
		lkl.startKernelSync(10 * 1024 * 1024);

		let mountpoint = lkl.mountSync(__dirname + '/fixtures/test.ext4', {readOnly: false, fsType: 'ext4'});

		let input = fs.createReadStream(__dirname + '/index.js');
		input.pipe(lkl.fs.createWriteStream(mountpoint + '/index.js'))
		.on('close', function () {
			lkl.umountSync(mountpoint);
			done();
		});
	});
});

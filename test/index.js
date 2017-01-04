const lkl = require('../');
const fs = require('fs');

describe('lkl', function() {
	it('should start the kernel', function(done) {
		let fs2 = lkl.mount(__dirname + '/fixtures/test.ext4', false);

		let input = fs.createReadStream(__dirname + '/index.js');
		input.pipe(fs2.createWriteStream('/index.js'))
		.on('close', function () {
			fs2.umount();
			done();
		});
	});
});

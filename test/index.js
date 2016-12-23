const lkl = require('../');

describe('lkl', function() {
	it('should start the kernel', function() {
		let fs = lkl.mount(__dirname + '/fixtures/test.ext4', false);

		let fd = fs.openSync('/petrosagg2', 'w+');
		fs.closeSync(fd);

		fs.umount();
	});
});

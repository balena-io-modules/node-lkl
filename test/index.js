const lkl = require('../');

describe('lkl', function() {
	it('should start the kernel', function() {
		let fs = lkl.mount(__dirname + '/fixtures/test.ext4', false);

		let fd = fs.openSync('/petrosagg2', 'r');
		fs.closeSync(fd);

		fd = fs.openSync('/petrosagg2', 'r+');
		let foo = Buffer.alloc(9, 0)

		fs.read(fd, foo, 0, foo.length, function (err, bytes_read) {
			fs.closeSync(fd);

			let fd = fs.openSync('/petrosagg2', 'r+');
			let buf = Buffer.from("fooo");
			fs.writeSync(fd, buf, 0, buf.length, 4);

			fs.closeSync(fd);
			fs.umount();
		});
	});
});

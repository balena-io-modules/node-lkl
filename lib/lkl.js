const bindings = require('bindings')('bindings');
const fs = require('fs');

exports.mount = function (path, ro) {
	let flags = 'r';

	if (!ro) {
		flags += '+'
	}

	let fd = fs.openSync(path, flags);
	bindings.mount(fd, ro);

	lkl_fs = require('./fs');

	lkl_fs.umount = function() {
		bindings.umount();
		fs.closeSync(fd);
	}
	return lkl_fs;
}

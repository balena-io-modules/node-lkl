const syscall = require('./syscall');

exports.FSReqWrap = function () {};
exports.FSInitialize = function () {};

exports.access = function(path, mode, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	return syscall.access(path, mode, callback);
}

exports.chmod = function(path, mode, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	return syscall.chmod(path, mode, callback);
}

exports.chown = function() {
	throw new Error('Unimplemented');
}

exports.close = function(fd, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	return syscall.close(fd, callback);
}

exports.fchmod = function() {
	throw new Error('Unimplemented');
}

exports.fchown = function() {
	throw new Error('Unimplemented');
}

exports.fdatasync = function() {
	throw new Error('Unimplemented');
}

exports.fstat = function() {
	throw new Error('Unimplemented');
}

exports.fsync = function() {
	throw new Error('Unimplemented');
}

exports.ftruncate = function() {
	throw new Error('Unimplemented');
}

exports.futimes = function() {
	throw new Error('Unimplemented');
}

exports.link = function() {
	throw new Error('Unimplemented');
}

exports.lstat = function() {
	throw new Error('Unimplemented');
}

exports.mkdir = function() {
	throw new Error('Unimplemented');
}

exports.mkdtemp = function() {
	throw new Error('Unimplemented');
}

exports.open = function(path, flags, mode, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	return syscall.openat(-0x64, path, flags, mode, callback);
}

exports.read = function(fd, buffer, offset, length, position, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	if (position !== -1) {
		return syscall.pread64(fd, buffer.slice(offset, offset + length), position, callback);
	} else {
		return syscall.read(fd, buffer.slice(offset, offset + length), callback);
	}
};

exports.readdir = function() {
	throw new Error('Unimplemented');
}

exports.readlink = function() {
	throw new Error('Unimplemented');
}

exports.rename = function() {
	throw new Error('Unimplemented');
}

exports.rmdir = function() {
	throw new Error('Unimplemented');
}

exports.stat = function() {
	throw new Error('Unimplemented');
}

exports.StatWatcher = function() {
	throw new Error('Unimplemented');
}

exports.symlink = function() {
	throw new Error('Unimplemented');
}

exports.unlink = function() {
	throw new Error('Unimplemented');
}

exports.utimes = function() {
	throw new Error('Unimplemented');
}

exports.writeBuffer = function(fd, buffer, offset, length, position, req) {
	let callback = req ? req.oncomplete.bind(req) : null;
	if (position !== -1) {
		return syscall.pwrite64(fd, buffer.slice(offset, offset + length), position, callback);
	} else {
		return syscall.write(fd, buffer.slice(offset, offset + length), callback);
	}
}

exports.writeBuffers = function() {
	throw new Error('Unimplemented');
}

exports.writeString = function() {
	throw new Error('Unimplemented');
}

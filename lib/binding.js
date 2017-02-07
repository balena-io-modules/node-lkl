const syscall = require('./syscall');
const constants = require('./constants');
const readdir = require('./readdir');

function getCallback(req) {
	if (!req || (typeof req.oncomplete !== 'function')) {
		throw new Error('A callback is required.');
	}
	return req.oncomplete.bind(req);
}

exports.FSReqWrap = function () {};
exports.FSInitialize = function () {};

exports.access = function(path, mode, req) {
	return syscall.access(path, mode, getCallback(req));
}

exports.chmod = function(path, mode, req) {
	return syscall.chmod(path, mode, getCallback(req));
}

exports.chown = function() {
	throw new Error('Unimplemented');
}

exports.close = function(fd, req) {
	return syscall.close(fd, getCallback(req));
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

exports.mkdir = function(path, mode, req) {
	return syscall.mkdirat(constants.AT_FDCWD, path, mode, getCallback(req));
}

exports.mkdtemp = function() {
	throw new Error('Unimplemented');
}

exports.open = function(path, flags, mode, req) {
	return syscall.openat(
		constants.AT_FDCWD,
		path,
		flags,
		mode,
		getCallback(req)
	);
}

exports.read = function(fd, buffer, offset, length, position, req) {
	const callback = getCallback(req);
	const slice = buffer.slice(offset, offset + length);
	if (typeof position === 'number' && position !== -1) {
		return syscall.pread64(fd, slice, slice.length, position, callback);
	} else {
		return syscall.read(fd, slice, slice.length, callback);
	}
};

exports.readdir = function(path, encoding, req) {
	return readdir.readdir(path, encoding, getCallback(req));
}

exports.readlink = function() {
	throw new Error('Unimplemented');
}

exports.rename = function() {
	throw new Error('Unimplemented');
}

exports.rmdir = function(path, req) {
	return syscall.unlinkat(
		constants.AT_FDCWD,
		path,
		constants.AT_REMOVEDIR,
		getCallback(req)
	);
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

exports.unlink = function(path, req) {
	return syscall.unlinkat(constants.AT_FDCWD, path, 0, getCallback(req));
}

exports.utimes = function() {
	throw new Error('Unimplemented');
}

exports.writeBuffer = function(fd, buffer, offset, length, position, req) {
	const callback = getCallback(req);
	const slice = buffer.slice(offset, offset + length);
	if (typeof position === 'number' && position !== -1) {
		return syscall.pwrite64(fd, slice, slice.length, position, callback);
	} else {
		return syscall.write(fd, slice, slice.length, callback);
	}
}

exports.writeBuffers = function() {
	throw new Error('Unimplemented');
}

exports.writeString = function() {
	throw new Error('Unimplemented');
}

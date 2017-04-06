'use strict';

const syscall = require('./syscall');
const constants = require('./constants');
const readdir = require('./readdir');
const bindings = require('bindings')('bindings');

const STRUCT_LKL_STAT_SIZE = bindings.sizeOfStructLklStat();

function getCallback(req) {
	if (!req || (typeof req.oncomplete !== 'function')) {
		throw new Error('A callback is required.');
	}
	return req.oncomplete.bind(req);
}

function addNullTerminationIfNeeded(s) {
	// If s is a Buffer, we need to add a NULL termination byte.
	if (Buffer.isBuffer(s)) {
		const nullTerminated = Buffer.allocUnsafe(s.length + 1);
		s.copy(nullTerminated);
		nullTerminated.writeUInt8(0, s.length);
		s = nullTerminated;
	}
	return s;
}

exports.FSReqWrap = function () {};

let Stats;

exports.FSInitialize = function (s) {
	Stats = s;
};

exports.access = function(path, mode, req) {
	return syscall.access(
		addNullTerminationIfNeeded(path),
		mode,
		getCallback(req)
	);
}

exports.chmod = function(path, mode, req) {
	return syscall.chmod(
		addNullTerminationIfNeeded(path),
		mode,
		getCallback(req)
	);
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

exports.fstat = function(fd, req) {
	const buf = Buffer.alloc(STRUCT_LKL_STAT_SIZE);
	syscall.fstat(fd, buf, function(err) {
		const callback = getCallback(req);
		if (err) {
			callback(err);
		}
		callback(null, bindings.parseLklStat(buf, Stats));
	})
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
	return syscall.mkdirat(
		constants.AT_FDCWD,
		addNullTerminationIfNeeded(path),
		mode,
		getCallback(req)
	);
}

exports.mkdtemp = function() {
	throw new Error('Unimplemented');
}

exports.open = function(path, flags, mode, req) {
	return syscall.openat(
		constants.AT_FDCWD,
		addNullTerminationIfNeeded(path),
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
	return readdir.readdir(
		addNullTerminationIfNeeded(path),
		encoding,
		getCallback(req)
	);
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
		addNullTerminationIfNeeded(path),
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
	return syscall.unlinkat(
		constants.AT_FDCWD,
		addNullTerminationIfNeeded(path),
		0,
		getCallback(req)
	);
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

exports.writeBuffers = function(fd, buffers, position, req) {
	const callback = getCallback(req);
	const iovecs = bindings.buffersToIoVecs(buffers);
	if (typeof position === 'number' && position !== -1) {
		return syscall.pwritev(
			fd,
			iovecs,
			buffers.length,
			position & 0xffffffff,  // 32 lower bits
			Math.floor(position / Math.pow(2, 32)),  // high bits
			callback
		);
	} else {
		return syscall.writev(fd, iovecs, buffers.length, callback);
	}
}

exports.writeString = function() {
	throw new Error('Unimplemented');
}

exports.clock_settime = function(date, callback){
	if (typeof callback !== 'function') {
		throw new Error('A callback is required.');
	}
	date = (date instanceof Date) ? date : new Date();
	const timespec = bindings.millisecondsToTimespec(date.getTime());
	syscall.clock_settime(constants.CLOCK_REALTIME, timespec, callback);
}

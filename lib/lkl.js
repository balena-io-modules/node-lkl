const bindings = require('bindings')('bindings');

const constants = require('constants');
const fs = require('fs');

// Used by binding.open and friends
function stringToFlags(flag) {
	// Return early if it's a number
	if (typeof flag === 'number') {
		return flag;
	}

	switch (flag) {
		case 'r' : return constants.O_RDONLY;
		case 'rs' : // fall through
		case 'sr' : return constants.O_RDONLY | constants.O_SYNC;
		case 'r+' : return constants.O_RDWR;
		case 'rs+' : // fall through
		case 'sr+' : return constants.O_RDWR | constants.O_SYNC;

		case 'w' : return constants.O_TRUNC | constants.O_CREAT | constants.O_WRONLY;
		case 'wx' : // fall through
		case 'xw' : return constants.O_TRUNC | constants.O_CREAT | constants.O_WRONLY | constants.O_EXCL;

		case 'w+' : return constants.O_TRUNC | constants.O_CREAT | constants.O_RDWR;
		case 'wx+': // fall through
		case 'xw+': return constants.O_TRUNC | constants.O_CREAT | constants.O_RDWR | constants.O_EXCL;

		case 'a' : return constants.O_APPEND | constants.O_CREAT | constants.O_WRONLY;
		case 'ax' : // fall through
		case 'xa' : return constants.O_APPEND | constants.O_CREAT | constants.O_WRONLY | constants.O_EXCL;

		case 'a+' : return constants.O_APPEND | constants.O_CREAT | constants.O_RDWR;
		case 'ax+': // fall through
		case 'xa+': return constants.O_APPEND | constants.O_CREAT | constants.O_RDWR | constants.O_EXCL;
	}

	throw new Error('Unknown file open flag: ' + flag);
}

function modeNum(m, def) {
	if (typeof m === 'number')
		return m;
	if (typeof m === 'string')
		return parseInt(m, 8);
	if (def)
		return modeNum(def);
	return undefined;
}

exports.mount = function (path, ro) {
	let flags = 'r';

	if (!ro) {
		flags += '+'
	}

	let fd = fs.openSync(path, flags);
	bindings.mount(fd, ro);

	let lkl_fs = {};

	lkl_fs.openSync = function(path, flags, mode) {
		mode = modeNum(mode, 0o666);
		let fd = bindings.openat(-0x64, path, stringToFlags(flags), mode);
		return fd;
	}

	lkl_fs.readSync = function(fd, buffer, offset, length, position) {
		if (typeof position === 'number') {
			return bindings.pread64(fd, buffer.slice(offset, offset + length), position);
		} else {
			return bindings.read(fd, buffer.slice(offset, offset + length));
		}
	}

	lkl_fs.read = function(fd, buffer, offset, length, position, callback) {
		if (typeof position === 'number') {
			return bindings.pread64(fd, buffer.slice(offset, offset + length), position, callback);
		} else {
			callback = position;
			return bindings.read(fd, buffer.slice(offset, offset + length), callback);
		}
	}

	lkl_fs.writeSync = function(fd, buffer, offset, length, position) {
		if (typeof position === 'number') {
			return bindings.pwrite64(fd, buffer.slice(offset, offset + length), position);
		} else {
			return bindings.write(fd, buffer.slice(offset, offset + length));
		}
	}

	lkl_fs.closeSync = function(fd) {
		return bindings.close(fd);
	}

	lkl_fs.umount = function() {
		bindings.umount();
		fs.closeSync(fd);
	}

	return lkl_fs;
}

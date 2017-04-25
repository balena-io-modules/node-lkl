'use strict';

const syscall = require('./syscall');
const constants = require('./constants');
const bindings = require('bindings')('bindings');

const OPEN_OPTIONS = constants.O_DIRECTORY | constants.O_RDONLY;
const BUFFER_SIZE = 1024;
const NOT_STRING_OR_BUFFER_ERROR = TypeError(
	'TypeError: Path must be a string or a Buffer.'
);
const INVALID_ENCODING_ERROR = new Error(
	'EINVAL: Invalid character encoding for filename'
);
INVALID_ENCODING_ERROR.code = 'EINVAL';
INVALID_ENCODING_ERROR.errno = constants.EINVAL;

function decodeArray(arr, encoding) {
	// Decodes each Buffer in arr using the encoding argument (in place).
	let s;
	if (encoding !== 'buffer') {
		for (let i = 0; i < arr.length; i++) {
			s = arr[i].toString(encoding);
			if (s.length === 0) {
				throw INVALID_ENCODING_ERROR;
			}
			arr[i] = s;
		}
	}
}

function isStringOrBuffer(x) {
	return (Buffer.isBuffer(x) || (typeof x === 'string'));
}

function listdir(fd, buf, callback, _result) {
	// Helper for readdir (see below).
	// Fills the result array with filenames from the lkl_linux_dirent* buf
	// Recurses if needed.
	if (_result === undefined) {
		_result = [];
	}
	syscall.getdents64(fd, buf, BUFFER_SIZE, function(err, nread) {
		if (err !== null) {
			// An error occured: callback with the error.
			return callback(err);
		}
		if (nread > 0) {
			// Some bytes were read: copy from buf to result and continue
			// reading.
			_result =  _result.concat(bindings.parseDirent64(buf.slice(0, nread)));
			listdir(fd, buf, callback, _result);
		} else {
			// No bytes were read, the result is ready: callback.
			callback(null, _result);
		}
	});
}

exports.readdir = function(path, encoding, callback) {
	if (!isStringOrBuffer(path)) {
		return callback(NOT_STRING_OR_BUFFER_ERROR);
	}
	syscall.openat(constants.AT_FDCWD, path, OPEN_OPTIONS, 0, function(err, fd) {
		if (err !== null) {
			return callback(err);
		}
		const buf = Buffer.allocUnsafe(BUFFER_SIZE);
		listdir(fd, buf, function(err, result) {
			syscall.close(fd, function(closeErr) {
				err = err || closeErr;
				if (err !== null) {
					return callback(err);
				}
				try {
					decodeArray(result, encoding);
					callback(null, result);
				} catch (decodeErr) {
					callback(decodeErr);
				}
			});
		});
	});
};

const syscall = require('./syscall');
const bindings = require('bindings')('bindings');


const O_DIRECTORY = 00200000;
const O_RDONLY = 00000000;
const openOptions = O_DIRECTORY | O_RDONLY;
const bufferSize = 1024;
const notStringOrBufferError = TypeError(
	"TypeError: Path must be a string or a Buffer."
);
const invalidEncodingError = new Error(
	'EINVAL: Invalid character encoding for filename'
);
invalidEncodingError.code = 'EINVAL';
invalidEncodingError.errno = 22;


function decodeArray(arr, encoding) {
	// Decodes each Buffer in arr using the encoding argument (in place).
	let s;
	if (encoding !== 'buffer') {
		for (let i=0; i<arr.length; i++) {
			s = arr[i].toString(encoding);
			if (s.length === 0) {
				throw invalidEncodingError;
			}
			arr[i] = s;
		}
	}
}


function castPath(path) {
	if (Buffer.isBuffer(path)) {
		path = path.toString('utf8');
	}
	return path;
}


function isStringOrBuffer(x) {
	return (Buffer.isBuffer(x) || (typeof x === 'string'));
}


function listdir(fd, buf, result, callback) {
	// Helper for readdir (see below).
	// Fills the result array with filenames from the lkl_linux_dirent* buf
	// Recurses if needed.
	syscall.getdents64(fd, buf, function(err, nread) {
		if (err !== null) {
			// An error occured: callback with the error.
			callback(err);
		} else if (nread > 0) {
			// Some bytes were read: copy from buf to result and continue
			// reading.
			bindings.parseDirent64(buf, nread, result);
			listdir(fd, buf, result, callback);
		} else {
			// No bytes were read, the result is ready: callback.
			callback(null);
		}
	});
}


exports.readdir = function(path, encoding, callback) {
	if (!isStringOrBuffer(path)) {
		callback(notStringOrBufferError);
	} else {
		path = castPath(path);
		syscall.openat(-0x64, path, openOptions, 0, function(err, fd) {
			if (err !== null) {
				callback(err);
			} else {
				const result = [];
				const buf = Buffer.alloc(bufferSize);
				listdir(fd, buf, result, function(err) {
					syscall.close(fd, function(closeErr, ret) {
						err = err || closeErr;
						if (err !== null) {
							callback(err);
						} else {
							try {
								decodeArray(result, encoding);
								callback(null, result);
							} catch (decodeErr) {
								callback(decodeErr);
							}
						}
					});
				});
			}
		});
	}
}


exports.readdirSync = function(path, encoding) {
	if (!isStringOrBuffer(path)) {
		throw notStringOrBufferError;
	}
	path = castPath(path);
	const result = [];
	const buf = Buffer.alloc(bufferSize);
	const fd = syscall.openat(-0x64, path, openOptions, 0);
	let nread;
	try {
		nread = syscall.getdents64(fd, buf);
		while (nread > 0) {
			bindings.parseDirent64(buf, nread, result);
			nread = syscall.getdents64(fd, buf);
		}
	} finally {
		syscall.close(fd);
	}
	decodeArray(result, encoding);
	return result;
}

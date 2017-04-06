'use strict';

const filedisk = require('resin-file-disk');

const constants = require('./constants');

function sum(arr) {
	return arr.reduce(function(a, b) {return a + b});
}

class FileDisk extends filedisk.FileDisk {
	constructor(path) {
		super(
			path,
			{
				read: constants.LKL_DEV_BLK_TYPE_READ,
				write: constants.LKL_DEV_BLK_TYPE_WRITE,
				flush: [
					constants.LKL_DEV_BLK_TYPE_FLUSH,
					constants.LKL_DEV_BLK_TYPE_FLUSH_OUT
				]
			}
		);
	};

	request(type, offset, iovecs, callback) {
		if (iovecs.length > 1) {
			// special case for read / write requests with more than one iovec
			const length = sum(iovecs.map(function(b) {return b.length}));
			if (type === constants.LKL_DEV_BLK_TYPE_READ) {
				const buf = Buffer.allocUnsafe(length);
				const cb = function(err, bytesRead, buf) {
					if (err) {
						callback(err);
						return;
					}
					let off = 0;
					// copy from result buffer to iovecs:
					for (let vec of iovecs) {
						buf.copy(vec, 0, off, off + vec.length);
						off += vec.length;
					}
					callback(null);
				};
				super.request(type, offset, length, buf, cb);
			} else if (type === constants.LKL_DEV_BLK_TYPE_WRITE) {
				// transform all iovecs to a single buffer:
				const length = sum(iovecs.map(function(b) {return b.length}));
				const buf = Buffer.concat(iovecs, length);
				super.request(type, offset, length, buf, callback);
			}
		} else {
			super.request(type, offset, iovecs[0].length, iovecs[0], callback);
		}
	};
}

exports.FileDisk = FileDisk;

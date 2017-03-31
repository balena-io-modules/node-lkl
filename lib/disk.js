'use strict';

const filedisk = require('resin-file-disk');

const constants = require('./constants');

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
		// FIXME: we have to fill / persist all the iovecs, not just the first
		super.request(type, offset, iovecs[0].length, iovecs[0], callback);
	};
}

exports.FileDisk = FileDisk;

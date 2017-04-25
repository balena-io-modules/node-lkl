'use strict';

const filedisk = require('file-disk');

const constants = require('./constants');

class DiskWrapper extends filedisk.DiskWrapper {
	request(type, offset, iovecs, callback) {
		// FIXME: we have to fill / persist all the iovecs, not just the first
		const buf = iovecs[0];
		switch(type) {
			case constants.LKL_DEV_BLK_TYPE_READ:
				this.disk.read(buf, 0, buf.length, offset, callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_WRITE:
				this.disk.write(buf, 0, buf.length, offset, callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_FLUSH:
			case constants.LKL_DEV_BLK_TYPE_FLUSH_OUT:
				this.disk.flush(callback);
				break;
		}
	}
}

exports.DiskWrapper = DiskWrapper;

'use strict';

const Bluebird = require('bluebird');
const constants = require('./constants');

class DiskWrapper {
	constructor(disk) {
		this.disk = disk;
	}

	request(type, offset, iovecs, callback) {
		// FIXME: we have to fill / persist all the iovecs, not just the first
		const buf = iovecs[0];
		switch(type) {
			case constants.LKL_DEV_BLK_TYPE_READ:
				Bluebird.resolve(this.disk.read(buf, 0, buf.length, offset)).asCallback(callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_WRITE:
				Bluebird.resolve(this.disk.write(buf, 0, buf.length, offset)).asCallback(callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_FLUSH:
			case constants.LKL_DEV_BLK_TYPE_FLUSH_OUT:
				Bluebird.resolve(this.disk.flush()).asCallback(callback);
				break;
		}
	}

	getCapacity(callback) {
		Bluebird.resolve(this.disk.getCapacity()).asCallback(callback);
	}

	getStream(highWaterMark, callback) {
		Bluebird.resolve(this.disk.getStream(highWaterMark)).asCallback(callback);
	}
}

exports.DiskWrapper = DiskWrapper;

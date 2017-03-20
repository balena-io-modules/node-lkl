'use strict';

const EventEmitter = require('events');
const fs = require('fs');

const constants = require('./constants');

class FileDisk extends EventEmitter {
	constructor(path) {
		super();
		this.path = path;
		this.fd = null;

		this.open();
	}

	open() {
		const self = this;
		// FIXME: allow opening the file readonly
		fs.open(this.path, 'r+', function(err, fd) {
			if (err) {
				self.fd = -1;
				return;
			}
			self.fd = fd;
			self.emit('open')
		});
	};

	request(type, offset, iovecs, callback) {
		if (typeof this.fd !== 'number') {
			return this.once('open', function() {
				this.request(type, offset, iovecs, callback);
			});
		}

		switch (type) {
			case constants.LKL_DEV_BLK_TYPE_READ:
				// FIXME: we have to fill all the iovecs, not just the first
				fs.read(this.fd, iovecs[0], 0, iovecs[0].length, offset, callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_WRITE:
				// FIXME: we have to persist all the iovecs, not just the first
				fs.write(this.fd, iovecs[0], 0, iovecs[0].length, offset, callback);
				break;
			case constants.LKL_DEV_BLK_TYPE_FLUSH:
			case constants.LKL_DEV_BLK_TYPE_FLUSH_OUT:
				fs.fdatasync(this.fd, callback);
				break;
			default:
				throw new Error("Unknown request type: " + type);
		}
	};

	getCapacity(callback) {
		if (typeof this.fd !== 'number') {
			return this.once('open', function() {
				this.getCapacity(callback);
			});
		}
		fs.fstat(this.fd, function (err, stat) {
			if (err) {
				return callback(err);
			}
			callback(null, stat.size);
		});
	};
}

exports.FileDisk = FileDisk;

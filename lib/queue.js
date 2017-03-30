'use strict';

const async = require('async');

/*
Node uses a thread pool for all its fs operations.
We use the same threadpool for 3 things we do with lkl:
 * mount
 * umount
 * syscall

These 3 actions need to do fs operations.
So we have to make sure we leave at least one thread of the threadpool free to
avoid deadlocks.
For example: all threadpool threads busy with mount calls waiting for a thread
to be available for an fs operation.

NOTE: node-lkl can not work with UV_THREADPOOL_SIZE < 2 .
*/

function maxThreads() {
	let poolSize = parseInt(process.env.UV_THREADPOOL_SIZE);
	if (isNaN(poolSize)) {
		poolSize = 4;  // default in libuv
	}
	return poolSize - 1;
};

const queue = async.queue(function(task, callback) {
	const cb = task.args.pop();
	task.method(...task.args, (err, value) => {
		cb(err, value);
		callback(err);
	});
}, maxThreads());

exports.addOperation = function(method, args) {
	queue.push({ method: method, args: args })
};

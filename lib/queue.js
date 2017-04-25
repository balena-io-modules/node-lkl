'use strict';

const async = require('async');

/*
We use threads for  the following actions:
 * mount
 * umount
 * syscall

We want to avoid threads accumulating if a callback is not called so we use
only one thread at a time.
*/

const queue = async.queue(function(task, callback) {
	const cb = task.args.pop();
	task.method(...task.args, (err, value) => {
		cb(err, value);
		callback(err);
	});
}, 1);

exports.addOperation = function(method, args) {
	queue.push({ method: method, args: args });
};

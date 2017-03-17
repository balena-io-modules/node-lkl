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

const MAX_THREADS = maxThreads();

const queue = [];
let active_threads = 0;

function queuePop() {
	if ((queue.length === 0) || (active_threads >= MAX_THREADS)) {
		return;
	}
	active_threads++;
	const args = queue.pop();
	const method = args.pop();
	const callback = args.pop();
	method(...args, (err, value) => {
		active_threads--;
		callback(err, value);
		queuePop();
	});
};

exports.addOperation = function(method, args) {
	const data = args.slice()
	data.push(method)
	queue.unshift(data);
	queuePop();
};

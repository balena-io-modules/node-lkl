node-lkl
=========
### NodeJS native bindings to the Linux Kernel Library project
[![Build Status](https://travis-ci.org/resin-io/node-lkl.svg?branch=master)](https://travis-ci.org/resin-io/node-lkl)

node-lkl uses the [Linux Kernel Library project](https://github.com/lkl/linux) to provide access to filesystem drivers
from NodeJS in a cross-platform way.

Some things you can do with this module:

* Read/write files in a filesystem image directly without mounting
* Use familiar APIs, node-lkl has the exact same interface as node's `fs` module (except that only async methods work).
* Combine lkl filesystem streams with host filesystem streams (e.g copy files)
* Create a tar archive from a filesystem image

Check the [How it works](#how-it-works) section to learn how it works.

**Warning: The API exposed by this library is still forming and can change at
any time!**

Installation
------------

`node-lkl` includes a stripped down copy of the linux source code so
there's no need to have it available on your system.

Simply compile and install `node-lkl` using `npm`:

``` bash
$ npm install lkl
```

Example
-------

Here's an example of using `node-lkl` to read a file inside a disk image of
a ext4 partition and pipe it to `process.stdout`:

If you can omit the `filesystem` options parameter, all supported filesystems
will be tried.

``` javascript
const Promise = require('bluebird');
const filedisk = Promise.promisifyAll(require('file-disk'), { multiArgs: true });
const lkl = Promise.promisifyAll(require('lkl'))
lkl.fs = Promise.promisifyAll(lkl.fs);

lkl.startKernelSync(10 * 1024 * 1024);

Promise.using(filedisk.openFile('ext4', 'r'), function(fd) {
	const disk = new filedisk.FileDisk(fd, true, false);
	return Promise.using(lkl.utils.attachDisk(disk), function(diskId) {
		const options = { filesystem: 'ext4', readOnly: true};
		return Promise.using(lkl.utils.mountPartition(diskId, options), function(mountpoint) {
			const file = lkl.fs.createReadStream(mountpoint + '/some_file');
			file.pipe(process.stdout);
			return new Promise((resolve, reject) => {
				file.on("close", resolve).on("error", reject);
			});
		});
	});
})
.then(function() {
	lkl.haltKernelSync();
});
```

S3 Example
----------

```javascript
const Promise = require('bluebird');
const aws = require('aws-sdk');
const filedisk = Promise.promisifyAll(require('file-disk'));
const lkl = Promise.promisifyAll(require('lkl'));

lkl.startKernelSync(10 * 1024 * 1024);

const s3 = new aws.S3('access_key', 'secret_key');
const disk = new filedisk.S3Disk(s3, 'some-bucket', 'some-object-key');

Promise.using(lkl.utils.attachDisk(disk), function(diskId) {
	const options = { partition: 1, filesystem: 'vfat' };
	return Promise.using(lkl.utils.mountPartition(diskId, options), function(mountpoint) {
		doSomething(mountpoint);
	});
})
.then(function() {
	return disk.getStreamAsync();
}
.then(function(stream) {
	const out = fs.createWriteStream('some-file')
	stream.pipe(out)
	return new Promise(function(resolve, reject) {
		out.on('close', resolve);
		out.on('error', reject);
	});
.then(function() {
	lkl.haltKernelSync();
});
```

More examples in [tests](test/index.js).

How it works
------------

node-lkl is based on the LKL (Linux Kernel Library) project. node-lkl is
focused on the filesystem capabilities of lkl although future work could allow
usage of the kernel networking stack as well. To understand how a userspace
process can "mount" an image without actually mounting it, it helps to think
about the interface between an actual kernel and a block device.

```
 +---------------+
 |  application  |
 +-------+-------+
         | syscall interface
+--------V---------+
|      kernel      |
| +-------------+  |
| |  fs driver  |  |
| +-------+-----+  |
|         |        |
| +-------V------+ |
| | block driver | |
| +-------+------+ |
+---------|--------+
          | block request interface
   +------V-----+
   | hard drive |
   +------------+
```

Everything inside the box named `kernel` doesn't actually need the hardware
to do its job. When an application instructs the kernel to read a file, the
filesystem driver issues some read requests to the block driver and the block
driver issues those to the hard drive. All of the high level filesystem
semantics like files, directories, permissions, etc. are "compiled" down to raw
reads and writes at specific offsets.

This means that as long as we can provide a block request interface to the
kernel, and find a way to run the kernel in userspace, we can use all of its
functionality. This is how it looks like with node-lkl:

```
 +----------------------------------------------------------+
 |  nodejs application                                      |
 |                                                          |
 | +-----------------------------------------------------+  |
 | | V8                                 +-----------+    |  |
 | |                                    | request() |-+  |  |
 | | fs.read();   lkl.fs.read();        +------^----+ |  |  |
 | +---|---------------|-----------------------|------|--+  |
 |     |               |                       |      |     |
 |     |       virtual | syscall interface     |      |     |
 |     |      +--------V---------+             |    async   |
 |     |      |     liblkl.so    |             |      |     |
 |     |      | +-------------+  |             |      |     |
 |     |      | |  fs driver  |  |           async    |     |
 |     |      | +-------+-----+  |             |      |     |
 |     |      |         |        |             |      |     |
 |     |      | +-------V------+ |             |      |     |
 |     |      | | block driver | |             |      |     |
 |     |      | +-------+------+ |             |      |     |
 |     |      +---------|--------+             |      |     |
 |     |  virtual block | request interface    |      |     |
 |     |        +-------V------+               |      |     |
 |     |        |  V8 adapter  |---------------+      |     |
 |     |        +--------------+                      |     |
 |     |                                              |     |
 |     |--------------------------------------------- +     |
 |     |                                                    |
 +-----|----------------------------------------------------+
       | syscall interface
 +-----V------------+
 |      kernel      |
 | +-------------+  |
 | |  fs driver  |  |
 | +-------+-----+  |
 |         |        |
 | +-------V------+ |
 | | block driver | |
 | +-------+------+ |
 +---------|--------+
           | block request interface
    +------V-----+
    | hard drive |
    +------------+
```

Support
-------

If you're having any problems, please [raise an issue][github-issue] on GitHub.

License
-------

node-lkl is free software, and may be redistributed under the terms specified
in the [license].

[github-issue]: https://github.com/resin-io/node-lkl/issues/new
[license]: https://github.com/resin-io/node-lkl/blob/master/LICENSE

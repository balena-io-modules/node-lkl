node-lkl
=========
### NodeJS native bindings to the Linux Kernel Library project
[![Build Status](https://travis-ci.org/petrosagg/node-lkl.svg?branch=master)](https://travis-ci.org/petrosagg/node-lkl)

node-lkl uses the [Linux Kernel Library project](https://github.com/lkl/linux) to provide access to filesystem drivers
from NodeJS in a cross-platform way.

Some things you can do with this module:

* Read/write files in a filesystem image directly without mounting
* Use familiar APIs, node-lkl has the exact same interface as node's `fs` module
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

``` javascript
const lkl = require('lkl');

// Run the kernel with 10MB of memory
lkl.startKernelSync(10 * 1024 * 1024);

const disk = new lkl.disk.FileDisk('data.ext4');

const mountpoint = lkl.mount(disk, {filesystem: 'ext4'}, function(err, mountpoint) {
	if (err) {
		console.error(err);
		return;
	}

	const file = lkl.fs.createReadStream(mountpoint + '/etc/passwd');

	file.pipe(process.stdout);
});
```

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

[github-issue]: https://github.com/petrosagg/node-lkl/issues/new
[license]: https://github.com/petrosagg/node-lkl/blob/master/LICENSE

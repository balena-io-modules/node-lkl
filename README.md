node-lkl
=========
### NodeJS native bindings to the Linux Kernel Library project
[![Build Status](https://travis-ci.org/petrosagg/node-lkl.svg?branch=master)](https://travis-ci.org/petrosagg/node-lkl)

node-lkl uses the Linux Kernel Library project to provide access to filesystem drivers
from NodeJS in a cross-platform way.

Some things you can do with this module:

* Read/write files in a filesystem image directy without mounting
* Use familiar APIs, node-lkl has the exact same interface as node's `fs` module
* Combine lkl filesystem streams with host filesystem streams (e.g copy files)
* Create a tar archive from a filesystem image

Check the [How it works] section to learn how it works.

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
var lkl = require('lkl');

// get a copy of the fs module for that image
let fs = lkl.mount('data-ext4.img', false);

// create a readable stream
let input = fs.createReadStream('/etc/passwd');

// print the file to stdout
input.pipe(process.stdout);
```

How it works
------------

node-lkl is based on the LKL (Linux Kernel Library) project. This project is
focused on the filesystem capabilities although future work could allow usage
of the kernel networking stack as well. To understand how a userspace process
can "mount" an image without actually mounting it, it helps to think about the
interface between an actual kernel and a block device.

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

Everything inside the box named `kernel` box doesn't actually need the hardware
to do its job. When an application instructs the kernel to read a file, the fs
driver issues some read requests to the block driver and the block driver uses
issues those to the hard drive. All of the high level filesystem semantics like
files, directories, permissions etc are "compiled" down to raw reads and writes
at specific offsets.

This means that as long as we can provide a block request interface to the
kernel, and find a way to run the kernel in userspace we can use all of its
functionality. This is how it looks like with node-lkl:

```
 +----------------------------------------------------+
 |  nodejs application                                |
 |                                                    |
 | +-------------------------------+                  |
 | | V8                            |                  |
 | |                               |                  |
 | | fs.read();     lkl_fs.read(); |                  |
 | +------|---------------|--------+                  |
 |        |               |                           |
 |        |       virtual | syscall interface         |
 |        |      +--------V---------+                 |
 |        |      |     liblkl.so    |                 |
 |        |      | +-------------+  |                 |
 |        |      | |  fs driver  |  |                 |
 |        |      | +-------+-----+  |                 |
 |        |      |         |        |                 |
 |        |      | +-------V------+ |                 |
 |        |      | | block driver | |                 |
 |        |      | +-------+------+ |                 |
 |        |      +---------|--------+                 |
 |        |        virtual | block request interface  |
 |        |    +-----------V----------+               |
 |        |    | open file descriptor |               |
 |        |    +-----------V----------+               |
 |        |                |                          |
 |        |----------------+                          |                |
 |        |                                           |
 +--------|-------------------------------------------+
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

Support
-------

If you're having any problem, please [raise an issue][github-issue] on GitHub.

License
-------

node-lkl is free software, and may be redistributed under the terms specified
in the [license].

[github-issue]: https://github.com/petrosagg/node-lkl/issues/new
[license]: https://github.com/petrosagg/node-lkl/blob/master/LICENSE

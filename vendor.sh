#!/bin/bash

set -o errexit

git clone https://github.com/lkl/linux.git --depth=1 --branch master linux-tmp
 
# Reset all access times to be in the past
find linux-tmp | xargs touch -a -d 2015-01-01
 
# Make the library. This will update the access times of only the actually needed files
make -j $(nproc) -C linux-tmp/tools/lkl static
make -j $(nproc) -C linux-tmp/tools/lkl clean
make -j $(nproc) CROSS_COMPILE=i686-w64-mingw32- -C linux-tmp/tools/lkl static

# Delete anything that wasn't accessed
find linux-tmp ! -path "*/.git/*" -atime +2 ! -type d ! -name .gitignore -delete
# Delete a second time to delete empty directories
find linux-tmp ! -path "*/.git/*" -atime +2 -type d -empty -delete

# Cleanup all built objects
git -C linux-tmp clean -ffxd

# Verify that the pruned tree builds
make -j $(nproc) -C linux-tmp/tools/lkl static
make -j $(nproc) -C linux-tmp/tools/lkl clean
make -j $(nproc) CROSS_COMPILE=i686-w64-mingw32- -C linux-tmp/tools/lkl static

# Cleanup all built objects again
git -C linux-tmp clean -ffxd

# Remove history
rm -rf linux-tmp/.git

# Vendor the code
rm -rf src/linux
mv linux-tmp src/linux

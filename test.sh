#!/bin/sh
tmp=$(mktemp -d)
mkdir $tmp/foo $tmp/bar
dd if=/dev/zero of=$tmp/foo/file1 count=5
./vdu $tmp
rm -rf $tmp

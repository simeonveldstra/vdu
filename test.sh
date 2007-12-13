#!/bin/sh
tmp=$(mktemp -d)
mkdir $tmp/foo $tmp/bar 
mkdir $tmp/bar/baz $tmp/bar/quux
dd if=/dev/zero of=$tmp/foo/file1 count=5
./vdu $tmp
rm -rf $tmp

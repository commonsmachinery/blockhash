#!/bin/sh

set -e

d=`dirname "$0"`

cd "$d/testdata"
echo
../build/blockhash -b 16 *.jpg | diff -s exact-hashes.txt -
echo
../build/blockhash -q -b 16 *.jpg | diff -s quick-hashes.txt -
echo
echo OK
echo

#!/bin/sh

set -e

d=`dirname "$0"`

cd "$d/testdata"

echo
../blockhash -b 16 *.jpg *.png | sort | diff -su exact-hashes.txt -

echo
../blockhash -q -b 16 *.jpg *.png | sort | diff -su quick-hashes.txt -

echo
echo Test finished.
echo


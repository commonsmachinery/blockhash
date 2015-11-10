#!/bin/sh

set -e

d=`dirname "$0"`

cd "$d/testdata"

echo
../build/blockhash -b 16 *.jpg *.png | sort | diff -su exact-hashes.txt -

echo
../build/blockhash -q -b 16 *.jpg *.png | sort | diff -su quick-hashes.txt -

echo
echo Test finished.
echo


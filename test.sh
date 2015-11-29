#!/bin/sh

set -e

d=`dirname "$0"`

cd "$d/testdata"

hash_tool=../src/blockhash

echo
echo Testing exact image hashes...
echo
${hash_tool}  -b 16 *.jpg *.png | sort | diff -su image-exact-hashes.txt -

echo
echo Testing quick image hashes...
echo
${hash_tool} -q -b 16 *.jpg *.png | sort | diff -su image-quick-hashes.txt -

echo
echo Testing exact video hashes...
echo
${hash_tool} -b 16 -V *.mp4 | sort | diff -su video-exact-hashes.txt -

echo
echo Testing quick video hashes...
echo
${hash_tool} -q -b 16 -V *.mp4 | sort | diff -su video-quick-hashes.txt -


echo
echo Test finished.
echo


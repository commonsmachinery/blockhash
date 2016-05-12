#!/bin/sh

set -e

d=`dirname "$0"`

cd "$d/testdata"

hash_tool=../src/phashtool

echo
echo Testing image hashes...
echo
${hash_tool} *.jpg *.png | sort | diff -su image-hashes.txt -

echo
echo Testing video hashes...
echo
${hash_tool} -V *.mp4 | sort | diff -su video-hashes.txt -


echo
echo Test finished.
echo


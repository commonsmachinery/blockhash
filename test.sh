#!/bin/sh

set -e

d=`dirname "$0"`

(cd "$d/testdata/images"

../../build/blockhash -b 16 *.jpg *.png | sort | diff -su exact-hashes.txt -
../../build/blockhash -q -b 16 *.jpg *.png | sort | diff -su quick-hashes.txt -
)

(cd "$d/testdata/videos"
../../build/blockhash_video -b 16 *.mpeg *.ogv | sort | diff -su exact-hashes.txt -
../../build/blockhash_video -q -b 16 *.mpeg *.ogv | sort | diff -su quick-hashes.txt -
)
echo
echo Test finished.
echo


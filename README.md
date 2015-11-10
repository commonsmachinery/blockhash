blockhash
=========

This is a perceptual image and video hash calculation tool based on algorithm descibed in
Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu.

Build and install
-----------------

Blockhash requires libmagickwand and libopencv. On Debian/Ubuntu it can be installed using
the following command:

    sudo apt-get install libmagickwand-dev libopencv-dev

On Fedora and friends:

    sudo dnf install ImageMagick-devel opencv-devel

To build blockhash cd to the source directory and type:

    ./waf configure
    ./waf

The program binary will land in `./build`. To install it to `/usr/local/bin/` type:

    ./waf install

Usage
-----

Run `blockhash [list of images]` for calculating hashes for image files.

Run `blockhash_video [list of videos]` for calculating hashes for video files.

Run `blockhash --help` for the list of options.

WARNING
-------

Please note that the status of the video algorithm is EXPERIMENTAL. It is
subject to substantial changes and should not be relied upon.

License
-------

Copyright (c) 2014-2015 Commons Machinery http://commonsmachinery.se/

Distributed under an MIT license, please see LICENSE in the top dir.

Contact: dev@commonsmachinery.se

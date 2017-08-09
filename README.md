blockhash
=========

This is a perceptual image hash calculation tool based on algorithm descibed in
Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu.

Build and install
-----------------

Blockhash requires libmagickwand. On Debian/Ubuntu it can be installed using
the following command:

    sudo apt-get install libmagickwand-dev

On Fedora and friends:

    sudo dnf install ImageMagick-devel

To build blockhash cd to the source directory and type:

    mkdir build
		cd build
		cmake ..
		cmake --build .

The program binary will land in the build directory. To install, copy it to `/usr/local/bin`

    cp blockhash /usr/local/bin

Usage
-----

Run `blockhash [list of images]` for calculating hashes.

Run `blockhash --help` for the list of options.

License
-------

Copyright 2014 Commons Machinery http://commonsmachinery.se/

Distributed under an MIT license, please see LICENSE in the top dir.

Contact: dev@commonsmachinery.se

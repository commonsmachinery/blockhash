blockhash
=========

This is a perceptual image and video hash calculation tool based on algorithm descibed in
Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu.

Build and install
-----------------

Blockhash requires:
- libmagickwand
- ffmpeg libraries (libavformat, libavcodec, libavutil, libswscale)

On Debian/Ubuntu it can be installed using the following command:

    sudo apt-get install libmagickcore-dev libmagickwand-dev libavformat-dev libavutil-dev libavcodec-dev libswscale-dev

On Fedora and friends:

First, you need to enable RPMForge repository (read more, for example, [here](http://www.tecmint.com/enable-rpmforge-repository/)). 
Then you'll be able to install as follows:

    sudo dnf install ImageMagick-devel ffmpeg-devel
    
or possibly

    sudo yum install ImageMagick-devel ffmpeg-devel


To build blockhash cd to the source directory and type:

    ./configure
    make rebuild

The program binary will land in `./src`. To install it to `/usr/local/bin/` type:

    make install

Usage
-----

Run `blockhash [list of images]` for calculating hashes for image files.

Run `blockhash --video [list of videos]` for calculating hashes for video files.

Run `blockhash --help` for the list of options.

License
-------

Copyright (c) 2014-2015 Commons Machinery http://commonsmachinery.se/

Distributed under an MIT license, please see LICENSE in the top dir.

Contact: dev@commonsmachinery.se


phashtool
=========

This is a perceptual image and video hash calculation tool based on the phash algorithm.


Build and install
-----------------

phashtool requires:
- libjpeg
- libpng
- libtiff
- libopenexr
- zlib
- ffmpeg libraries (libavformat, libavcodec, libavutil, libswscale)

On Debian/Ubuntu it can be installed using the following command:

    sudo apt-get install libjpeg-dev libpng-dev libtiff-dev libopenexr-dev libz-dev libavformat-dev libavutil-dev libavcodec-dev libswscale-dev

On Fedora and friends:

First, you need to enable RPMForge repository (read more, for example, [here](http://www.tecmint.com/enable-rpmforge-repository/)). 
Then you'll be able to install as follows:

    sudo dnf install libjpeg-devel libpng-devel libtiff-devel libopenexr-devel zlib-devel ffmpeg-devel 
    
or possibly

    sudo yum install libjpeg-devel libpng-devel libtiff-devel OpenEXR-devel zlib-devel ffmpeg-devel 


To build phashtool cd to the source directory and type:
    
    ./configure
    make

The program binary will land in `./src`. To install it to `/usr/local/bin/` type:

    make install

Usage
-----

Run `phashtool [list of images]` for calculating hashes for image files.

Run `phashtool --video [list of videos]` for calculating hashes for video files.

Run `phashtool --help` for the list of options.

License
-------

Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/

Distributed under GPLv3 license, please see bundled file COPYING for details.

Contact: dev@commonsmachinery.se


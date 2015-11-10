#
# Perceptual image hash calculation library based on algorithm descibed in
# Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
#
# Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
# Distributed under an MIT license, please see LICENSE in the top dir.
#

CC=gcc
LD=gcc

ifeq ("$(DEBUG)","1")
CFLAGS:=-g3 -ggdb
else
CFLAGS:=-O3
endif

CFLAGS+=-Wall -Werror -fno-common

CFLAGS+=$(shell MagickWand-config --cflags)
LDFLAGS:=-L. $(shell MagickWand-config --ldflags)

LIBS:= 	-lblockhash \
	$(shell MagickWand-config --libs) \
	-lopencv_core \
	-lopencv_imgproc \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_video \
	-lopencv_features2d \
	-lopencv_calib3d \
	-lopencv_objdetect \
	-lopencv_contrib \
	-lopencv_legacy \
	-lopencv_stitching \
	-lm \

ARFLAGS:=

.PHONY: all build clean rebuild install

all: build

build: blockhash

clean:
	-rm -f *.o
	-rm -f libblockhash.a
	-rm -f blockhash

rebuild: clean build
	
install: blockhash
	install -c -t /usr/local/bin blockhash
	
blockhash: main.o libblockhash.a
	$(LD) -o $@ $(LDFLAGS)  $^ $(LIBS)
	
libblockhash.a : blockhash.o
	$(AR) rvs $(ARFLAGS) $@ $^
	
%.o: %.c
	$(CC) -o $@ $(CFLAGS)-c $<


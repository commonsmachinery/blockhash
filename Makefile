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

CFLAGS+=-Wall -Werror -fno-common -fmax-errors=2 -DHAVE_MACHINE_ENDIAN_H

CFLAGS+=$(shell MagickWand-config --cflags)
LDFLAGS:=-L. $(shell MagickWand-config --ldflags)

LIBS:= 	-lblockhash \
	$(shell MagickWand-config --libs) \
	-lavformat \
	-lavcodec \
	-lavutil \
	-lswscale \
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
	
blockhash: libblockhash.a main.o misc.o process_image.o process_video.o
	$(LD) -o $@ $(LDFLAGS)  $^ $(LIBS)
	
libblockhash.a : blockhash.o
	$(AR) rvs $(ARFLAGS) $@ $^
	
%.o: %.c
	$(CC) -o $@ $(CFLAGS)-c $<


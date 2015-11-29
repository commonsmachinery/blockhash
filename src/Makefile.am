lib_LTLIBRARIES = libblockhash.la
libblockhash_la_SOURCES = blockhash.c
libblockhash_la_LDFLAGS = -no-undefined
include_HEADERS = blockhash.h

bin_PROGRAMS = blockhash
blockhash_SOURCES = main.c misc.c process_image.c process_video.c processing.h
blockhash_LDADD = libblockhash.la

/*
 * Perceptual image hash calculation tool based on pHash algorithm
 *
 * Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an GPLv3 license, please see file COPYING in the top dir.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <inttypes.h>
#include "processing.h"

void print_digest(const char* filename, const uint64_t* digest, size_t digest_size) {
    const uint64_t* digest_end = digest + digest_size;
    while(digest < digest_end)
        printf("%016"PRIx64, *digest++);
    printf("  %s\n", filename);
}

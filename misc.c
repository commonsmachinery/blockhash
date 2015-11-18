/*
 * Perceptual image hash calculation tool based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#include <stdio.h>
#include "processing.h"

void debug_print_hash(const int* hash, int bits) 
{
    int i;
    for (i = 0; i < bits * bits; i++) {
        if (i != 0 && i % bits == 0)
            printf("\n");
        printf("%d", hash[i]);
    }
    printf("\n");
}

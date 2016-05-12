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
#include "processing.h"
#include "pHash-img-c.h"

int process_image_file(const hash_computation_task* task) {
    uint64_t hash = 0;
    int res = cph_dct_imagehash(task->src_file_name, &hash);
    
    if(res)
        fprintf(stderr, "Error computhing hash for image file %s.\n", task->src_file_name);
    else
        print_digest(task->src_file_name, &hash, 1);

    return res;
}

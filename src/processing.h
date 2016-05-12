/*
 * Perceptual image hash calculation tool based on pHash algorithm
 *
 * Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an GPLv3 license, please see file COPYING in the top dir.
 */

#ifndef PHASHTOOL_PROCESSING_H__
#define PHASHTOOL_PROCESSING_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
#endif

struct _hash_computation_task {
    const char* src_file_name;
    int digest_type;
    int debug;
    int video;
};

typedef struct _hash_computation_task hash_computation_task;

int process_image_file(const hash_computation_task* task);

int process_video_file(const hash_computation_task* task);

void print_digest(const char* filename, const uint64_t* digest, size_t digest_size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

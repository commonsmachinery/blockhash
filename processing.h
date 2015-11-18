/*
 * Perceptual image hash calculation tool based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#ifndef __BLOCKHASH_PROCESSING_H__
#define __BLOCKHASH_PROCESSING_H__

#define DEFAULT_BITS 16

struct _hash_computation_task {
    const char* src_file_name;
    int bits;
    int quick;
    int debug;
    int video;
};

typedef struct _hash_computation_task hash_computation_task;

void debug_print_hash(const int* hash, int bits);

int process_image_file(const hash_computation_task* task);

int process_video_file(const hash_computation_task* task);

struct _MagickWand;
typedef struct _MagickWand MagickWand;

MagickWand* new_magic_wand();

MagickWand* load_image_from_file(const char* src_file_name);

int compute_image_hash(MagickWand* magick_wand, int bits, int quick, int** hash);


#endif

/*
 * Perceptual image hash calculation tool based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <wand/MagickWand.h>
#include "blockhash.h"
#include "processing.h"

int process_image_file(const hash_computation_task* task)
{
    int result = 0;
    int *hash;
    MagickWand *magick_wand;

    // Load Image
    magick_wand = load_image_from_file(task->src_file_name);
    if(!magick_wand) return -1;
    
    // Compute Image Hash
    result = compute_image_hash(magick_wand, task->bits, task->quick, &hash);
    
    switch(result)
    {
        case 0: {
            // Show debug output
            if (task->debug) {
                printf("Dump of the image hash:\n");
                debug_print_hash(hash, task->bits);
            }
            
            // Print blockhash string
            char* hex = blockhash_to_str(hash, task->bits * task->bits);
            if(hex) {
                printf("%s  %s\n", hex, task->src_file_name);
                free(hex);
            } else {
                result = -1;
                fprintf(stderr, "Error: Failed to convert blockhash value to string for the image file '%s'.\n", 
                        task->src_file_name);
            }
            
            // Free hash buffer
            free(hash);
            break;
        }
        
        case 1: {
            fprintf(stderr, "Error: Failed to compute blockhash for the zero-sized image file '%s'.\n", 
                    task->src_file_name);
            break;
        }
        
        case 2: {
            fprintf(stderr, "Error: Failed to convert image data to RGBA for the image file '%s'.\n", 
                    task->src_file_name);
            break;
        }
        default: {
            fprintf(stderr, "Error: Failed to compute blockhash for the image file '%s'.\n", 
                    task->src_file_name);
            break;
        }
    }

    // Cleanup temporary data
    DestroyMagickWand(magick_wand);
    
    // Report the result
    return result;
}


int compute_image_hash(MagickWand* magick_wand, int bits, int quick, int** hash) 
{
    MagickBooleanType status;
    size_t width, height, data_size;
    unsigned char *image_data;
    
    // Remove color profiles for interoperability with other hashing tools
    MagickProfileImage(magick_wand, "*", NULL, 0);
    
    // Compute pixel data size
    width = MagickGetImageWidth(magick_wand);
    height = MagickGetImageHeight(magick_wand);
    data_size = width * height * 4;
    
    // Handle special zero size case
    if(data_size == 0) {      
        size_t hash_size = bits * bits * sizeof(int); 
        int* h = (int*) malloc(hash_size);
        if(!h) return 1;
        memset(h, 0, hash_size);
        *hash = h;
        return 0;
    }
    
    // Export pixel data
    image_data = (unsigned char*) malloc(data_size);
    if(!image_data) return 2;
    
    status = MagickExportImagePixels(magick_wand, 0, 0, width, height, "RGBA", CharPixel, image_data);
    if (status == MagickFalse) return 3;
    
    // Compute blockhash
    return (quick)
        ? blockhash_quick(bits, image_data, width, height, hash)
        : blockhash(bits, image_data, width, height, hash);    
}


MagickWand* new_magic_wand() 
{
    // Create MagickWand object
    MagickWand* magick_wand = NewMagickWand();
    if(!magick_wand)
        fprintf(stderr, "Error: Couldn't create MagicWand object.\n");
    return magick_wand;
}


MagickWand* load_image_from_file(const char* src_file_name) 
{
    // Create MagickWand object
    MagickWand* magick_wand = new_magic_wand();
    if(magick_wand) {
        // Read an image file into memory
        MagickBooleanType status = MagickReadImage(magick_wand, src_file_name);
        if (status == MagickFalse) {
            fprintf(stderr, "Error: Couldn't read image file '%s'.\n", src_file_name);
            DestroyMagickWand(magick_wand);
            magick_wand = NULL;
        }
    }
    return magick_wand;    
}

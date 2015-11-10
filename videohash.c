/*
 * Perceptual image hash calculation tool based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <wand/MagickWand.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "blockhash.h"

MagickWand* load_image_from_frame(char* filename, size_t frame_number, CvMat* frame_data) 
{
    // Create MagickWand object
    MagickWand* magick_wand = NewMagickWand();

    if(magick_wand) {
        // Read an image file into memory
        MagickBooleanType status = MagickReadImageBlob(magick_wand, frame_data->data.ptr, frame_data->cols);
        if (status == MagickFalse) {
            fprintf(stderr, "Error reading converted to image frame #%llu of video file '%s'.\n", 
        	    (unsigned long long)frame_number,
        	    filename);
            DestroyMagickWand(magick_wand);
            magick_wand = NULL;
        }
    }
    return magick_wand;    
}

static int compute_image_hash(MagickWand* magick_wand, int bits, int quick, int** hash) 
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
	int* h = malloc(hash_size);
	if(!h) return 1;
	memset(h, 0, hash_size);
	*hash = h;
	return 0;
    }
    
    // Export pixel data
    image_data = malloc(data_size);
    if(!image_data) return 2;
    
    status = MagickExportImagePixels(magick_wand, 0, 0, width, height, "RGBA", CharPixel, image_data);
    if (status == MagickFalse) return 3;
    
    // Compute blockhash
    return (quick)
	? blockhash_quick(bits, image_data, width, height, hash)
	: blockhash(bits, image_data, width, height, hash);    
}


int *process_video_frame(char *filename, int bits, int quick, size_t frame_number, CvMat* frame_data)
{
    int result = 0;
    int *hash;
    MagickWand *magick_wand;
    
    // Load Image
    magick_wand = load_image_from_frame(filename, frame_number, frame_data);
    if(!magick_wand) return NULL;
    
    // Compute Image Hash
    result = compute_image_hash(magick_wand, bits, quick, &hash);
    
    switch(result)
    {
        case 0: break;
        
        case 1: {
            fprintf(stderr, "Error computing blockhash for the zero-sized frame #%llu of the video file '%s'.", 
        	    (unsigned long long)frame_number, filename);
            break;
        }
        
        case 2: {
            fprintf(stderr, "Error converting image data to RGBA for the frame #%llu of the video file '%s'.\n", 
        	    (unsigned long long)frame_number, filename);
            break;
        }
        default: {
            fprintf(stderr, "Error computing blockhash for the frame #%llu of the video file '%s'.", 
        	    (unsigned long long)frame_number, filename);
            break;
        }
    }
    
    // Cleanup temporary data
    DestroyMagickWand(magick_wand);
    
    // Report the result
    return hash;
}

typedef struct _video_frame_info {
    size_t frame_number;
    int* hash;
} video_frame_info;

#define HASH_PART_COUNT 4 

int process_video(char *filename, int bits, int quick, int debug)
{
    size_t i;
    int result = 0;
    CvCapture* capture;
    size_t frame_count;
    video_frame_info hash_frames[HASH_PART_COUNT];
    size_t next_hash_frame;
    size_t current_frame;
    int* hash = NULL;
    
    // Initialize data
    memset(&hash_frames[0], 0, sizeof(hash_frames));
    
    // Open video file
    capture = cvCreateFileCapture(filename);
    if(!capture) {
        fprintf(stderr, "Error opening video file '%s'.", filename);
        return -1;
    }
    
    // Get frame count
    frame_count = (size_t)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
    // Determine first and last hash frames
    if(frame_count < 11) {
        if(frame_count > 0) {
            hash_frames[0].frame_number = 0;
            hash_frames[3].frame_number = frame_count - 1;	  
        } else {
            // Handle special zero frames case
            result = -1;
            fprintf(stderr, "Error computing blockhash for zero-frame video file");
            goto cleanup;
        }
    } else {
        hash_frames[0].frame_number = 10;
        hash_frames[3].frame_number = frame_count - 11;
    }
    
    // Determine middle hash frames
    hash_frames[1].frame_number = (size_t)floor(frame_count * 0.35);
    hash_frames[2].frame_number = (size_t)floor(frame_count * 0.7);
    
    // Read frames one by one and compute hash for the selected frames
    next_hash_frame = 0;
    for(current_frame = 0; current_frame < frame_count; ++current_frame) {
        IplImage* frame_image = cvQueryFrame(capture);
        if(!frame_image) {
            result = -1;
            fprintf(stderr, "Error capturing frame #%llu of %llu from the video file '%s'.", 
        	    (unsigned long long)current_frame,
        	    (unsigned long long)frame_count,
        	    filename);
            goto cleanup;
        } else {
            CvMat* mat = NULL;
            for(i = next_hash_frame; i < HASH_PART_COUNT; ++i) {
        	if(hash_frames[i].frame_number == current_frame) {
        	    if(!mat) {
        		mat = cvEncodeImage(".bmp", frame_image, NULL);
        		if(!mat) {
        		    result = -1;
        		    fprintf(stderr, "Error converting to image frame #%llu of the video file '%s'.", 
        			    (unsigned long long)current_frame,
        			    filename);
        		    goto cleanup;
        		}
        	    }
        	    hash_frames[i].hash = process_video_frame(filename, bits, quick, current_frame, mat);
        	    ++next_hash_frame;
        	    // Do not break here, becasue in certain cases 
        	    // a single frame may happen multiple times as hash_frame
        	}
            }
        }
    }
    
    hash = malloc(bits * bits * sizeof(int) * HASH_PART_COUNT);
    if(!hash) {
        fprintf(stderr, "Error creating hash for video file '%s'.\n", filename);
        goto cleanup;
    } else {
        size_t block_element_count = bits * bits;
        size_t block_size = block_element_count * sizeof(int);
        int* dest = hash;
        for(i = 0; i < HASH_PART_COUNT; ++i, dest += block_element_count)
            memcpy(dest, hash_frames[i].hash, block_size); 
    }
    
    char* hex = bits_to_hexhash(hash, HASH_PART_COUNT * bits * bits);
    if(hex) {
      result = 0;
      printf("%s  %s\n", hex, filename);
      free(hex);
    } else {
      result = -1;
      fprintf(stderr, "Error converting blockhash value to string for the image file '%s'.\n", 
              filename);
    }

    
    // Free hash buffer
    free(hash);
    
cleanup:
    // Free partial hash buffers
    for(i = 0; i < HASH_PART_COUNT; ++i) {
        int* h = hash_frames[i].hash; 
        if(h) free(h);
    }
    
    // Close video file
    cvReleaseCapture(&capture);
    
    // report the result
    return result;
}

void help() {    
    printf("Usage: blockhash_video [-h|--help] [--quick] [--video] [--bits BITS] [--debug] filenames...\n"
           "\n"
           "Optional arguments:\n"
           "-h, --help            Show this help message and exit\n"
           "-q, --quick           Use quick hashing method.\n"
           "-b, --bits BITS       Specify hash size (N^2) bits. Default: 16\n"
           "--debug               Print hashes as 2D maps (for debugging)\n");
}


void main (int argc, char **argv) 
{
    MagickWandGenesis();

    int quick = 0;
    int debug = 0;
    int bits = 16;
    int x;

    int option_index = 0;
    int c;

    struct option long_options[] = {
        {"help",    no_argument,        0, 'h'},
        {"quick",   no_argument,        0, 'q'},
        {"bits",    required_argument,  0, 'b'},
        {"debug",   no_argument,        0, 'd'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        help();
        exit(0);
    }


    while ((c = getopt_long(argc, argv, "hqb:d",
                 long_options, &option_index)) != -1) {
        switch (c) {
        case 'h':
            help();
            exit(0);
            break;
        case 'q':
            quick = 1;
            break;
        case 'b':
            if (sscanf(optarg, "%d", &bits) != 1) {;
                printf("Error: couldn't parse bits argument\n");
                exit(-1);
            }
            if (bits % 4 != 0) {
                printf("Error: bits argument should be a multiple of 4\n");
                exit(-1);
            }
            break;
        case 'd':
            debug = 1;
            break;
        case '?':
        default:
            exit(-1);
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            process_video(argv[optind++], bits/2, quick, debug);
        }
    }

    MagickWandTerminus();
    
    exit(0);
}

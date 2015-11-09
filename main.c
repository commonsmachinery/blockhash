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
#include "parameters.h"

typedef struct _hash_computing_task {
	const char* file_name;
	int bits;
	int quick;
	int debug;
} hash_computing_task;


static void debug_print_hash(const int* hash, int bits) 
{
    int i;
    for (i = 0; i < bits * bits; i++) {
	if (i != 0 && i % bits == 0)
	    printf("\n");
	printf("%d", hash[i]);
    }
    printf("\n");
}


static MagickWand* new_magic_wand() 
{
    // Create MagickWand object
    MagickWand* magick_wand = NewMagickWand();
    if(!magick_wand)
	fprintf(stderr, "Error creating MagicWand object.\n");
    return magick_wand;
}


static MagickWand* load_image_from_file(const char* file_name) 
{
    // Create MagickWand object
    MagickWand* magick_wand = new_magic_wand();
    if(magick_wand) {
	// Read an image file into memory
	MagickBooleanType status = MagickReadImage(magick_wand, file_name);
	if (status == MagickFalse) {
	    fprintf(stderr, "Error reading image file '%s'.\n", file_name);
	    DestroyMagickWand(magick_wand);
	    magick_wand = NULL;
	}
    }
    return magick_wand;    
}


static MagickWand* load_image_from_frame(const char* file_name, size_t frame_number, CvMat* frame_data) 
{
    // Create MagickWand object
    MagickWand* magick_wand = new_magic_wand();
    if(magick_wand) {
	// Read an image file into memory
	MagickBooleanType status = MagickReadImageBlob(magick_wand, frame_data->data.ptr, frame_data->cols);
	if (status == MagickFalse) {
	    fprintf(stderr, "Error reading converted to image frame #%llu of video file '%s'.\n", 
		    (unsigned long long)frame_number,
		    file_name);
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


static int* process_video_frame(const hash_computing_task* task, size_t frame_number, CvMat* frame_data)
{
    int result = 0;
    int *hash;
    MagickWand *magick_wand;
    
    // Load Image
    magick_wand = load_image_from_frame(task->file_name, frame_number, frame_data);
    if(!magick_wand) return NULL;
    
    // Compute Image Hash
    result = compute_image_hash(magick_wand, task->bits, task->quick, &hash);
    
    switch(result)
    {
	case 0: {
	    // Show debug output
	    if (task->debug) {
		printf("Dump of the frame#%llu hash:\n", (unsigned long long)frame_number);
		debug_print_hash(hash, task->bits);
	    }
	    break;
	}
	
	case 1: {
	    fprintf(stderr, "Error computing blockhash for the zero-sized frame #%llu of the video file '%s'.", 
		    (unsigned long long)frame_number, task->file_name);
	    break;
	}
	
	case 2: {
	    fprintf(stderr, "Error converting image data to RGBA for the frame #%llu of the video file '%s'.\n", 
		    (unsigned long long)frame_number, task->file_name);
	    break;
	}
	default: {
	    fprintf(stderr, "Error computing blockhash for the frame #%llu of the video file '%s'.", 
		    (unsigned long long)frame_number, task->file_name);
	    break;
	}
    }
    
    // Cleanup temporary data
    DestroyMagickWand(magick_wand);
    
    // Report the result
    return hash;
}


static int process_image_file(const hash_computing_task* task)
{
    int result = 0;
    int *hash;
    MagickWand *magick_wand;

    // Load Image
    magick_wand = load_image_from_file(task->file_name);
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
	    char* hex = blockhash_bits_to_hex_str(hash, task->bits * task->bits);
	    if(hex) {
		printf("%s  %s\n", hex, task->file_name);
		free(hex);
	    } else {
		result = -1;
		fprintf(stderr, "Error converting blockhash value to string for the image file '%s'.\n", 
			task->file_name);
	    }
	    
	    free(hash);
	    break;
	}
	
	case 1: {
	    fprintf(stderr, "Error computing blockhash for the zero-sized image file '%s'.", 
		    task->file_name);
	    break;
	}
	
	case 2: {
	    fprintf(stderr, "Error converting image data to RGBA for the image file '%s'.\n", 
		    task->file_name);
	    break;
	}
	default: {
	    fprintf(stderr, "Error computing blockhash for the image file '%s'.", 
		    task->file_name);
	    break;
	}
    }

    // Cleanup temporary data
    DestroyMagickWand(magick_wand);
    
    // Report the result
    return result;
}

typedef struct _video_frame_info {
    size_t frame_number;
    int* hash;
} video_frame_info;

static int process_video_file(const hash_computing_task* task)
{
    int result = 0;
    CvCapture* capture;
    size_t frame_count;
    video_frame_info hash_frames[4];
    size_t next_hash_frame;
    size_t current_frame;
    int* hash = NULL;
    
    // Initialize data
    memset(&hash_frames[0], 0, sizeof(hash_frames));
    
    // Open video file
    capture = cvCreateFileCapture(task->file_name);
    if(!capture) {
        fprintf(stderr, "Error opening video file '%s'.", task->file_name);
	return -1;
    }
    
    // Get frame count
    frame_count = (size_t)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
    if(task->debug)
	printf("Video file '%s' has %llu frames.\n", 
	       task->file_name, (unsigned long long)frame_count);
    
    // Determine first and last hash frames
    if(frame_count < 11) {
	if(frame_count > 0) {
	    hash_frames[0].frame_number = 0;
	    hash_frames[3].frame_number = frame_count - 1;	  
	} else {
	    // Handle special zero frames case
	    size_t hash_size = task->bits * task->bits * sizeof(int); 
	    hash = malloc(hash_size);
	    if(!hash) {
		result = -1;
		fprintf(stderr, "Error computing blockhash for the zero-frame video file '%s'.", 
			task->file_name);
		goto cleanup_release_capture;
	    }
	    memset(hash, 0, hash_size);
	    goto hash_computed;
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
		    task->file_name);
	    goto cleanup_release_capture;
	} else {
	    CvMat* mat = NULL;
	    size_t i;
	    for(i = next_hash_frame; i < 4; ++i) {
		if(hash_frames[i].frame_number == current_frame) {
		    if(!mat) {
			mat = cvEncodeImage(".bmp", frame_image, NULL);
			if(!mat) {
			    result = -1;
			    fprintf(stderr, "Error converting to image frame #%llu of the video file '%s'.", 
				    (unsigned long long)current_frame,
				    task->file_name);
			    goto cleanup_release_capture;
			}
		    }
		    hash_frames[i].hash = process_video_frame(task, current_frame, mat);
		    ++next_hash_frame;
		    // Do not break here, becasue in certain cases 
		    // a single frame may happen multipl times as hash_frame
		}
	    }
	}
    }
    
hash_computed:
    // Show debug output
    if (task->debug)
	debug_print_hash(hash, task->bits);

    // Print blockhash string
    char* hex = blockhash_bits_to_hex_str(hash, task->bits * task->bits);
    if(hex) {
      result = 0;
      printf("%s  %s\n", hex, task->file_name);
      free(hex);
    } else {
      result = -1;
      fprintf(stderr, "Error converting blockhash value to string for the image file '%s'.\n", 
	      task->file_name);
    }

    
    // Clenup temporary data 
    free(hash);
    
    // close video file
cleanup_release_capture:
    cvReleaseCapture(&capture);
    
    // report the result
    return result;
}


static inline int hash_size_in_true_bits(int bits, int scale) {
    return bits*bits*scale;
}


static void show_help(char* program_name) 
{    
    printf("Usage: %s [-h|--help] [-v|--version] [--quick] [--video] [--bits BITS] [--debug] filenames...\n"
           "\n"
           "Optional arguments:\n"
           "-h, --help            Show this help message and exit\n"
           "-v, --version         Show program version information and exit\n"
           "-q, --quick           Use quick hashing method.\n"
           "-V, --video           Expect video files instead of image files\n"
           "-b, --bits BITS       Specify hash size:\n"
	   "                             (N^2)*32 bits for images\n"
	   "                             (N^2)*128 bits for videos.\n"
	   "                      Defaults: %d for images (gives %d-bit hash),\n"
	   "                                %d for videos (gives %d-bit hash)\n"
           "--debug               Print debugging information.\n"
	   "                      This includes printing hashes as 2D arrays.\n"
	   ,
	   program_name,
	   DEFAULT_BITS_FOR_IMAGES,
	   hash_size_in_true_bits(DEFAULT_BITS_FOR_IMAGES, 32),
	   DEFAULT_BITS_FOR_VIDEOS,
	   hash_size_in_true_bits(DEFAULT_BITS_FOR_VIDEOS, 128)
    );
}


static void show_version(char* program_name)
{
  printf("%s ver. %s. Copyright (c) %s %s. All rights reserved.", 
	 program_name, 
	 PROGRAM_VERSION, 
	 COPYRIGHT_YEARS, 
	 OWNER_NAME);
}


int main (int argc, char **argv) 
{
    char* path_end = strrchr(argv[0], '/');
    char* program_name = path_end ? path_end + 1: argv[0];
    
    int c;
    int n_failed = 0;
    int n_succeeded = 0;
    int custom_bits_defined = 0;
    int option_index = 0;
    int video = 0;
    hash_computing_task task;

    struct option long_options[] = {
        {"help",    no_argument,        0, 'h'},
        {"version", no_argument,        0, 'v'},
        {"quick",   no_argument,        0, 'q'},
        {"video",   no_argument,        0, 'V'},
        {"bits",    required_argument,  0, 'b'},
        {"debug",   no_argument,        0, 'd'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        show_help(program_name);
        return 1;
    }
    
    memset(&task, 0, sizeof(task));

    while ((c = getopt_long(argc, argv, "hvqVb:d",
                 long_options, &option_index)) != -1) {
        switch (c) {
	  case 'h':
	      show_help(program_name);
	      return 0;

	  case 'v':
	      show_version(program_name);
	      return 0;
	  
	  case 'q':
	      task.quick = 1;
	      break;

	  case 'V':
	      video = 1;
	      break;
	  
	  case 'b':
	      if (sscanf(optarg, "%d", &task.bits) != 1) {
		  fprintf(stderr, "Error: couldn't parse bits argument\n");
		  return 2;
	      }
	      if (task.bits % 4 != 0) {
		  fprintf(stderr, "Error: bits argument should be a multiple of 4\n");
		  return 2;
	      }
	      custom_bits_defined = 1;
	      break;
	  
	  case 'd':
	      task.debug = 1;
	      break;
	  
	  case '?':
	  default:
	      return -1;
        }
    }

    if(optind < argc) {
      
      if(!custom_bits_defined) {
	task.bits = video 
	  ? DEFAULT_BITS_FOR_VIDEOS
	  : DEFAULT_BITS_FOR_IMAGES;
      }
      
      MagickWandGenesis();
      
      while (optind < argc) {
	task.file_name = argv[optind];
	
	int result = video 
	  ? process_video_file(&task)
	  : process_image_file(&task);
	 
	if(result)
	  ++n_failed;
	else
	  ++n_succeeded;
	
	++optind;
      }
      
      MagickWandTerminus();
    }
    
    return n_failed > 0 ? 1 : 0;
}

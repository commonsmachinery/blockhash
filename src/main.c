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
#include <getopt.h>
#include <string.h>
#include <wand/MagickWand.h>
#include <libavformat/avformat.h>
#include "processing.h"
#include "version.h"

static int process_task(const hash_computation_task* task) {
    int result = task->video 
        ? process_video_file(task)
        : process_image_file(task);
    return result;
}

static void show_help(char* prog_name) 
{    
    printf("\n"
           "Usage: %s [-h|--help] [-v|--version] [--quick] [--video] [--bits BITS] [--debug] filenames...\n"
           "\n"
           "Optional arguments:\n"
           "-h, --help            Show this help message and exit.\n"
           "-v, --version         Show program version information and exit.\n"
           "-q, --quick           Use quick hashing method.\n"
           "-V, --video           Expect video files instead of image files.\n"
           "-b, --bits BITS       Specify hash size (N^2) bits.\n"
           "                      Default is %d which gives %d-bit hash.\n"
           "--debug               Print debugging information.\n"
	   "                      This includes printing hashes as 2D arrays.\n"
           "\n"
	   ,
	   prog_name,
	   DEFAULT_BITS,
           DEFAULT_BITS * DEFAULT_BITS
    );
}


static void show_version(char* program_name)
{
  printf("%s ver. %s. Copyright (c) %s %s. All rights reserved.\n", 
	 program_name, 
	 PROGRAM_VERSION, 
	 COPYRIGHT_YEARS, 
	 OWNER_NAME);
}


int main (int argc, char **argv) 
{
    char* path_end = strrchr(argv[0], '/');
    char* prog_name = path_end ? path_end + 1: argv[0];
    
    int c;
    int n_failed = 0;
    int n_succeeded = 0;
    int custom_bits_defined = 0;
    int option_index = 0;
    hash_computation_task task;

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
        show_help(prog_name);
        return 1;
    }
    
    memset(&task, 0, sizeof(task));

    while ((c = getopt_long(argc, argv, "hvqVb:d",
                 long_options, &option_index)) != -1) {
        switch (c) {
	  case 'h':
	      show_help(prog_name);
	      return 0;

	  case 'v':
	      show_version(prog_name);
	      return 0;
	  
	  case 'q':
	      task.quick = 1;
	      break;

	  case 'V':
	      task.video = 1;
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
      
      if(!custom_bits_defined)
	task.bits = DEFAULT_BITS;
      
      if(task.video)
      {
          task.bits = task.bits / 2;

          // register all formats and codecs
          av_register_all();
      }
      
      MagickWandGenesis();
      
      while (optind < argc) {
	task.src_file_name = argv[optind];
	
        int result = process_task(&task);
	 
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

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
#include <getopt.h>
#include <string.h>
#include <libavformat/avformat.h>
#include "pHash-img-c.h"
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
           "Usage: %s [-h|--help] [-v|--version] [--video] [--debug] filenames...\n"
           "\n"
           "Optional arguments:\n"
           "-h, --help            Show this help message and exit.\n"
           "-v, --version         Show program version information and exit.\n"
           "-V, --video           Expect video files instead of image files.\n"
           "-d, --debug           Print debugging information.\n"
           "\n"
	   ,
	   prog_name
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
    int option_index = 0;
    hash_computation_task task;

    struct option long_options[] = {
        {"help",       no_argument,        0, 'h'},
        {"version",    no_argument,        0, 'v'},
        {"video",      no_argument,        0, 'V'},
        {"debug",      no_argument,        0, 'd'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        show_help(prog_name);
        return 1;
    }
    
    memset(&task, 0, sizeof(task));

    while ((c = getopt_long(argc, argv, "hvVd",
                 long_options, &option_index)) != -1) {
        switch (c) {
	  case 'h':
	      show_help(prog_name);
	      return 0;

	  case 'v':
	      show_version(prog_name);
	      return 0;
	  
	  case 'V':
	      task.video = 1;
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
        
        if(task.debug) {
          cph_set_debug_mode();
        }
        
        if(task.video) {
          // register all formats and codecs
          av_register_all();
        }
      
        while (optind < argc) {
            task.src_file_name = argv[optind];
	
            int result = process_task(&task);
	 
            if(result)
                ++n_failed;
            else
                ++n_succeeded;
	
            ++optind;
        }
    }
    
    return n_failed > 0 ? 1 : 0;
}

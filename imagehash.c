/* Perceptual image hash calculation tool based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include "blockhash.h"

#include <wand/MagickWand.h>

int process_image(char * fn, int bits, int quick, int debug)
{
    int i, j;
    size_t width, height;
    unsigned char *image_data;
    int *hash;
    MagickBooleanType status;
    MagickWand *magick_wand;

    magick_wand = NewMagickWand();
    status = MagickReadImage(magick_wand, fn);

    if (status == MagickFalse) {
        printf("Error opening image file %s\n", fn);
        exit(-1);
    }

    // Remove color profiles for interoperability with other hashing tools
    MagickProfileImage(magick_wand, "*", NULL, 0);

    width = MagickGetImageWidth(magick_wand);
    height = MagickGetImageHeight(magick_wand);

    image_data = malloc(width * height * 4);

    status = MagickExportImagePixels(magick_wand, 0, 0, width, height, "RGBA", CharPixel, image_data);

    if (status == MagickFalse) {
        printf("Error converting image data to RGBA\n");
        exit(-1);
    }

    hash = malloc(bits * bits * sizeof(int));

    if (quick) {
        blockhash_quick(bits, image_data, width, height, &hash);
    } else {
        blockhash(bits, image_data, width, height, &hash);
    }

    if (debug) {
        for (i = 0; i < bits * bits; i++) {
            if (i != 0 && i % bits == 0)
                printf("\n");
            printf("%d", hash[i]);
        }
        printf("\n");
    }

    char* hex = bits_to_hexhash(hash, bits*bits);
    printf("%s  %s\n", hex, fn);

    free(hex);
    free(hash);
    free(image_data);

    DestroyMagickWand(magick_wand);
}

void help() {
    printf("Usage: blockhash [-h|--help] [--quick] [--bits BITS] [--debug] filenames...\n"
           "\n"
           "Optional arguments:\n"
           "-h, --help            Show this help message and exit\n"
           "-q, --quick           Use quick hashing method.\n"
           "-b, --bits BITS       Create hash of size N^2 bits. Default: 16\n"
           "--debug               Print hashes as 2D maps (for debugging)\n");
}

void main (int argc, char **argv) {
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
            process_image(argv[optind++], bits, quick, debug);
        }
    }

    MagickWandTerminus();
    
    exit(0);
}

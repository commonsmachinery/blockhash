/*
 * Perceptual image hash calculation tool based on pHash algorithm
 *
 * Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an GPLv3 license, please see file COPYING in the top dir.
 */

/*
    The folloing code is derived from this work:

    pHash, the open source perceptual hash library
    Copyright (C) 2008-2009 Aetilius, Inc.
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Evan Klinger - eklinger@phash.org
    D Grant Starkweather - dstarkweather@phash.org

*/

#ifndef PHASH_IMG_C_H__
#define PHASH_IMH_C_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PHASH_Digest_DEFINED
#define PHASH_Digest_DEFINED
 /*! /brief Digest info
 */
typedef struct ph_digest {
    char *id;                   //hash id
    uint8_t *coeffs;            //the head of the digest integer coefficient array
    int size;                   //the size of the coeff array
} Digest;
#endif

/*! /brief enable debug mode
 *  enables debug mode
 */
void cph_set_debug_mode();

/*! /brief image digest
 *  Compute the image digest given the file name.
 *  /param file - string value for file name of input image.
 *  /param sigma - double value for the deviation for gaussian filter
 *  /param gamma - double value for gamma correction on the input image.
 *  /param digest - Digest struct
 *  /param N      - int value for number of angles to consider (recommnded default 180)
 */
int cph_image_digest(const char *file, double sigma, double gamma, Digest* digest,int N);

/*! /brief Compute the image digest for the in-memory BMP image
 *  Compute the image digest given the file name.
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param sigma - double value for the deviation for gaussian filter
 *  /param gamma - double value for gamma correction on the input image.
 *  /param digest - Digest struct
 *  /param N      - int value for number of angles to consider (recommnded default 180)
 */
int cph_mbmp_image_digest(const uint8_t* image_buffer, size_t image_size, double sigma, double gamma, Digest* digest,int N);

/*! /brief compute dct robust image hash
 *  /param file string variable for name of file
 *  /param hash of type ulong64 (must be 64-bit variable)
 *  /return int value - -1 for failure, 1 for success
 */
int cph_dct_imagehash(const char* file, uint64_t* hash);

/*! /brief compute dct robust image hash for the in-memory BMP image
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param hash of type ulong64 (must be 64-bit variable)
 *  /return int value - -1 for failure, 1 for success
 */
int cph_mbmp_dct_imagehash(const uint8_t* image_buffer, size_t image_size, uint64_t* hash);

/*! /brief create MH image hash for filename image
 *  /param filename - string name of image file
 *  /param N - (out) int value for length of image hash returned
 *  /param alpha - float scale factor for marr wavelet (recommended default 2)
 *  /param lvl   - float level of scale factor (recommended default 1)
 *  /return uint8_t array
 */
uint8_t* cph_mh_imagehash(const char* filename, int* N,float alpha, float lvl);

/*! /brief create MH image hash for in-memory BMP image
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param N - (out) int value for length of image hash returned
 *  /param alpha - float scale factor for marr wavelet (recommeded default 2)
 *  /param lvl   - float level of scale factor (recommended default 1)
 *  /return uint8_t array
 */
uint8_t* cph_mbmp_mh_imagehash(const uint8_t* image_buffer, size_t image_size, int* N, float alpha, float lvl);

/*! /brief compute hamming distance between two hashes
 *  /param hash1 - first hash
 *  /param hash2 - second hash
 *  /return int value for hamming distance
 */
int cph_hamming_distance(const uint64_t hash1, const uint64_t hash2);

/*! /brief compute hamming distance between two byte arrays
 *  /param hashA - byte array for first hash
 *  /param lenA - int length of hashA 
 *  /param hashB - byte array for second hash
 *  /param lenB - int length of hashB
 *  /return double value for normalized hamming distance
 */
double cph_hammingdistance2(uint8_t *hashA, int lenA, uint8_t *hashB, int lenB);

#ifdef __cplusplus
} // extern "C"
#endif
    
#endif

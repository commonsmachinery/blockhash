/*
 * Perceptual image hash calculation tool based on pHash algorithm
 *
 * Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an GPLv3 license, please see file COPYING in the top dir.
 */

/*
    The folloing code is derived from this work:

    pHash, the open source perceptual hash library
    Copyright (C) 2009 Aetilius, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdio>
#include "CImg-ext.h"
using namespace cimg_library;
#include "pHash-img.h"
#include "pHash-img-c.h"

extern "C" {

void cph_set_debug_mode() {
    ph_set_debug_mode();
}
    
int cph_image_digest(const char *file, double sigma, double gamma, Digest* digest,int N) {
    int res = -1;
    try {
        res = ph_image_digest(file, sigma, gamma, *digest, N);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return res;
}

int cph_mbmp_image_digest(const uint8_t* image_buffer, size_t image_size, double sigma, double gamma, Digest* digest,int N) {
    int res = -1;
    try {
        res = ph_mbmp_image_digest(image_buffer, image_size, sigma, gamma, *digest, N);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return res;
}

int cph_dct_imagehash(const char* file, uint64_t* hash) {
    int res = -1;
    try {
        res = ph_dct_imagehash(file, *hash);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return res;
}

int cph_mbmp_dct_imagehash(const uint8_t* image_buffer, size_t image_size, uint64_t* hash) {
    int res = -1;
    try {
        res = ph_mbmp_dct_imagehash(image_buffer, image_size, *hash);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return res;
}

uint8_t* cph_mh_imagehash(const char* filename, int* N,float alpha, float lvl) {
    uint8_t* hash = NULL;
    try {
        hash = ph_mh_imagehash(filename, *N, alpha, lvl);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return hash;
}

uint8_t* cph_mbmp_mh_imagehash(const uint8_t* image_buffer, size_t image_size, int* N,float alpha, float lvl) {
    uint8_t* hash = NULL;
    try {
        hash = ph_mbmp_mh_imagehash(image_buffer, image_size, *N, alpha, lvl);
    } catch(std::exception& ex) {
        fprintf(stderr, "Error: %s\n", ex.what());
    }
    return hash;
}

int cph_hamming_distance(const uint64_t hash1,const uint64_t hash2){
    return ph_hamming_distance(hash1, hash2);
}

double cph_hammingdistance2(uint8_t *hashA, int lenA, uint8_t *hashB, int lenB) {
    return ph_hammingdistance2(hashA, lenA, hashB, lenB);
}

} // extern "C"

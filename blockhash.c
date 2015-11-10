/*
 * Perceptual image hash calculation library based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// comparison function for the qsort
static int cmpint(const void *pa, const void *pb)
{
    int a = *(const int *) pa;
    int b = *(const int *) pb;
    return (a < b) ? -1 : (a > b);
}


// comparison function for the qsort
static int cmpfloat(const void *pa, const void *pb)
{
    float a = *(const float *) pa;
    float b = *(const float *) pb;
    return (a < b) ? -1 : (a > b);
}


static float median(int *data, int n)
{
    int *sorted;
    float result;

    sorted = malloc(n * sizeof(int));
    memcpy(sorted, data, n * sizeof(int));
    qsort(sorted, n, sizeof(int), cmpint);

    if (n % 2 == 0) {
        result = (float) (sorted[n / 2] + sorted[n / 2 + 1]) / 2;
    } else {
        result = (float) sorted[n / 2];
    }

    free(sorted);
    return result;
}


static float medianf(float *data, int n)
{
    float *sorted;
    float result;

    sorted = malloc(n * sizeof(float));
    memcpy(sorted, data, n * sizeof(float));
    qsort(sorted, n, sizeof(float), cmpfloat);

    if (n % 2 == 0) {
        result = (sorted[n / 2] + sorted[n / 2 + 1]) / 2;
    } else {
        result = sorted[n / 2];
    }

    free(sorted);
    return result;
}

/** Compare medians across four horizontal bands
 *
 * Output a 1 if the block is brighter than the median.
 * With images dominated by black or white, the median may
 * end up being 0 or the max value, and thus having a lot
 * of blocks of value equal to the median.  To avoid
 * generating hashes of all zeros or ones, in that case output
 * 0 if the median is in the lower value space, 1 otherwise
 */
void translate_blocks_to_bits(int *blocks, int nblocks, int pixels_per_block)
{
    float half_block_value;
    int bandsize, i, j, v;
    float m;

    half_block_value = pixels_per_block * 256 * 3 / 2;
    bandsize = nblocks / 4;

    for (i = 0; i < 4; i++) {
        m = median(&blocks[i * bandsize], bandsize);
        for (j = i * bandsize; j < (i + 1) * bandsize; j++) {
            v = blocks[j];
            blocks[j] = v > m || (abs(v - m) < 1 && m > half_block_value);
        }
    }
}

void translate_blocks_to_bitsf(float *blocks, int *result, int nblocks, int pixels_per_block)
{
    float half_block_value;
    int bandsize, i, j;
    float m, v;

    half_block_value = pixels_per_block * 256 * 3 / 2;
    bandsize = nblocks / 4;

    for (i = 0; i < 4; i++) {
        m = medianf(&blocks[i * bandsize], bandsize);
        for (j = i * bandsize; j < (i + 1) * bandsize; j++) {
            v = blocks[j];
            result[j] = v > m || (abs(v - m) < 1 && m > half_block_value);
        }
    }
}

/** Convert array of bits to hexadecimal string representation.
 * Hash length should be a multiple of 4.
 *
 * Returns: null-terminated hexadecimal string hash.
 */
char* bits_to_hexhash(int *bits, int nbits)
{
    int    i, j, b;
    int    len;
    int    tmp;
    char  *hex;
    char  *stmp;

    len = nbits / 4;

    hex = malloc(len + 1);
    if(!hex) return NULL;
    stmp = malloc(2);
    if(!stmp) {
      free(hex);
      return NULL;
    }
    hex[len] = '\0';

    for (i = 0; i < len; i++) {
        tmp = 0;
        for (j = 0; j < 4; j++) {
            b = i * 4 + j;
            tmp = tmp | (bits[b] << 3 >> j);
        }

        sprintf(stmp, "%1x", tmp);
        hex[i] = stmp[0];
    }

    free(stmp);
    return hex;
}


int blockhash_quick(int bits, unsigned char *data, int width, int height, int **hash)
{
    int    x, y, ix, iy;
    int    ii, alpha, value;
    int    block_width;
    int    block_height;
    int   *blocks;

    block_width = width / bits;
    block_height = height / bits;

    blocks = calloc(bits * bits, sizeof(int));
    if(!blocks) return -1;
    
    for (y = 0; y < bits; y++) {
        for (x = 0; x < bits; x++) {
            value = 0;

            for (iy = 0; iy < block_height; iy++) {
                for (ix = 0; ix < block_width; ix++) {
                    ii = ((y * block_height + iy) * width + (x * block_width + ix)) * 4;

                    alpha = data[ii+3];
                    if (alpha == 0) {
                        value += 765;
                    } else {
                        value += data[ii] + data[ii+1] + data[ii+2];
                    }
                }
            }

            blocks[y * bits + x] = value;
        }
    }

    translate_blocks_to_bits(blocks, bits * bits, block_width * block_height);
    *hash = blocks;
    return 0;
}


int blockhash(int bits, unsigned char *data, int width, int height, int **hash)
{
    float   block_width;
    float   block_height;
    float   y_frac, y_int;
    float   x_frac, x_int;
    float   x_mod, y_mod;
    float   weight_top, weight_bottom, weight_left, weight_right;
    int     block_top, block_bottom, block_left, block_right;
    int     x, y, ii, alpha;
    float   value;
    float  *blocks;
    int    *result;

    if (width % bits == 0 && height % bits == 0) {
        return blockhash_quick(bits, data, width, height, hash);
    }

    block_width = (float) width / (float) bits;
    block_height = (float) height / (float) bits;

    blocks = calloc(bits * bits, sizeof(float));
    if(!blocks) return -1;
    
    result = malloc(bits * bits * sizeof(int));
    if(!result) {
	free(blocks);
	return -1;
    }

    for (y = 0; y < height; y++) {
        y_mod = fmodf(y + 1, block_height);
        y_frac = modff(y_mod, &y_int);

        weight_top = (1 - y_frac);
        weight_bottom = y_frac;

        // y_int will be 0 on bottom/right borders and on block boundaries
        if (y_int > 0 || (y + 1) == height) {
            block_top = block_bottom = (int) floor((float) y / block_height);
        } else {
            block_top = (int) floor((float) y / block_height);
            block_bottom = (int) ceil((float) y / block_height);
        }

        for (x = 0; x < width; x++) {
            x_mod = fmodf(x + 1, block_width);
            x_frac = modff(x_mod, &x_int);

            weight_left = (1 - x_frac);
            weight_right = x_frac;

            // x_int will be 0 on bottom/right borders and on block boundaries
            if (x_int > 0 || (x + 1) == width) {
                block_left = block_right = (int) floor((float) x / block_width);
            } else {
                block_left = (int) floor((float) x / block_width);
                block_right = (int) ceil((float) x / block_width);
            }

            ii = (y * width + x) * 4;

            alpha = data[ii + 3];
            if (alpha == 0) {
                value = 765;
            } else {
                value = data[ii] + data[ii + 1] + data[ii + 2];
            }

            // add weighted pixel value to relevant blocks
            blocks[block_top * bits + block_left] += value * weight_top * weight_left;
            blocks[block_top * bits + block_right] += value * weight_top * weight_right;
            blocks[block_bottom * bits + block_left] += value * weight_bottom * weight_left;
            blocks[block_bottom * bits + block_right] += value * weight_bottom * weight_right;
        }
    }

    translate_blocks_to_bitsf(blocks, result, bits * bits, block_width * block_height);
    *hash = result;
    free(blocks);

    *hash = result;
    return 0;  
}

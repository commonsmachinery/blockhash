/*
 * Perceptual image hash calculation library based on algorithm descibed in
 * Block Mean Value Based Image Perceptual Hashing by Bian Yang, Fan Gu and Xiamu Niu
 *
 * Copyright 2014-2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an MIT license, please see LICENSE in the top dir.
 */

#ifndef __BLOCKHASH_BLOCKHASH_H__
#define __BLOCKHASH_BLOCKHASH_H__

#ifdef __cplusplus
extern "C" {
#endif
  
/** Convert array of bits to hexadecimal string representation.
 * Hash length should be a multiple of 4.
 *
 * Returns: null-terminated hexadecimal string hash on succes, NULL on failure.
 */
char* blockhash_bits_to_hex_str(int *bits, int nbits);


/** Calculate perceptual hash for an RGBA image using quick method.
*
* Quick method uses rounded block sizes and is less accurate in case image
* width and height are not divisible by the number of bits.
*
* Parameters:
*
* bits - number of blocks to divide the image by horizontally and vertically.
* data - RGBA image data.
* width - image width.
* height - image height.
* hash - the resulting hash will be allocated and stored in the given array as bits.
* 
* Returns: 0 on success, -1 on failure.
*/
int blockhash_quick(int bits, unsigned char *data, int width, int height, int **hash);


/** Calculate perceptual hash for an RGBA image using precise method.
*
* Precise method puts weighted pixel values to blocks according to pixel
* area falling within a given block and provides more accurate results
* in case width and height are not divisible by the number of bits.
*
* Parameters:
*
* bits - number of blocks to divide the image by horizontally and vertically.
* data - RGBA image data.
* width - image width.
* height - image height.
* hash - the resulting hash will be allocated and stored in the given array as bits.
* 
* Returns: 0 on success, -1 on failure.
*/
int blockhash(int bits, unsigned char *data, int width, int height, int **hash);

  
#ifdef __cplusplus
} 
#endif

#endif

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

#ifndef PHASH_IMG_H__
#define PHASH_IMG_H__

#include <cstdint>

#define SQRT_TWO 1.4142135623730950488016887242097

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

#define ROUNDING_FACTOR(x) (((x) >= 0) ? 0.5 : -0.5) 

#ifdef __cplusplus
extern "C" {
#endif


/* structure for a single hash */
typedef struct ph_datapoint {
    char *id;
    void *hash;
    float *path;
    uint32_t hash_length;
    uint8_t hash_type;
}DP;

typedef struct ph_slice {
    DP **hash_p;
    int n;
    void *hash_params;
} slice;

struct BinHash 
{
	uint8_t *hash;
	uint32_t bytelength;
	uint32_t byteidx; // used by addbit()
	uint8_t bitmask;  // used by addbit()

	/*
	 * add a single bit to hash. the bits are 
	 * written from left to right.
	 */
	int addbit(uint8_t bit)
	{
		if (bitmask == 0) 
		{
			bitmask = 128; // reset bitmask to "10000000"
			byteidx++;     // jump to next byte in array
		}

		if (byteidx >= bytelength) return -1;
		
		if (bit == 1) *(hash + byteidx) |= bitmask;
		bitmask >>=1;
		return 0;
	}	
};

BinHash* _ph_bmb_new(uint32_t bytelength);
void ph_bmb_free(BinHash *binHash);

/*! /brief Radon Projection info
 */
typedef struct ph_projections {
    CImg<uint8_t> *R;           //contains projections of image of angled lines through center
    int *nb_pix_perline;        //the head of int array denoting the number of pixels of each line
    int size;                   //the size of nb_pix_perline
}Projections;

/*! /brief feature vector info
 */
typedef struct ph_feature_vector {
    double *features;           //the head of the feature array of double's
    int size;                   //the size of the feature array
}Features;

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

#define ROTATELEFT(x, bits)  (((x)<<(bits)) | ((x)>>(64-bits)))

/*! /brief enable debug mode
 *  enables debug mode
 */
void ph_set_debug_mode();


/*! /brief alloc a single data point
 *  allocates path array, does nto set id or path
 */
 DP* ph_malloc_datapoint(int hashtype);

/*! /brief free a datapoint and its path
 *
 */
void ph_free_datapoint(DP *dp);

/*! /brief radon function
 *  Find radon projections of N lines running through the image center for lines angled 0
 *  to 180 degrees from horizontal.
 *  /param img - CImg src image
 *  /param  N  - int number of angled lines to consider.
 *  /param  projs - (out) Projections struct 
 *  /return int value - less than 0 for error
 */
int ph_radon_projections(const CImg<uint8_t> &img,int N,Projections &projs);

/*! /brief feature vector
 *         compute the feature vector from a radon projection map.
 *  /param  projs - Projections struct
 *  /param  fv    - (out) Features struct
 *  /return int value - less than 0 for error
*/
int ph_feature_vector(const Projections &projs,Features &fv);

/*! /brief dct 
 *  Compute the dct of a given vector
 *  /param R - vector of input series
 *  /param D - (out) the dct of R
 *  /return  int value - less than 0 for error
*/
int ph_dct(const Features &fv, Digest &digest);

/*! /brief cross correlation for 2 series
 *  Compute the cross correlation of two series vectors
 *  /param x - Digest struct
 *  /param y - Digest struct
 *  /param pcc - double value the peak of cross correlation
 *  /param threshold - double value for the threshold value for which 2 images
 *                     are considered the same or different.
 *  /return - int value - 1 (true) for same, 0 (false) for different, < 0 for error
 */

int ph_crosscorr(const Digest &x, const Digest &y, double &pcc, double threshold = 0.90);

/*! /brief image digest
 *  Compute the image digest for an image given the input image
 *  /param img - CImg object representing an input image
 *  /param sigma - double value for the deviation for a gaussian filter function 
 *  /param gamma - double value for gamma correction on the input image
 *  /param digest - (out) Digest struct
 *  /param N      - int value for the number of angles to consider. 
 *  /return       - less than 0 for error
 */
int _ph_image_digest(CImg<uint8_t> &img, double sigma, double gamma, Digest &digest, int N=180);

/*! /brief image digest
 *  Compute the image digest given the file name.
 *  /param file - string value for file name of input image.
 *  /param sigma - double value for the deviation for gaussian filter
 *  /param gamma - double value for gamma correction on the input image.
 *  /param digest - Digest struct
 *  /param N      - int value for number of angles to consider
 */
int ph_image_digest(const char *file, double sigma, double gamma, Digest &digest, int N=180);

/*! /brief image digest
 *  Compute the image digest for the given BMP image in the memory buffer
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param sigma - double value for the deviation for gaussian filter
 *  /param gamma - double value for gamma correction on the input image.
 *  /param digest - Digest struct
 *  /param N      - int value for number of angles to consider
 */
int ph_mbmp_image_digest(const uint8_t* image_buffer, size_t image_size, double sigma, double gamma, Digest &digest, int N=180);

/*! /brief compare 2 images
 *  /param imA - CImg object of first image 
 *  /param imB - CImg object of second image
 *  /param pcc   - (out) double value for peak of cross correlation
 *  /param sigma - double value for the deviation of gaussian filter
 *  /param gamma - double value for gamma correction of images
 *  /param N     - int number for the number of angles of radon projections
 *  /param theshold - double value for the threshold
 *  /return int 0 (false) for different images, 1 (true) for same image, less than 0 for error
 */
int _ph_compare_images(CImg<uint8_t> &imA, CImg<uint8_t> &imB, double &pcc, 
                       double sigma = 3.5, double gamma = 1.0, int N=180, double threshold=0.90);

/*! /brief compare 2 images
 *  Compare 2 images given the file names
 *  /param file1 - char string of first image file
 *  /param file2 - char string of second image file
 *  /param pcc   - (out) double value for peak of cross correlation
 *  /param sigma - double value for deviation of gaussian filter
 *  /param gamma - double value for gamma correction of images
 *  /param N     - int number for number of angles
 *  /return int 0 (false) for different image, 1 (true) for same images, less than 0 for error
 */
int ph_compare_images(const char *file1, const char *file2, double &pcc, 
                      double sigma = 3.5, double gamma=1.0, int N=180, double threshold=0.90);

/*! /brief return dct matrix, C
 *  Return DCT matrix of sqare size, N
 *  /param N - int denoting the size of the square matrix to create.
 *  /return CImg<double> size NxN containing the dct matrix
 */
CImg<float>* ph_dct_matrix(const int N);

/*! /brief compute dct robust image hash for the given image file
 *  /param file string variable for name of file
 *  /param hash of type uint64_t (must be 64-bit variable)
 *  /return int value - -1 for failure, 1 for success
 */
int ph_dct_imagehash(const char* file, uint64_t &hash);

/*! /brief compute dct robust image hash for the given in-memory BMP image
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param hash of type uint64_t (must be 64-bit variable)
 *  /return int value - -1 for failure, 1 for success
 */
int ph_mbmp_dct_imagehash(const uint8_t* image_buffer, size_t image_size, uint64_t &hash);

/*! /brief create MH image hash for filename image
 *  /param filename - string name of image file
 *  /param N - (out) int value for length of image hash returned
 *  /param alpha - int scale factor for marr wavelet (default=2)
 *  /param lvl   - int level of scale factor (default = 1)
 *  /return uint8_t array
 */
uint8_t* ph_mh_imagehash(const char *filename, int &N, float alpha=2.0f, float lvl = 1.0f);

/*! /brief create MH image hash for filename image
 *  /param image_buffer - uint8_t* image buffer address 
 *  /param image_size - size_t image data size
 *  /param N - (out) int value for length of image hash returned
 *  /param alpha - int scale factor for marr wavelet (default=2)
 *  /param lvl   - int level of scale factor (default = 1)
 *  /return uint8_t array
 */
uint8_t* ph_mbmp_mh_imagehash(const uint8_t* image_buffer, size_t image_size, int &N, 
                              float alpha=2.0f, float lvl = 1.0f);

/*! /brief count number bits set in given byte
 *  /param val - uint8_t byte value
 *  /return int value for number of bits set
 */
int ph_bitcount8(uint8_t val);

/*! /brief compute hamming distance between two hashes
 *  /param hash1 - first hash
 *  /param hash2 - second hash
 *  /return int value for hamming distance
 */
int ph_hamming_distance(const uint64_t hash1, const uint64_t hash2);

/*! /brief compute hamming distance between two byte arrays
 *  /param hashA - byte array for first hash
 *  /param lenA - int length of hashA 
 *  /param hashB - byte array for second hash
 *  /param lenB - int length of hashB
 *  /return double value for normalized hamming distance
 */
double ph_hammingdistance2(uint8_t *hashA, int lenA, uint8_t *hashB, int lenB);

#ifdef __cplusplus
}
#endif

#endif

#ifndef __BLOCKHASH_BMP_IMAGE_H__
#define __BLOCKHASH_BMP_IMAGE_H__ 

#pragma pack(push, 1)

struct _bitmap_file_header 
{ 
   uint16_t bfType; // +0
   uint32_t bfSize; // +2
   uint16_t bfReserved1; // +6
   uint16_t bfReserved2; // +8
   uint32_t bfOffBits; // +10
};

typedef struct _bitmap_file_header bitmap_file_header;

struct _bitmap_info_header
{
   uint32_t  biSize; // +14
   int32_t   biWidth; // +18
   int32_t   biHeight; // +22
   uint16_t  biPlanes; // + 26
   uint16_t  biBitCount; // +28
   uint32_t  biCompression; // +30
   uint32_t  biSizeImage; // +34
   int32_t   biXPelsPerMeter; // +38
   int32_t   biYPelsPerMeter; // +42
   uint32_t  biClrUsed; // + 46
   uint32_t  biClrImportant; // +50
};
 
typedef struct _bitmap_info_header bitmap_info_header;

struct _bitmap_image {
    bitmap_file_header bfh;
    bitmap_info_header bih;
    uint8_t data[1];
};

typedef struct _bitmap_image bitmap_image;

#pragma pack(pop)

#endif

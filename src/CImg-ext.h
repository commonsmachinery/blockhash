/*
 * Perceptual image hash calculation tool based on pHash algorithm
 *
 * Copyright (c) 2015 Commons Machinery http://commonsmachinery.se/
 * Distributed under an GPLv3 license, please see file COPYING in the top dir.
 */

/*
 # 
 #  THE FOLLOWING CODE IS DERIVED FROM THE FOLLOWING WORK:
 #
 #  File            : CImg.h
 #                    ( C++ header file )
 #
 #  Description     : The C++ Template Image Processing Toolkit.
 #                    This file is the main component of the CImg Library project.
 #                    ( http://cimg.eu )
 #
 #  Project manager : David Tschumperle.
 #                    ( http://tschumperle.users.greyc.fr/ )
 #
 #                    A complete list of contributors is available in file 'README.txt'
 #                    distributed within the CImg package.
 #
 #  Licenses        : This file is 'dual-licensed', you have to choose one
 #                    of the two licenses below to apply.
 #
 #                    CeCILL-C
 #                    The CeCILL-C license is close to the GNU LGPL.
 #                    ( http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html )
 #
 #                or  CeCILL v2.0
 #                    The CeCILL license is compatible with the GNU GPL.
 #                    ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
 #
 #  This software is governed either by the CeCILL or the CeCILL-C license
 #  under French law and abiding by the rules of distribution of free software.
 #  You can  use, modify and or redistribute the software under the terms of
 #  the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
 #  at the following URL: "http://www.cecill.info".
 #
 #  As a counterpart to the access to the source code and  rights to copy,
 #  modify and redistribute granted by the license, users are provided only
 #  with a limited warranty  and the software's author,  the holder of the
 #  economic rights,  and the successive licensors  have only  limited
 #  liability.
 #
 #  In this respect, the user's attention is drawn to the risks associated
 #  with loading,  using,  modifying and/or developing or reproducing the
 #  software by the user in light of its specific status of free software,
 #  that may mean  that it is complicated to manipulate,  and  that  also
 #  therefore means  that it is reserved for developers  and  experienced
 #  professionals having in-depth computer knowledge. Users are therefore
 #  encouraged to load and test the software's suitability as regards their
 #  requirements in conditions enabling the security of their systems and/or
 #  data to be ensured and,  more generally, to use and operate it in the
 #  same conditions as regards security.
 #
 #  The fact that you are presently reading this means that you have had
 #  knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
 #
*/

#ifndef PHASHTOOL_CIMG_EXT_H__
#define PHASHTOOL_CIMG_EXT_H__

#include "CImg.h"
#include <cassert>

namespace cimg_library_suffixed {

#define cimg_instance2 this->_width,this->_height,this->_depth,this->_spectrum,this->_data,this->_is_shared?"":"non-",this->pixel_type()

template<typename T>
struct CImgExt : public CImg<T> {
    
    // [IP] loading from stream support
    struct input_stream {
        virtual ~input_stream() {}
        virtual size_t read(void* dest, size_t n) = 0;
        virtual int seek(off_t off, int whence) = 0;
        virtual void close() = 0;
        virtual size_t size() = 0;
        virtual const char* name() = 0;
    };
    
    // [IP] loading from stream support
    struct file_input_stream : public input_stream {
        file_input_stream(const char* filename) 
        :
            m_name(filename),
            m_file(fopen(filename, "rb")),
            m_auto_close(true)
        {
            if(!m_file) {
                std::ostringstream str;
                str << "Can't open image file '" << filename << "'";
                throw std::runtime_error(str.str().c_str());
            }
        }

        file_input_stream(FILE* f, bool auto_close = false) 
        :
            m_name("(FILE*)"),
            m_file(f),
            m_auto_close(auto_close)
        {
            assert(f);
        }
        
        virtual size_t read(void* dest, size_t n) {
            int res = fread(dest, 1, n, m_file);
            return static_cast<size_t>(res);
        }

        virtual int seek(off_t off, int whence) {
            return fseek(m_file, off, whence);
        }
        
        virtual void close() {
            fclose(m_file);
            m_file = 0;
        }

        virtual size_t size() {
            long pos = ftell(m_file);
            fseek(m_file, 0, SEEK_END);
            size_t n = ftell(m_file);
            fseek(m_file, pos, SEEK_SET);
            return n;
        }
        
        virtual ~file_input_stream() {
            if(m_file && m_auto_close)
                fclose(m_file);
        }
        
        virtual const char* name() {
            return m_name.c_str();
        }
        
        std::string m_name;
        FILE* m_file;
        bool m_auto_close;
    };
    
    // [IP] loading from stream support
    struct memory_input_stream : public input_stream {
        
        memory_input_stream(const uint8_t* buffer, size_t buffer_size) 
        :
            m_ptr(buffer),
            m_buffer(buffer),
            m_size(buffer_size)
        {
            assert(buffer);
            assert(buffer_size);
        }
        
        virtual ~memory_input_stream() {}
        
        virtual size_t read(void* dest, size_t n) {
            size_t available = m_size - (m_ptr - m_buffer);
            size_t effective_n = (available >= n) ? n : available;
            if(effective_n) {
                memcpy(dest, m_ptr, effective_n);
                m_ptr += effective_n;
            }
            return effective_n;
        }
        
        virtual int seek(off_t off, int whence) {
            switch(whence) {
                case SEEK_SET: {
                    if(off >= 0 && off < m_size) {
                        m_ptr = m_buffer + off;
                        return 0;
                    }
                    break;
                }
                case SEEK_CUR: {
                    const uint8_t* newptr = m_ptr + off;
                    if(newptr >= m_buffer && newptr < m_buffer + m_size) {
                        m_ptr = newptr;
                        return 0;
                    }
                }
                case SEEK_END: {
                    if(off >= 0 && off < m_size) {
                        m_ptr = m_buffer + (m_size - off);
                        return 0;
                    }
                    break;
                }
            }
            return -1;
        }
        
        virtual void close() {
            m_ptr = m_buffer + m_size;
        }
        
        virtual size_t size() {
            return m_size;
        }

        virtual const char* name() {
            return "(memory buffer)";
        }
        
        const uint8_t* m_ptr;
        const uint8_t* m_buffer;
        size_t   m_size;
    };
    
    CImg<T>& load_bmp2(const uint8_t* image_buffer, size_t image_size) {
      memory_input_stream ms(image_buffer, image_size);
      return _load_bmp2(ms);
    }

    CImg<T>& _load_bmp2(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "_load_bmp2(): Specified filename is (null).",
                                    cimg_instance2);
        
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      if(!nfile) throw std::runtime_error("Can't open image file");
      file_input_stream fs(nfile, file == NULL);
      return _load_bmp2(fs);
    }
    
    CImg<T>& _load_bmp2(input_stream& is) {
 
      unsigned char header[64] = { 0 };
      is.read(header,54);
      if (*header!='B' || header[1]!='M') {
        throw CImgIOException(_cimg_instance
                              "_load_bmp2(): Invalid BMP file '%s'.",
                              cimg_instance2,
                              is.name());
      }

      // Read header and pixel buffer
      int
        file_size = header[0x02] + (header[0x03]<<8) + (header[0x04]<<16) + (header[0x05]<<24),
        offset = header[0x0A] + (header[0x0B]<<8) + (header[0x0C]<<16) + (header[0x0D]<<24),
        header_size = header[0x0E] + (header[0x0F]<<8) + (header[0x10]<<16) + (header[0x11]<<24),
        dx = header[0x12] + (header[0x13]<<8) + (header[0x14]<<16) + (header[0x15]<<24),
        dy = header[0x16] + (header[0x17]<<8) + (header[0x18]<<16) + (header[0x19]<<24),
        compression = header[0x1E] + (header[0x1F]<<8) + (header[0x20]<<16) + (header[0x21]<<24),
        nb_colors = header[0x2E] + (header[0x2F]<<8) + (header[0x30]<<16) + (header[0x31]<<24),
        bpp = header[0x1C] + (header[0x1D]<<8);

      if (!file_size || file_size==offset) {
          file_size = is.size();
      }
      
      if (header_size>40) is.seek(header_size - 40, SEEK_CUR);

      const int
        cimg_iobuffer = 24*1024*1024,
        dx_bytes = (bpp==1)?(dx/8 + (dx%8?1:0)):((bpp==4)?(dx/2 + (dx%2?1:0)):(dx*bpp/8)),
        align_bytes = (4 - dx_bytes%4)%4,
        buf_size = cimg::min(cimg::abs(dy)*(dx_bytes + align_bytes),file_size - offset);

      CImg<int32_t> colormap;
      if (bpp<16) { if (!nb_colors) nb_colors = 1<<bpp; } else nb_colors = 0;
      if (nb_colors) { colormap.assign(nb_colors); is.read(colormap._data,nb_colors); }
      const int xoffset = offset - 14 - header_size - 4*nb_colors;
      if (xoffset>0) is.seek(xoffset,SEEK_CUR);

      CImg<uint8_t> buffer;
      if (buf_size<cimg_iobuffer) { buffer.assign(buf_size); is.read(buffer._data,buf_size); }
      else buffer.assign(dx_bytes + align_bytes);
      unsigned char *ptrs = buffer;

      // Decompress buffer (if necessary)
      if (compression) {
        if (is.name()[0] == '(')
          throw CImgIOException(_cimg_instance
                                "_load_bmp2(): Unable to load compressed data from '%s' inputs.",
                                is.name(),
                                cimg_instance2);
        else {
          is.close();
          return this->load_other(is.name());
        }
      }

      // Read pixel data
      this->assign(dx,cimg::abs(dy),1,3);
      switch (bpp) {
      case 1 : { // Monochrome
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          unsigned char mask = 0x80, val = 0;
          cimg_forX(*this,x) {
            if (mask==0x80) val = *(ptrs++);
            const unsigned char *col = (unsigned char*)(colormap._data + (val&mask?1:0));
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
            mask = cimg::ror(mask);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 4 : { // 16 colors
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          unsigned char mask = 0xF0, val = 0;
          cimg_forX(*this,x) {
            if (mask==0xF0) val = *(ptrs++);
            const unsigned char color = (unsigned char)((mask<16)?(val&mask):((val&mask)>>4));
            const unsigned char *col = (unsigned char*)(colormap._data + color);
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
            mask = cimg::ror(mask,4);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 8 : { //  256 colors
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            const unsigned char *col = (unsigned char*)(colormap._data + *(ptrs++));
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 16 : { // 16 bits colors
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            const unsigned char c1 = *(ptrs++), c2 = *(ptrs++);
            const unsigned short col = (unsigned short)(c1|(c2<<8));
            (*this)(x,y,2) = (T)(col&0x1F);
            (*this)(x,y,1) = (T)((col>>5)&0x1F);
            (*this)(x,y,0) = (T)((col>>10)&0x1F);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 24 : { // 24 bits colors
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            (*this)(x,y,2) = (T)*(ptrs++);
            (*this)(x,y,1) = (T)*(ptrs++);
            (*this)(x,y,0) = (T)*(ptrs++);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 32 : { // 32 bits colors
        for (int y = this->height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            is.read(ptrs=buffer._data,dx_bytes);
            is.seek(align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            (*this)(x,y,2) = (T)*(ptrs++);
            (*this)(x,y,1) = (T)*(ptrs++);
            (*this)(x,y,0) = (T)*(ptrs++);
            ++ptrs;
          }
          ptrs+=align_bytes;
        }
      } break;
      }
      if (dy<0) this->mirror('y');
      return *this;
    }

};

} // namespace cimg_library_suffixed

#endif


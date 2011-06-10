//******************************************************************************
//    Copyright (c) 2010, Christopher James Huff
//    All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//******************************************************************************

#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <stdint.h>

#include <png.h>
#include "simpleimage.h"


class PNG_FileInfo {
  public:
	png_uint_32 width, height;
	uint8_t pixelBytes;
	int depth;
	
  public:
	PNG_FileInfo():
		width(0), height(0), pixelBytes(4), depth(32)
	{}
	PNG_FileInfo(int w, int h, int d = 32):
		width(w), height(h), pixelBytes(d/8), depth(d)
	{}
	~PNG_FileInfo() {}
	
	bool Read(const std::string & filename, SimpleImage & image);
	bool Write(const std::string & filename, const SimpleImage & image);
};

inline bool PNG_FileInfo::Read(const std::string & filename, SimpleImage & image)
{
    FILE * fp = fopen(filename.c_str(), "rb");
    if(!fp) return false;
    
    uint8_t header[8];
    fread(header, 1, 8, fp);
    if(png_sig_cmp(header, 0, 8) != 0) {
        fclose(fp);
        std::cerr << "Not a PNG file!" << std::endl;
        return false;
    }
    //    png_structp png_ptr = png_create_read_struct_2
    //       (PNG_LIBPNG_VER_STRING, (png_voidp)user_error_ptr,
    //        user_error_fn, user_warning_fn, (png_voidp)
    //        user_mem_ptr, user_malloc_fn, user_free_fn);
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "Could not create PNG read struct" << std::endl;
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        std::cerr << "Could not create PNG info struct" << std::endl;
        return false;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        std::cerr << "Could not read PNG file" << std::endl;
        return false;
    }
    
    png_init_io(png_ptr, fp);
    
    png_set_sig_bytes(png_ptr, 8);
    
    png_read_info(png_ptr, info_ptr);
    
    int bit_depth, color_type, interlace_type, compression_type, filter_method;
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
       &bit_depth, &color_type, &interlace_type,
       &compression_type, &filter_method);
    
//    std::cout << "got IHDR" << std::endl;
//    std::cout << "width: " << width << std::endl;
//    std::cout << "height: " << height << std::endl;
//    std::cout << "color_type: " << color_type << std::endl;
    
//    png_set_packing(png_ptr);
    
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    
    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    
    pixelBytes = 4;
    depth = 32;
    
    png_read_update_info(png_ptr, info_ptr);
    
/*    switch(color_type) {
      case PNG_COLOR_TYPE_PALETTE: {
        png_color * palette;
        png_uint_32 num_palette;
        png_get_PLTE(png_ptr, info_ptr, palette, &num_palette);
      }
      break;
      case PNG_COLOR_TYPE_GRAY:
      case PNG_COLOR_TYPE_GRAY_ALPHA:
        cerr << "GRAY/GRAY+A not yet supported!" << endl;
      break;
      
      case PNG_COLOR_TYPE_RGB:
      case PNG_COLOR_TYPE_RGB_ALPHA:
        cerr << "RGB/RGBA not yet supported!" << endl;
      break;
      
      case PNG_COLOR_MASK_PALETTE:
      case PNG_COLOR_MASK_COLOR:
      case PNG_COLOR_MASK_ALPHA:
        cerr << "COLOR_MASK not yet supported!" << endl;
      break;
    }*/
    
    image.Realloc((int)width, (int)height, 4);
    
    // A rows array just for libpng, to correct the coordinate system
    std::vector<uint8_t *> rows(height);
    for(uint32_t y = 0; y < height; ++y)
        rows[y] = image.rows[height - y - 1];
    
    png_read_image(png_ptr, &rows[0]);
    png_read_end(png_ptr, info_ptr);
	
//    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
	return true;
}

inline bool PNG_FileInfo::Write(const std::string & filename, const SimpleImage & image)
{
	pixelBytes = image.pixelBytes;
	depth = image.pixelBytes*8;
	width = image.width;
	height = image.height;
    
    FILE * fp = fopen(filename.c_str(), "wb");
    if(!fp) return false;
    
    //png_create_write_struct_2()
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        fclose(fp);
        std::cerr << "Could not create PNG write struct" << std::endl;
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        std::cerr << "Could not create PNG info struct" << std::endl;
        return false;
    }
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        std::cerr << "PNG error" << std::endl;
        return false;
    }
    
    png_init_io(png_ptr, fp);
    
    //    PNG_FILTER_NONE  | PNG_FILTER_VALUE_NONE |
    //    PNG_FILTER_SUB   | PNG_FILTER_VALUE_SUB  |
    //    PNG_FILTER_UP    | PNG_FILTER_VALUE_UP   |
    //    PNG_FILTER_AVG   | PNG_FILTER_VALUE_AVG  |
    //    PNG_FILTER_PAETH | PNG_FILTER_VALUE_PAETH|
    //    PNG_ALL_FILTERS
    //png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
    
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    
    //PNG_COLOR_TYPE_GRAY       (bit depths 1, 2, 4, 8, 16)
    //PNG_COLOR_TYPE_GRAY_ALPHA (bit depths 8, 16)
    //PNG_COLOR_TYPE_PALETTE    (bit depths 1, 2, 4, 8)
    //PNG_COLOR_TYPE_RGB        (bit_depths 8, 16)
    //PNG_COLOR_TYPE_RGB_ALPHA  (bit_depths 8, 16)
    //
    // PNG_INTERLACE_NONE or PNG_INTERLACE_ADAM7
    png_set_IHDR(png_ptr, info_ptr, width, height,
       depth/pixelBytes, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    // png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
    
    
    // A rows array just for libpng, to correct the coordinate system
    std::vector<uint8_t *> rows(height);
    for(uint32_t y = 0; y < height; ++y)
        rows[y] = image.rows[height - y - 1];
    
    png_set_rows(png_ptr, info_ptr, &rows[0]);
    
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
	return true;
}

#endif // PNGIMAGE_H

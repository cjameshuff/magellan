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

#ifndef SIMPLEIMAGE_H
#define SIMPLEIMAGE_H

#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <algorithm>

template<typename T>
T min3(T a, T b, T c) {return std::min(std::min(a, b), c);}

template<typename T>
T max3(T a, T b, T c) {return std::max(std::max(a, b), c);}

// Simple image structure. Most functions assume 32-bpp RGBA color, but basic handling
// of arbitrary pixel sizes exists.
struct SimpleImage {
    // 1D pixels array, starting at lower left corner and going right and up,
    // rows array, starting at bottom row and going up.
	uint8_t * pixels;
	uint8_t ** rows;
	int32_t width, height;
	size_t pixelBytes;
	
	SimpleImage(): pixels(NULL), rows(NULL), width(0), height(0), pixelBytes(0) {}
    
	SimpleImage(const SimpleImage & rhs): pixels(NULL), rows(NULL), width(0), height(0), pixelBytes(0) {
        Realloc(rhs.width, rhs.height, rhs.pixelBytes);
        memcpy(pixels, rhs.pixels, width*height*rhs.pixelBytes);
    }
	
	SimpleImage(size_t w, size_t h, size_t pb = 4):
		 pixels(NULL), rows(NULL), width(0), height(0), pixelBytes(0)
	{Realloc(w, h, pb);}
	
	~SimpleImage() {
		if(pixels) delete[] pixels;
		if(rows) delete[] rows;
    }
    
    // Extract rectangular area of image as new image
    SimpleImage * Slice(int32_t x, int32_t y, int32_t w, int32_t h) const
    {
        SimpleImage * result = new SimpleImage(w, h, pixelBytes);
        result->Copy(*this, x, y, 0, 0, w, h);
        return result;
    }
    
    // Copy whole of src to part of dst
    void Blit(const SimpleImage & src, int32_t dstx, int32_t dsty, float light = 1.0f)
    {
        int32_t h = std::min(height - dsty, src.height);
        int32_t w = std::min(width - dstx, src.width);
        for(int32_t y = 0; y < h; ++y)
        for(int32_t x = 0; x < w; ++x) {
            uint8_t * d = pixels + ((dsty + y)*width + dstx + x)*pixelBytes;
            uint8_t * s = src.pixels + (y*src.width + x)*pixelBytes;
            float a = (float)s[3]/255.0f;
            float scl = light*a;
            d[0] = d[0]*(1.0 - a) + s[0]*scl;
            d[1] = d[1]*(1.0 - a) + s[1]*scl;
            d[2] = d[2]*(1.0 - a) + s[2]*scl;
            d[3] = 255;
        }
    }
    
    // Copy rectangular part of src to rectangular part of dst
    // Source and destination must be same depth!
    void Copy(const SimpleImage & src, int32_t srcx, int32_t srcy, int32_t dstx, int32_t dsty, int32_t w, int32_t h)
    {
        // truncate rect to portion within both images
        h = min3(h, height - dsty, src.height - srcy);
        w = min3(w, width - dstx, src.width - srcx);
        for(int32_t y = 0; y < h; ++y)
            memcpy(pixels + ((dsty + y)*width + dstx)*pixelBytes,
                src.pixels + ((srcy + y)*src.width + srcx)*pixelBytes,
                w*pixelBytes);
    }
	
	void Realloc(size_t w, size_t h, size_t pb) {
		width = w; height = h; pixelBytes = pb;
		if(pixels) delete[] pixels;
		if(rows) delete[] rows;
		pixels = new uint8_t[width*height*pixelBytes];
		rows = new uint8_t *[height];
		for(int32_t y = 0; y < height; ++y)
			rows[y] = pixels + width*y*pixelBytes;
	}
    
    // Reduce image size by half, averaging 2x2 pixel blocks into each destination pixel
    void Halve() {
        int32_t nw = width/2;
        int32_t nh = height/2;
        uint8_t * newPixels = new uint8_t[nw*nh*pixelBytes];
        for(int32_t y = 0; y < height - 1; y += 2)
        for(int32_t x = 0; x < width - 1; x += 2)
        {
            uint8_t * d = newPixels + ((y >> 1)*nw + (x >> 1))*pixelBytes;
            uint8_t * s = pixels + (y*width + x)*pixelBytes;
            d[0] = ((int)s[0] + s[pixelBytes] + s[width*pixelBytes] + s[width*pixelBytes + pixelBytes])/4;
            d[1] = ((int)s[1] + s[pixelBytes + 1] + s[width*pixelBytes + 1] + s[width*pixelBytes + pixelBytes + 1])/4;
            d[2] = ((int)s[2] + s[pixelBytes + 2] + s[width*pixelBytes + 2] + s[width*pixelBytes + pixelBytes + 2])/4;
            d[3] = ((int)s[3] + s[pixelBytes + 3] + s[width*pixelBytes + 3] + s[width*pixelBytes + pixelBytes + 3])/4;
        }
        delete[] pixels;
        delete[] rows;
        pixels = newPixels;
        width = nw; height = nh;
		rows = new uint8_t *[height];
		for(int32_t y = 0; y < height; ++y)
			rows[y] = pixels + width*y*pixelBytes;
    }
	
	void Clear() {memset(pixels, 0, width*height*pixelBytes);}
	
	void Clear(uint8_t r, uint8_t g, uint8_t b) {
		for(size_t p = 0, n = width*height*pixelBytes; p < n; p += pixelBytes) {
			pixels[p] = r;
			pixels[p+1] = g;
			pixels[p+2] = b;
		}
	}
	void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		for(size_t p = 0, n = width*height*pixelBytes; p < n; p += pixelBytes) {
			pixels[p] = r;
			pixels[p+1] = g;
			pixels[p+2] = b;
			pixels[p+3] = a;
		}
	}
    
    
	void Mul(float r, float g, float b, float a) {
		for(size_t p = 0, n = width*height*pixelBytes; p < n; p += pixelBytes) {
			pixels[p] = fminf(255.0f, (float)pixels[p]*r);
			pixels[p+1] = fminf(255.0f, (float)pixels[p+1]*g);
			pixels[p+2] = fminf(255.0f, (float)pixels[p+2]*b);
			pixels[p+3] = fminf(255.0f, (float)pixels[p+3]*a);
		}
	}
	
	//Make sure image depth supports the number of components you specify...no checking done!
	void SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) {
		rows[y][x*pixelBytes] = r;
		rows[y][x*pixelBytes+1] = g;
		rows[y][x*pixelBytes+2] = b;
	}
	void SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		rows[y][x*pixelBytes] = r;
		rows[y][x*pixelBytes+1] = g;
		rows[y][x*pixelBytes+2] = b;
		rows[y][x*pixelBytes+3] = a;
	}
	void BlendPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        float alpha = (float)a/255.0f;
		rows[y][x*pixelBytes]   = rows[y][x*pixelBytes]*(1.0 - alpha) + r*alpha;
		rows[y][x*pixelBytes+1] = rows[y][x*pixelBytes+1]*(1.0 - alpha) + g*alpha;
		rows[y][x*pixelBytes+2] = rows[y][x*pixelBytes+2]*(1.0 - alpha) + b*alpha;
//		rows[y][x*pixelBytes+3] = 255;// TODO: something sensible...
	}
	
	uint8_t * GetPixel(size_t x, size_t y) {return rows[y] + x*pixelBytes;}
	const uint8_t * GetPixel(size_t x, size_t y) const {return rows[y] + x*pixelBytes;}
};


//******************************************************************************
#endif // SIMPLEIMAGE_H

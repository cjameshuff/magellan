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

#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <vector>

template<typename T>
class Array2D {
  private:
    size_t width;
    size_t height;
    std::vector<T> data;
    
  public:
    Array2D(): width(0), height(0) {}
    Array2D(size_t x, size_t y, const T & defVal = T()): width(x), height(y), data(x*y, defVal) {}
    
    void Resize(size_t x, size_t y, const T & defVal = T()) {
        data.resize(x*y, defVal);
        width = x;
        height = y;
    }
    
    size_t Width() const {return width;}
    size_t Height() const {return height;}
    
    T & operator()(size_t x, size_t y) {return data[x*height + y];}
    const T & operator()(size_t x, size_t y) const {return data[x*height + y];}
    
    T * operator[](size_t x) {return &data[x*height];}
    const T * operator[](size_t x) const {return &data[x*height];}
};


#endif // ARRAY2D_H

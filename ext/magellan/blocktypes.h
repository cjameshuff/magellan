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

#ifndef BLOCKTYPES_H
#define BLOCKTYPES_H

#include "pngimage.h"
#include "blockdefs.h"

enum {
    kBS_Top,
    kBS_Bottom,
    kBS_North,
    kBS_South,
    kBS_East,
    kBS_West,
    kBS_Left = kBS_West, // Mapping of left/right/front/back coordinates:
    kBS_Back = kBS_North,// think of a map on a workbench, north side away from you.
    kBS_Front = kBS_South,
    kBS_Right = kBS_East
};

// TODO: block sides, oddball entities like doors

// Textures are stored at precalculated power-of-2 scales. All textures are
// assumed to be of equal size at each scale.
// Scale decreases with increasing index. Index 0 is largest scale, index 2 is
// halved, etc.
const int kNumTextureScales = 4;
const int kTextureScaleMap[] = {// map from pixel scales to power of 2 indices
    3,
    3,
    3, 2,
    2, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0,
    0
};
struct ScaledTexture {
    SimpleImage * texture[kNumTextureScales];
    
    ScaledTexture();
    
    void Compute(SimpleImage * base);
    SimpleImage * operator[](size_t idx) {return texture[idx];}
};


struct BlockType {
    bool isOpaque;
    ScaledTexture texture;
    uint8_t color[4];
    BlockType():
        isOpaque(true) {}
    
//    virtual ScaledTexture * GetTexture(const MC_Block & block, int lightval, );
};

// misnamed, only has texture information now
extern BlockType blockTypes[];


void LoadTextures();

#endif // BLOCKTYPES_H

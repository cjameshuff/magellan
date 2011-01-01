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

#ifndef MAGELLAN_H
#define MAGELLAN_H

#include "nbt.h"
#include "nbtlua.h"
#include "mc.h"
#include "array2d.h"

#include "pngimage.h"

#include <string>

enum {
    kLightingAltitude,
    kLightingAltitudeGray,
    kLightingDay,
    kLightingNight,
    kLightingMorning,// halflight with fog
    kLightingEvening// halflight without fog
};

struct MagellanOptions {
    std::string outputFile;
//    std::string mcWorldName;
//    
//    int worldNum;
//    int dimension;
    
//    int lighting;
    
    int displayMode;
    bool layers;
    
    int yMin, yMax;
    int xMin, xMax;
    int zMin, zMax;
    int xMinBlock, zMinBlock;
    int xMaxBlock, zMaxBlock;
    int scale;
    int peel;
    
    int xSize, zSize;
    
    MagellanOptions():
        outputFile("output.png"),
//        worldNum(1), dimension(0),
        displayMode(kLightingAltitude),
        yMin(1), yMax(127),
        xMin(INT_MIN), xMax(INT_MAX),
        zMin(INT_MIN), zMax(INT_MAX),
        scale(2), peel(0)
    {}
};

struct MC_Stats {
    int yMin, yMax;
    size_t numBlocks;
    size_t blockCounts[kNumBlockTypes];
    size_t blocksDrawn;
};

//******************************************************************************

extern MC_Stats stats;
extern MC_World world;

//******************************************************************************

void MGL_Init(lua_State * lua);

void RenderMap(const MagellanOptions & opts);


void WriteImage(SimpleImage & outputImage, const std::string & path);

void ComputeStats(const MagellanOptions & opts);

void RenderBlock(SimpleImage & outputImage, int scale, uint8_t type, int x, int y, int z, float light);

void DrawTop(SimpleImage & outputImage, const MagellanOptions & opts);
//void DrawLayer(SimpleImage & outputImage, const MagellanOptions & opts, int by);

//******************************************************************************
#endif // MAGELLAN_H

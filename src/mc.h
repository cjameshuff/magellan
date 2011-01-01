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

// http://minecraftwiki.net/wiki/Alpha_Level_Format
// http://minecraftwiki.net/wiki/Map_Format_(NBT)

#ifndef ALPHA_H
#define ALPHA_H

#include <stdint.h>

#include <vector>

#include "nbt.h"
#include "pngimage.h"
#include "array2d.h"

#include "blocktypes.h"


/*struct ALpha_Inventory {
    int16_t id;
    int16_t damage;
    int8_t count;
    int8_t slot;
};

struct ALpha_LevelData {

};*/

struct MC_Block {
    uint8_t type;
    uint8_t data;
    uint8_t skylight;
    uint8_t blocklight;
};

// This class provides a faster interface to a NBT file chunk, and that chunk
// must remain in existence. Do not delete the chunk!
class MC_Chunk {
    std::vector<uint8_t> * blocks;
    std::vector<uint8_t> * data;
    std::vector<uint8_t> * skylight;
    std::vector<uint8_t> * blocklight;
    std::vector<uint8_t> * heightmap; // (16x16, ordered ZX)
    NBT_TagCompound * entities;
    int64_t lastupdate;
    int32_t xPos;
    int32_t zPos;
    int8_t populated;
  public:
    MC_Chunk(NBT_TagCompound * chunkTag);
    ~MC_Chunk() {}
    
static int32_t GetIdx(int32_t x, int32_t y, int32_t z) {return y + (z + x*16)*128;}
// Compute indices of neighboring blocks
static int32_t GetIdxD(int32_t idx) {return idx - 1;}
static int32_t GetIdxU(int32_t idx) {return idx + 1;}
//static int32_t GetIdxN(int32_t idx) {return idx - ;}
//static int32_t GetIdxS(int32_t idx) {return idx + ;}
//static int32_t GetIdxE(int32_t idx) {return idx - ;}
//static int32_t GetIdxW(int32_t idx) {return idx + ;}
static bool IdxGood(int32_t idx) {return idx > 0 && idx < (16*16*128);}
    
    void GetBlock(MC_Block & block, int32_t x, int32_t y, int32_t z) const {
        size_t idx = GetIdx(x, y, z);
        size_t halfidx = idx >> 1;
        block.type = (*blocks)[idx];
        block.data = (idx & 0x01)? ((*data)[halfidx] >> 4) : ((*data)[halfidx] & 0x0F);
        block.skylight = (idx & 0x01)? ((*skylight)[halfidx] >> 4) : ((*skylight)[halfidx] & 0x0F);
        block.blocklight = (idx & 0x01)? ((*blocklight)[halfidx] >> 4) : ((*blocklight)[halfidx] & 0x0F);
    }
    
    
    uint8_t GetType(size_t idx) const {return (*blocks)[idx];}
    uint8_t GetData(size_t idx) const {
        size_t halfidx = idx >> 1;
        return (idx & 0x01)? ((*data)[halfidx] >> 4) : ((*data)[halfidx] & 0x0F);
    }
    uint8_t GetSkylight(size_t idx) const {
        size_t halfidx = idx >> 1;
        return (idx & 0x01)? ((*skylight)[halfidx] >> 4) : ((*skylight)[halfidx] & 0x0F);
    }
    uint8_t GetBlocklight(size_t idx) const {
        size_t halfidx = idx >> 1;
        return (idx & 0x01)? ((*blocklight)[halfidx] >> 4) : ((*blocklight)[halfidx] & 0x0F);
    }
};

struct MC_World {
    std::string mcWorldPath;
    std::string mcWorldName;
    // Map info:
    int xChunkMin, xChunkMax;
    int zChunkMin, zChunkMax;
    int xSize, zSize;
    std::vector<NBT_TagCompound *> chunkTags;
    Array2D<MC_Chunk *> chunks;
    
    MC_World():
        xChunkMin(INT_MAX), xChunkMax(INT_MIN),
        zChunkMin(INT_MAX), zChunkMax(INT_MIN)
    {}
    
    ~MC_World() {
        for(size_t j = 0; j < chunkTags.size(); ++j)
            delete chunkTags[j];
    }
    void Load(const std::string & wPath, const std::string & wName, bool ignoreEmpties = false);
    
    // Does not do bounds checks!
    MC_Chunk * ChunkAt(int x, int z) {return chunks[x - xChunkMin][z - zChunkMin];}
};

void LoadTextures();

//******************************************************************************
#endif // ALPHA_H

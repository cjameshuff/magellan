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

#include "mc.h"

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "fileutils.h"

#include "misc.h"

using namespace std;

//******************************************************************************
// Statics and globals
//******************************************************************************

MC_Chunk::MC_Chunk(NBT_TagCompound * chunk)
{
    blocks = &(chunk->GetTag<NBT_TagByteArray>("Blocks")->value);
    data = &(chunk->GetTag<NBT_TagByteArray>("Data")->value);
    skylight = &(chunk->GetTag<NBT_TagByteArray>("SkyLight")->value);
    blocklight = &(chunk->GetTag<NBT_TagByteArray>("BlockLight")->value);
    heightmap = &(chunk->GetTag<NBT_TagByteArray>("HeightMap")->value);
    
    lastupdate = chunk->GetTag<NBT_TagLong>("LastUpdate")->value;
    
    xPos = chunk->GetTag<NBT_TagInt>("xPos")->value;
    zPos = chunk->GetTag<NBT_TagInt>("zPos")->value;
    
    populated = chunk->GetTag<NBT_TagByte>("TerrainPopulated")->value;
}

//******************************************************************************
// Names of directories containing chunks are the contained chunk coordinates mod 64,
// in base36. Top level directories are x % 64, and each contain up to 64 folders
// similarly named for z % 64.
// Directory names:
static const char * chunkDirs[72] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h",// 0-17
    "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",// 18-35
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "1g", "1h",// 36-53
    "1i", "1j", "1k", "1l", "1m", "1n", "1o", "1p", "1q", "1r", "1s", "1t", "1u", "1v", "1w", "1x", "1y", "1z" // 54-72
};


void MC_World::Load(const std::string & wPath, const std::string & wName, bool ignoreEmpties)
{
    mcWorldPath = wPath;
    mcWorldName = wName;
    
    cout << "Searching for files..." << endl;
    vector<string> filepaths;
    for(int x = 0; x < 64; ++x)
    for(int z = 0; z < 64; ++z)
    {
        string dirPath = mcWorldPath + "/" + wName + "/" + chunkDirs[x] + "/" + chunkDirs[z];
        GetFilePaths(dirPath, filepaths);
    }
    
    // Load chunks and compute world size
    cout << "Loading..." << endl;
    for(size_t j = 0; j < filepaths.size(); ++j)
    {
        NBT_TagCompound * nbt = LoadNBT_File(filepaths[j]);
        if(!nbt) continue;
        
//        cout << "Loading " << filepaths[j] << endl;
        
        chunkTags.push_back(nbt);
        NBT_TagCompound * level = nbt->GetTag<NBT_TagCompound>("Level");
        
        NBT_TagInt * xPosTag = level->GetTag<NBT_TagInt>("xPos");
        NBT_TagInt * zPosTag = level->GetTag<NBT_TagInt>("zPos");
        
        if(ignoreEmpties)
        {
            bool hasContent = false;
            MC_Chunk chunk(level);
            MC_Block block;
            for(int x = 0; x < 16 && !hasContent; ++x)
            for(int y = 0; y < 128 && !hasContent; ++y)
            for(int z = 0; z < 16 && !hasContent; ++z) {
                chunk.GetBlock(block, x, y, z);
                if(block.type != kBT_Air)
                    hasContent = true;
            }
            if(!hasContent) {
                cout << "Discarded empty world chunk at " << xPosTag->value << ", " << zPosTag->value << endl;
                delete nbt;
                continue;
            }
        }
        
        xChunkMin = min(xChunkMin, xPosTag->value);
        xChunkMax = max(xChunkMax, xPosTag->value);
        zChunkMin = min(zChunkMin, zPosTag->value);
        zChunkMax = max(zChunkMax, zPosTag->value);
    }
    cout << "Chunks loaded" << endl;
    
    // Place chunks in terrain grid
    xSize = xChunkMax - xChunkMin + 1;
    zSize = zChunkMax - zChunkMin + 1;
    chunks.Resize(xSize, zSize, NULL);
    for(size_t j = 0; j < chunkTags.size(); ++j)
    {
        NBT_TagCompound * level = chunkTags[j]->GetTag<NBT_TagCompound>("Level");
        NBT_TagInt * xPosTag = level->GetTag<NBT_TagInt>("xPos");
        NBT_TagInt * zPosTag = level->GetTag<NBT_TagInt>("zPos");
        int x = xPosTag->value - xChunkMin;
        int z = zPosTag->value - zChunkMin;
        chunks[x][z] = new MC_Chunk(level);
    }
}

//******************************************************************************

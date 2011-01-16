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



//******************************************************************************
// MC_Chunk
//******************************************************************************

MC_Chunk::MC_Chunk(NBT_TagCompound * cNBT)
{
    chunkNBT = cNBT;
    SetupFromNBT();
}

MC_Chunk::MC_Chunk(int32_t x, int32_t z)
{
    // Create a NBT structure for this chunk, rather than use an existing one.
    chunkNBT = new NBT_TagCompound();
    NBT_TagCompound * level = new NBT_TagCompound("Level");
    chunkNBT->AddTag(level);
    
    NBT_TagByteArray * ba = new NBT_TagByteArray("Blocks");
    ba->value.resize(32768);
    level->AddTag(ba);
    
    ba = new NBT_TagByteArray("Data");
    ba->value.resize(16384);
    level->AddTag(ba);
    
    ba = new NBT_TagByteArray("SkyLight");
    ba->value.resize(16384);
    level->AddTag(ba);
    
    ba = new NBT_TagByteArray("BlockLight");
    ba->value.resize(16384);
    level->AddTag(ba);
    
    ba = new NBT_TagByteArray("HeightMap");
    ba->value.resize(256);
    level->AddTag(ba);
    
    entities = new NBT_TagList("Entities", kNBT_TAG_Compound);
    level->AddTag(entities);
    tileEntities = new NBT_TagList("TileEntities", kNBT_TAG_Compound);
    level->AddTag(tileEntities);
    
    lastupdate = 0;
    level->AddTag(new NBT_TagLong("LastUpdate", lastupdate));
    
    xPos = x; zPos = z;
    level->AddTag(new NBT_TagInt("xPos", xPos));
    level->AddTag(new NBT_TagInt("zPos", zPos));
    
    populated = true;
    level->AddTag(new NBT_TagByte("TerrainPopulated", populated));
    
    SetupFromNBT();
}

void MC_Chunk::SetupFromNBT()
{
    NBT_TagCompound * level = chunkNBT->GetTag<NBT_TagCompound>("Level");
    
    blocks = &(level->GetTag<NBT_TagByteArray>("Blocks")->value);
    data = &(level->GetTag<NBT_TagByteArray>("Data")->value);
    skylight = &(level->GetTag<NBT_TagByteArray>("SkyLight")->value);
    blocklight = &(level->GetTag<NBT_TagByteArray>("BlockLight")->value);
    heightmap = &(level->GetTag<NBT_TagByteArray>("HeightMap")->value);
    
    entities = level->GetTag<NBT_TagList>("Entities");
    tileEntities = level->GetTag<NBT_TagList>("TileEntities");
    
    lastupdate = level->GetTag<NBT_TagLong>("LastUpdate")->value;
    
    xPos = level->GetTag<NBT_TagInt>("xPos")->value;
    zPos = level->GetTag<NBT_TagInt>("zPos")->value;
    
    populated = level->GetTag<NBT_TagByte>("TerrainPopulated")->value;
}


//******************************************************************************
// MC_World
//******************************************************************************
void MC_World::Load(const std::string & wPath, const std::string & wName)
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
//        cout << "Loading " << filepaths[j] << endl;
        NBT_TagCompound * nbt = LoadNBT_File(filepaths[j]);
        if(nbt)
            AddChunk(new MC_Chunk(nbt));
    }
    cout << "Chunks loaded" << endl;
    
    RebuildGrid();
}

void MC_World::AddChunk(MC_Chunk * chunk)
{
    allChunks.push_back(chunk);
    
    xChunkMin = min(xChunkMin, chunk->xPos);
    xChunkMax = max(xChunkMax, chunk->xPos);
    zChunkMin = min(zChunkMin, chunk->zPos);
    zChunkMax = max(zChunkMax, chunk->zPos);
    xSize = xChunkMax - xChunkMin + 1;
    zSize = zChunkMax - zChunkMin + 1;
}

void MC_World::RebuildGrid()
{
    // Place chunks in terrain grid
    chunks.Resize(xSize, zSize, NULL);
    for(size_t j = 0; j < allChunks.size(); ++j)
    {
        // Chunk grid is indexed from 0, must offset by lowest coordinates
        int x = allChunks[j]->xPos - xChunkMin;
        int z = allChunks[j]->zPos - zChunkMin;
        chunks[x][z] = allChunks[j];
    }
}


MC_World::~MC_World() {
    for(size_t j = 0; j < allChunks.size(); ++j)
        delete allChunks[j];
}

//******************************************************************************
// MC_BlockBuffer
//******************************************************************************

MC_BlockBuffer::MC_BlockBuffer(int xS, int yS, int zS):
    xSize(xS), ySize(yS), zSize(zS)
{
    
}
//******************************************************************************

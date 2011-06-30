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
static const char * b36digits[36] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h",// 0-17
    "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"// 18-35
};

static inline std::string ToBase36(int32_t val) {
    std::string str;
    if(val < 0) {
        str += '-';
        val = -val;
    }
    
    // Find largest power of 36 that is not larger than abs(val)
    int pwr = 1;// 36^0
    while(pwr*36 <= val)
        pwr *= 36;
    
    while(val >= 36) {
        str += b36digits[val/pwr];
        val = val%pwr;
        pwr /= 36;
    }
    str += b36digits[val];
    return str;
}


//******************************************************************************
// MC_Chunk
//******************************************************************************

MC_Chunk::MC_Chunk(NBT_TagCompound * cNBT): dirty(false)
{
    chunkNBT = cNBT;
    SetupFromNBT();
}

MC_Chunk::MC_Chunk(int32_t x, int32_t z): dirty(false)
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


MC_World::MC_World():
    xChunkMin(INT_MAX), xChunkMax(INT_MIN),
    zChunkMin(INT_MAX), zChunkMax(INT_MIN)
{}

MC_World::~MC_World() {
    for(size_t j = 0; j < allChunks.size(); ++j)
        delete allChunks[j];
}


void GenerateLevelDat(const std::string & ldPath, int spawnX, int spawnY, int spawnZ)
{
    NBT_TagCompound * leveldatNBT = new NBT_TagCompound();
    NBT_TagCompound * data = new NBT_TagCompound("Data");
    leveldatNBT->AddTag(data);
    
    data->AddTag(new NBT_TagLong("Time", 0));
    data->AddTag(new NBT_TagLong("LastPlayed", MC_Timestamp()));
    
    NBT_TagCompound * player = new NBT_TagCompound("Player");
    data->AddTag(player);
    
    player->AddTag(new NBT_TagList("Inventory", kNBT_TAG_Compound));
    player->AddTag(new NBT_TagInt("Score", 0));
    player->AddTag(new NBT_TagLong("Dimension", 0));
    
    data->AddTag(new NBT_TagInt("SpawnX", spawnX));
    data->AddTag(new NBT_TagInt("SpawnY", spawnY));
    data->AddTag(new NBT_TagInt("SpawnZ", spawnZ));
    
    data->AddTag(new NBT_TagLong("SizeOnDisk", 0));// TODO: compute this!
    data->AddTag(new NBT_TagLong("RandomSeed", 0));
    
    WriteNBT_File(leveldatNBT, ldPath);
    delete leveldatNBT;
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


// Get a block, returns "air" block if chunk doesn't exist for location
MC_Block MC_World::GetBlock(int32_t x, int32_t y, int32_t z) const
{
    MC_Block block = {kBT_Air, 0x0, 0x0, 0x0};
    if(y < 0 || y > 127)
        return block;
    
    // need to round negative numbers down, not toward zero
    int cx = x & 15;//(x < 0)? ((x/16) - 1) : x/16;
    int cz = z & 15;//(z < 0)? ((z/16) - 1) : z/16;
    
    const MC_Chunk * chunk = ChunkAt(cx, cz);
    if(chunk) {
        chunk->GetBlock(block, x - cx*16, y, z - cz*16);
    }
    return block;
}

// Set a block, creating chunk if necessary.
// TODO: versions of SetBlock()/GetBlock() that don't pack/unpack lighting and data
void MC_World::SetBlock(const MC_Block & block, int32_t x, int32_t y, int32_t z)
{
    if(y < 0 || y > 127)
        return;
    
    // need to round negative numbers down, not toward zero
    int cx = (x < 0)? ((x/16) - 1) : x/16;
    int cz = (z < 0)? ((z/16) - 1) : z/16;
    
    MC_Chunk * chunk = ChunkAt(cx, cz);
    if(chunk == NULL) {
        cout << "Chunk for block does not exist!" << endl;
        return;// FIXME: No tag "Level" in ""!
//        cout << "Chunk for block does not exist, creating one..." << endl;
//        chunk = new MC_Chunk(cx, cz);
//        AddChunk(chunk);
//        RebuildGrid();
    }
    chunk->SetBlock(block, x - cx*16, y, z - cz*16);
}

void MC_World::CalcHeightmap()
{
    
}


void MC_World::SetHeightmap(int x, int z, int height)
{
    if(height < 0 || height > 127)
        return;
    
    // need to round negative numbers down, not toward zero
    int cx = (x < 0)? ((x/16) - 1) : x/16;
    int cz = (z < 0)? ((z/16) - 1) : z/16;
    
    MC_Chunk * chunk = ChunkAt(cx, cz);
    if(chunk == NULL) {
        cout << "Chunk for block does not exist!" << endl;
        return;
//        chunk = new MC_Chunk(cx, cz);
//        AddChunk(chunk);
//        RebuildGrid();
    }
    chunk->SetHeightmap(x - cx*16, z - cz*16, height);
}


//******************************************************************************
// MC_BlockBuffer
//******************************************************************************

MC_BlockBuffer::MC_BlockBuffer(int xS, int yS, int zS):
    xSize(xS), ySize(yS), zSize(zS)
{
    
}
//******************************************************************************

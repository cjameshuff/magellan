//******************************************************************************
//    Copyright (c) 2011, Christopher James Huff
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

#include "nbtio.h"
#include "nbt.h"

#include <algorithm>
#include <iomanip>

using namespace std;


static const size_t COMP_CHUNK_SIZE =  64*1024;
static const size_t DECOMP_CHUNK_SIZE = 128*1024;


// sort in ascending order by start
bool sortbystart(const RegionBlock & a, const RegionBlock & b) {return a.start < b.start;}
// sort in ascending order by size
bool sortbysize(const RegionBlock & a, const RegionBlock & b) {return a.size < b.size;}



NBT_Region_IO::NBT_Region_IO():
    regFile(NULL),
    fileSize(0),
    decompBfr(NULL)
{
}


int NBT_Region_IO::Open(const std::string & fpath)
{
    if(regFile)
        fclose(regFile);
    
    fileSize = 0;
    endUsedSectors = 0;
    regFile = fopen(fpath.c_str(), "r+b");
    if(!regFile) {
        std::cerr << "Could not open \"" << fpath << "\"" << std::endl;
        return -1;
    }
    return ReadRegionTOC();
}


NBT_Region_IO::~NBT_Region_IO() {
    if(regFile)
        fclose(regFile);
    if(decompBfr)
        delete[] decompBfr;
}


void NBT_Region_IO::PrintStats(std::ostream & ostrm)
{
    vector<RegionBlock>::iterator j;
    int smallestF = (int)fileSize;
    int largestF = 0;
    int totalF = 0;
    int smallestU = (int)fileSize;
    int largestU = 0;
    int totalU = 0;
    // Note: chunk blocks may be consecutive, with no free space between them
    int ctr = 0;
    for(int j = 0; j < 1024; ++j) {
        totalU += chunkBlocks[j].size;
        smallestU = min(smallestU, chunkBlocks[j].size);
        largestU = max(largestU, chunkBlocks[j].size);
        // ostrm << ctr++ << ": " << j->start << ", " << j->size << endl;
    }
    for(j = freeBlocks.begin(); j != freeBlocks.end(); ++j) {
        totalF += j->size;
        smallestF = min(smallestF, j->size);
        largestF = max(largestF, j->size);
    }
    
    ostrm << endl;
    ostrm << "Used blocks: 1024" << endl;
    ostrm << "Smallest used block (sectors): " << smallestU << endl;
    ostrm << "Largest used block (sectors): " << largestU << endl;
    ostrm << "Mean used block size (sectors): " << fixed << setprecision(2) << (double)totalU/1024 << endl;
    ostrm << "All used blocks (sectors): " << totalU << endl;
    ostrm << endl;
    ostrm << "Free blocks: " << freeBlocks.size() << endl;
    if(freeBlocks.size() > 0)
    {
        ostrm << "Smallest free block (sectors): " << smallestF << endl;
        ostrm << "Largest free block (sectors): " << largestF << endl;
        ostrm << "Mean free block size (sectors): " << fixed << setprecision(2) << (double)totalF/freeBlocks.size() << endl;
        ostrm << "All free blocks (sectors): " << totalF << endl;
    }
    ostrm << endl;
    ostrm << (double)totalF*4096.0/fileSize*100.0 << "% of file space unused"  << endl;
}


int NBT_Region_IO::ReadRegionTOC()
{
    // Find file size
    fseek(regFile, 0, SEEK_END);
    fileSize = ftell(regFile);
    if(fileSize <= 8192) {
        std::cerr << "Input file too short" << std::endl;
        fclose(regFile);
        return -1;
    }
    
    // Seek to location info
    fseek(regFile, 0, SEEK_SET);
    uint8_t buf[4096];
    fread(buf, 4096, 1, regFile);
    
    int empty = 0;
    for(int j = 0, i = 0; j < 1024; ++j, i += 4) {
        // Sectors are 4096 bytes.
        // size is size of chunk in sectors
        // start is offset to start of chunk from start of file, in sectors.
        chunkBlocks[j].start = (buf[i] << 16) | (buf[i + 1] << 8) | buf[i + 2];
        chunkBlocks[j].size = buf[i + 3];
        if(chunkBlocks[j].start == 0)
            ++empty;
    }
    cout << empty << " chunks are empty" << endl;
    fread(buf, 4096, 1, regFile);
    for(int j = 0; j < 1024; ++j)
        chunkTimestamps[j] = (buf[j] << 24) | (buf[j + 1] << 16) | (buf[j + 2] << 8) | buf[j + 3];
    
    // build list of contiguous blocks of free sectors
    // There will be at most 1024 used blocks, and at most 1024 free blocks...
    // one at the start, one between each pair of used blocks. There may be free
    // sectors following the last used block, but that block is effectively of
    // unlimited size, so is ignored unless all other blocks are too small.
    
    // first, build list of used blocks, sorted by size.
    /*std::vector<RegionBlock> chunkBlocksInOrder = chunkBlocks;
    sort(chunkBlocksInOrder.begin(), chunkBlocksInOrder.end(), sortbystart);
    
    freeBlocks.clear();
    make_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
    // pop_heap(freeBlocks.begin(), freeBlocks.end()); freeBlocks.pop_back()
    // freeBlocks.push_back(); push_heap(freeBlocks.begin(), freeBlocks.end())
    // sort_heap(freeBlocks.begin(), freeBlocks.end())
    vector<RegionBlock>::iterator j;
    int crsr = 2;
    for(j = chunkBlocksInOrder.begin(); j != chunkBlocksInOrder.end(); ++j)
    {
        if(j->start == 0 || j->size == 0)
            continue; // skip empty chunk
        
        if(j->start - crsr > 0) {
            // free space exists between start of this used block and end of
            // previous used block
            freeBlocks.push_back(RegionBlock(crsr, j->start - crsr));
            push_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
        }
        crsr = j->start + j->size;
    }
    
    endUsedSectors = chunkBlocksInOrder.back().start + chunkBlocksInOrder.back().size;*/
    
    return 0;
}

void NBT_Region_IO::UpdateTOC(size_t chunkIdx, const RegionBlock & chunkBlock)
{
    uint8_t buf[4];
    
    RegionBlock oldBlock = chunkBlocks[chunkIdx];
    chunkBlocks[chunkIdx] = chunkBlock;
    
    // Old block of sectors used by chunk is now free for reuse
    // Check for contiguous free blocks and combine them
    freeBlocks.push_back(oldBlock);
    for(size_t j = 0; j < freeBlocks.size(); ++j)
    {
        if((freeBlocks[j].start + freeBlocks[j].size) == oldBlock.start)
        {
            freeBlocks[j].size += oldBlock.size;
            make_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
            oldBlock.start = 0;
            break;
        }
        else if((oldBlock.start + oldBlock.size) == freeBlocks[j].start)
        {
            freeBlocks[j].start = oldBlock.start;
            make_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
            oldBlock.start = 0;
            break;
        }
    }
    if(oldBlock.start)// did not merge with existing block, insert
        push_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
    
    // Recompute endUsedSectors
    endUsedSectors = 0;
    for(int j = 0; j < 1024; ++j) {
        if((chunkBlocks[j].start + chunkBlocks[j].size) > endUsedSectors)
            endUsedSectors = chunkBlocks[j].start + chunkBlocks[j].size;
    }
    
    buf[0] = ((chunkBlock.start >> 16) & 0xFF);
    buf[1] = ((chunkBlock.start >> 8) & 0xFF);
    buf[2] = (chunkBlock.start & 0xFF);
    buf[3] = chunkBlock.size;
    fseek(regFile, 4*chunkIdx, SEEK_SET);
    fwrite(buf, 4, 1, regFile);
    
    // Update timestamp
    int32_t timestamp = (int32_t)time(NULL);
    chunkTimestamps[chunkIdx] = timestamp;
    
    buf[0] = ((timestamp >> 24) & 0xFF);
    buf[1] = ((timestamp >> 16) & 0xFF);
    buf[2] = ((timestamp >> 8) & 0xFF);
    buf[3] = (timestamp & 0xFF);
    fseek(regFile, 8*chunkIdx, SEEK_SET);
    fwrite(buf, 4, 1, regFile);
}


int NBT_Region_IO::WriteChunk(int cx, int cz)
{
    chunkX = cx; chunkZ = cz;
    // Compress the data and determine the compressed size.
    // Find an appropriate location for chunk...possible algorithms:
    // A: First contiguous free area of sufficient size, else append
    // B: Largest contiguous free area, if of sufficient size, else append
    // C: Contiguous area of sufficient size that is adjacent to the longest
    //    run of in-use sectors. (optimize for reading)
    // D: Contiguous area of sufficient size that is not adjacent to any
    //    in-use sectors. (Allows for later growth without moving elsewhere...
    //    which requires overwriting old chunk, slightly less safe.)
    // Perhaps perform free space defragmenting/optmization tasks as well.
    
    if(!decompBfr)
    {
        std::cerr << "No chunk to write!" << std::endl;
        return -1;
    }
    
    uint8_t compBfr[COMP_CHUNK_SIZE];
    
    // decompress chunk into decompBfr
    int status;
    z_stream strm;
    strm.zalloc = (alloc_func)NULL;
    strm.zfree = (free_func)NULL;
    strm.opaque = NULL;
    
    strm.next_in = decompBfr;
    strm.next_out = compBfr;
    strm.avail_out = COMP_CHUNK_SIZE;
    strm.avail_in = (int)chunkBytes;
    
    int compressionLevel = 6;
    deflateInit(&strm, compressionLevel);
    status = deflate(&strm, Z_FINISH);
    deflateEnd(&strm);
    
    if(status != Z_STREAM_END) {
        std::cerr << "Error while compressing" << std::endl;
        return -1;
    }
    
    int compChunkBytes = strm.avail_out;
    int compChunkSectors = (compChunkBytes + 5)/4096;// (compressed data + 5 byte header)/sector size
    
    // If there's a free block with sufficient size, use it.
    // Else write at end of file.
    RegionBlock freeBlock(0, 0);
    if(freeBlocks.size() > 0)
    {
        pop_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
        
        if(freeBlocks.back().size >= compChunkSectors)
        {
            freeBlock = freeBlocks.back();
            freeBlocks.pop_back();
            if(freeBlock.size > compChunkSectors)
            {
                // free block larger than needed, split and push remainder back on the heap
                freeBlocks.push_back(RegionBlock(freeBlock.start + compChunkSectors, freeBlock.size - compChunkSectors));
                push_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
                freeBlock.size = compChunkSectors;
            }
        }
        else
        {
            // largest block unusably small, push back onto heap
            push_heap(freeBlocks.begin(), freeBlocks.end(), sortbysize);
        }
    }
    
    if(freeBlock.start == 0)
    {
        // Couldn't allocate space from free blocks, allocate new space
        freeBlock.start = endUsedSectors;
        freeBlock.size = compChunkSectors;
    }
    
    fseek(regFile, 4096*freeBlock.start, SEEK_SET);
    
    // The size includes the compression method byte, but not the size field itself.
    uint8_t buf[5];
    size_t chunkSize = compChunkBytes + 1;
    buf[0] = ((chunkSize >> 24) & 0xFF);
    buf[1] = ((chunkSize >> 16) & 0xFF);
    buf[2] = ((chunkSize >> 8) & 0xFF);
    buf[3] = (chunkSize & 0xFF);
    buf[4] = 2;// compression method 2
    
    // Write chunk and update TOC, in that order. If chunk write fails, old
    // chunk is still intact.
    fwrite(buf, 5, 1, regFile);
    fwrite(compBfr, compChunkBytes, 1, regFile);
    
    UpdateTOC(ChunkIdx(chunkX, chunkZ), freeBlock);
    
    rwPtr = 0;
    chunkBytes = 0;
    
    return 0;
}


int NBT_Region_IO::ReadChunk(int cx, int cz)
{
    chunkX = cx; chunkZ = cz;
    size_t chunkIdx = ChunkIdx(chunkX, chunkZ);
    int offset = chunkBlocks[chunkIdx].start;
    size_t numSectors = chunkBlocks[chunkIdx].size;
    
    if(offset == 0 || numSectors == 0) {
        std::cerr << "Chunk " << cx << ", " << cz << " is empty." << std::endl;
        return -1;
    }
    
    fseek(regFile, 4096*offset, SEEK_SET);
    uint8_t buf[5];
    fread(buf, 5, 1, regFile);
    
    // Compressed chunk size in bytes
    // The size includes the compression method byte (buf[4]), but not the size field itself.
    size_t compChunkBytes = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if(compChunkBytes > numSectors*4096 || compChunkBytes > COMP_CHUNK_SIZE) {
        std::cerr << "Bad chunk size: " << compChunkBytes << std::endl;
        std::cerr << "offset: " << offset << std::endl;
        std::cerr << "numSectors: " << numSectors << std::endl;
        std::cerr << "chunkIdx: " << chunkIdx << std::endl;
        return -1;
    }
    int compMthd = buf[4];
    if(compMthd != 2) {
        std::cerr << "Only compression method 2 supported at this time" << std::endl;
        return -1;
    }
    
    if(!decompBfr)
        decompBfr = new uint8_t[DECOMP_CHUNK_SIZE];
    
    uint8_t compBfr[COMP_CHUNK_SIZE];
    fread(compBfr, compChunkBytes - 1, 1, regFile);
    
    // decompress chunk into decompBfr
    int status;
    z_stream strm;
    strm.zalloc = (alloc_func)NULL;
    strm.zfree = (free_func)NULL;
    strm.opaque = NULL;
    
    strm.next_in = compBfr;
    strm.next_out = decompBfr;
    strm.avail_out = DECOMP_CHUNK_SIZE;
    strm.avail_in = (int)compChunkBytes - 1;
    
    inflateInit(&strm);
    status = inflate(&strm, Z_FINISH);
    inflateEnd(&strm);
    
    if(status != Z_STREAM_END) {
        std::cerr << "Error while decompressing" << std::endl;
        return -1;
    }
    
    rwPtr = 0;
    chunkBytes = strm.avail_out;
    
    return 0;
}


//******************************************************************************

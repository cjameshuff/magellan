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

#ifndef NBTIO_H
#define NBTIO_H

#include <zlib.h>
#include <stdint.h>

#include <vector>
#include <iostream>

//******************************************************************************

class NBT_I {
  public:
    NBT_I() {}
    
    virtual void Read(uint8_t * bfr, size_t size) = 0;
    
    virtual bool Eof() = 0;
    
    uint8_t Parse_UByte() {
        uint8_t data = 0;
        Read(&data, 1);
        return data;
    }
    
    int8_t Parse_Byte() {return Parse_UByte();}
    
    int16_t Parse_Short() {
        uint16_t val = ((uint16_t)Parse_UByte() << 8);
        val |= (uint16_t)Parse_UByte();
        return (uint16_t)val;
    }
    
    int32_t Parse_Int() {
        uint32_t val = ((uint32_t)Parse_UByte() << 24);
        val |= ((uint32_t)Parse_UByte() << 16);
        val |= ((uint32_t)Parse_UByte() << 8);
        val |= (uint32_t)Parse_UByte();
        return (int32_t)val;
    }
    
    int64_t Parse_Long() {
        uint64_t val = ((uint64_t)Parse_UByte() << 56);
        val |= ((uint64_t)Parse_UByte() << 48);
        val |= ((uint64_t)Parse_UByte() << 40);
        val |= ((uint64_t)Parse_UByte() << 32);
        val |= ((uint64_t)Parse_UByte() << 24);
        val |= ((uint64_t)Parse_UByte() << 16);
        val |= ((uint64_t)Parse_UByte() << 8);
        val |= (uint64_t)Parse_UByte();
        return (int64_t)val;
    }
    
    float Parse_Float() {
        // TODO: check endianness issues
        union {
            uint32_t intval;
            float floatval;
        };
        intval = ((uint32_t)Parse_UByte() << 24);
        intval |= ((uint32_t)Parse_UByte() << 16);
        intval |= ((uint32_t)Parse_UByte() << 8);
        intval |= (uint32_t)Parse_UByte();
        return floatval;
    }
    
    double Parse_Double() {
        // TODO: check endianness issues
        union {
            uint64_t intval;
            double dblval;
        };
        intval = ((uint64_t)Parse_UByte() << 56);
        intval |= ((uint64_t)Parse_UByte() << 48);
        intval |= ((uint64_t)Parse_UByte() << 40);
        intval |= ((uint64_t)Parse_UByte() << 32);
        intval |= ((uint64_t)Parse_UByte() << 24);
        intval |= ((uint64_t)Parse_UByte() << 16);
        intval |= ((uint64_t)Parse_UByte() << 8);
        intval |= (uint64_t)Parse_UByte();
        return dblval;
    }
    
    void Parse_ByteArray(std::vector<uint8_t> & array) {
        int length = Parse_Int();
        array.resize(length);
        Read(&array[0], length);
    }
    
    void Parse_String(std::string & str) {
        int length = Parse_Short();
        str.resize(length);
        Read((uint8_t*)&str[0], length);
    }
};

//******************************************************************************

class NBT_O {
  public:
    NBT_O() {}
    
    virtual void Write(void * bfr, size_t size) = 0;
    
    virtual bool Eof() = 0;
    
    void NBT_Write(int8_t val) {Write(&val, 1);}
    
    void NBT_Write(int16_t val) {
        NBT_Write((int8_t)((val >> 8) & 0xFF));
        NBT_Write((int8_t)(val & 0xFF));
    }
    
    void NBT_Write(int32_t val) {
        NBT_Write((int8_t)((val >> 24) & 0xFF));
        NBT_Write((int8_t)((val >> 16) & 0xFF));
        NBT_Write((int8_t)((val >> 8) & 0xFF));
        NBT_Write((int8_t)(val & 0xFF));
    }
    
    void NBT_Write(int64_t val) {
        NBT_Write((int8_t)((val >> 56) & 0xFF));
        NBT_Write((int8_t)((val >> 48) & 0xFF));
        NBT_Write((int8_t)((val >> 40) & 0xFF));
        NBT_Write((int8_t)((val >> 32) & 0xFF));
        NBT_Write((int8_t)((val >> 24) & 0xFF));
        NBT_Write((int8_t)((val >> 16) & 0xFF));
        NBT_Write((int8_t)((val >> 8) & 0xFF));
        NBT_Write((int8_t)(val & 0xFF));
    }
    
    void NBT_Write(float val) {
        union {
            int32_t intval;
            float floatval;
        };
        floatval = val;
        NBT_Write(intval);
    }
    
    void NBT_Write(double val) {
        union {
            int64_t intval;
            double dblval;
        };
        dblval = val;
        NBT_Write(intval);
    }
    
    void NBT_Write(const std::vector<uint8_t> & val) {
        NBT_Write((int32_t)val.size());
        Write((void *)&val[0], (int)val.size());
    }
    
    void NBT_Write(const std::string & val) {
        NBT_Write((int16_t)val.length());
        Write((void *)&val[0], (int)val.length());
    }
};

//******************************************************************************

class NBT_gzFile_I: public NBT_I {
  private:
    gzFile fin;
    
  public:
    NBT_gzFile_I(const std::string & fpath) {
        fin = gzopen(fpath.c_str(), "rb");
        if(!fin) {
            std::cerr << "Could not open \"" << fpath << "\"" << std::endl;
            int errnum;
            std::cerr << "ZLib error: \"" << gzerror(fin, &errnum) << "\"" << std::endl;
            gzclose(fin);
            exit(EXIT_FAILURE);
        }
    }
    ~NBT_gzFile_I() {gzclose(fin);}
    
    virtual void Read(uint8_t * bfr, size_t size) {
        if(gzread(fin, bfr, (int)size) != (int)size) {
            std::cerr << "Could not read data from file" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    virtual bool Eof() {return gzeof(fin);}
};


class NBT_gzFile_O: public NBT_O {
  private:
    gzFile fout;
    
  public:
    NBT_gzFile_O(const std::string & fpath) {
        fout = gzopen(fpath.c_str(), "wb");
        if(!fout) {
            std::cerr << "Could not open \"" << fpath << "\"" << std::endl;
            int errnum;
            std::cerr << "ZLib error: \"" << gzerror(fout, &errnum) << "\"" << std::endl;
            gzclose(fout);
        }
    }
    ~NBT_gzFile_O() {gzclose(fout);}
    
    virtual void Write(void * bfr, size_t size) {
        if(gzwrite(fout, (void *)bfr, (int)size) != (int)size) {
            std::cerr << "Could not write data to file" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    virtual bool Eof() {return gzeof(fout);}
};

struct RegionBlock {
    int start, size;
    RegionBlock() {}
    RegionBlock(const RegionBlock & rhs): start(rhs.start), size(rhs.size) {}
    RegionBlock(int st, int sz): start(st), size(sz) {}
};

class NBT_Region_IO: public NBT_I, public NBT_O {
  private:
    FILE * regFile;
    long fileSize;
    
    int chunkX, chunkZ;
    size_t chunkBytes;
    size_t rwPtr;
    uint8_t * decompBfr;
    
    uint32_t chunkTimestamps[1024];
    RegionBlock chunkBlocks[1024];// chunk blocks in index order
    std::vector<RegionBlock> freeBlocks;// heap of blocks of unused sectors
    int endUsedSectors;// index of sector after last used sector
    
    static size_t ChunkIdx(int cx, int cz) {return ((cx & 31) + (cz & 31)*32);}
    
    int ReadRegionTOC();
    void UpdateTOC(size_t chunkIdx, const RegionBlock & newBlock);
    
  public:
    NBT_Region_IO();
    ~NBT_Region_IO();
    
    int Open(const std::string & fpath);
    
    void PrintStats(std::ostream & ostrm);
    
    // Sets up chunk buffer for read/write operations
    void ClearChunkBuffer() {rwPtr = 0; chunkBytes = 0;}
    
    // Takes decompressed data buffer for external use. Caller is responsible for
    // deleting their copy. This can be used to keep chunk data around without copying
    // it into an external structure.
    uint8_t * StealChunkBuffer() {
        uint8_t * tmp = decompBfr;
        decompBfr = NULL;
        return tmp;
    }
    
    // The converse, set the chunk buffer. Caller gives up responsibility for deleting
    // the buffer passed in.
    void SetChunkBuffer(uint8_t * bfr) {
        if(decompBfr) delete[] decompBfr;
        decompBfr = bfr;
    }
    
    bool ChunkExists(int cx, int cz) const {
        size_t idx = ChunkIdx(cx, cz);
        return chunkBlocks[idx].start > 0 && chunkBlocks[idx].size > 0;
    }
    
    // Performs write of buffered chunk
    int WriteChunk(int cx, int cz);
    int WriteChunk() {return WriteChunk(chunkX, chunkZ);}
    
    // Reads a chunk into the chunk buffer
    int ReadChunk(int cx, int cz);
    
    // Get timestamp for currently buffered chunk. Only valid for chunks loaded from file.
    // Timestamp is automatically updated on chunk write.
    int32_t GetTimestamp() const {return chunkTimestamps[ChunkIdx(chunkX, chunkZ)];}
    
    // Get size in bytes of currently buffered chunk.
    size_t GetChunkSize() const {return chunkBytes;}
    
    // The following functions aren't called directly by the user, but are used by the NBT
    // loading/writing code.
    virtual void Read(uint8_t * bfr, size_t size) {
        memcpy(bfr, decompBfr + rwPtr, size);
        rwPtr += size;
    }
    
    virtual void Write(void * bfr, size_t size) {
        memcpy(decompBfr + rwPtr, bfr, size);
        rwPtr += size;
        chunkBytes += size;
        // Random access writes not supported. Writes start at the beginning of the buffer
        // and go sequentially from there.
    }
    
    virtual bool Eof() {return rwPtr >= chunkBytes;}
};

#endif // NBTIO_H


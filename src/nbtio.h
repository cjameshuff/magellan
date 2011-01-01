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

//******************************************************************************

static inline uint8_t Parse_UByte(gzFile fin)
{
    uint8_t data = 0;
    if(gzread(fin, &data, 1) != 1) {
        std::cerr << "Could not read data from file" << std::endl;
        exit(EXIT_FAILURE);
    }
    return data;
}

static inline int8_t Parse_Byte(gzFile fin)
{
    int8_t data = 0;
    if(gzread(fin, &data, 1) != 1) {
        std::cerr << "Could not read data from file" << std::endl;
        exit(EXIT_FAILURE);
    }
    return data;
}

static inline int16_t Parse_Short(gzFile fin)
{
    uint16_t val = ((uint16_t)Parse_UByte(fin) << 8);
    val |= (uint16_t)Parse_UByte(fin);
    return (uint16_t)val;
}

static inline int32_t Parse_Int(gzFile fin)
{
    uint32_t val = ((uint32_t)Parse_UByte(fin) << 24);
    val |= ((uint32_t)Parse_UByte(fin) << 16);
    val |= ((uint32_t)Parse_UByte(fin) << 8);
    val |= (uint32_t)Parse_UByte(fin);
    return (int32_t)val;
}

static inline int64_t Parse_Long(gzFile fin)
{
    uint64_t val = ((uint64_t)Parse_UByte(fin) << 56);
    val |= ((uint64_t)Parse_UByte(fin) << 48);
    val |= ((uint64_t)Parse_UByte(fin) << 40);
    val |= ((uint64_t)Parse_UByte(fin) << 32);
    val |= ((uint64_t)Parse_UByte(fin) << 24);
    val |= ((uint64_t)Parse_UByte(fin) << 16);
    val |= ((uint64_t)Parse_UByte(fin) << 8);
    val |= (uint64_t)Parse_UByte(fin);
    return (int64_t)val;
}

static inline float Parse_Float(gzFile fin) {
    // TODO: check endianness issues
    union {
        uint32_t intval;
        float floatval;
    };
    intval = ((uint32_t)Parse_UByte(fin) << 24);
    intval |= ((uint32_t)Parse_UByte(fin) << 16);
    intval |= ((uint32_t)Parse_UByte(fin) << 8);
    intval |= (uint32_t)Parse_UByte(fin);
    return floatval;
}

static inline double Parse_Double(gzFile fin) {
    // TODO: check endianness issues
    union {
        uint64_t intval;
        double dblval;
    };
    intval = ((uint64_t)Parse_UByte(fin) << 56);
    intval |= ((uint64_t)Parse_UByte(fin) << 48);
    intval |= ((uint64_t)Parse_UByte(fin) << 40);
    intval |= ((uint64_t)Parse_UByte(fin) << 32);
    intval |= ((uint64_t)Parse_UByte(fin) << 24);
    intval |= ((uint64_t)Parse_UByte(fin) << 16);
    intval |= ((uint64_t)Parse_UByte(fin) << 8);
    intval |= (uint64_t)Parse_UByte(fin);
    return dblval;
}

static inline void Parse_ByteArray(std::vector<uint8_t> & array, gzFile fin)
{
    size_t length = Parse_Int(fin);
    array.resize(length);
    gzread(fin, &array[0], length);
}

static inline void Parse_String(std::string & str, gzFile fin)
{
    size_t length = Parse_Short(fin);
    str.resize(length);
    gzread(fin, &str[0], length);
}

//******************************************************************************

template<typename T>
T Parse_Value(gzFile fin) {}

/*template<>
int8_t Parse_Value<int8_t>(gzFile fin) {return Parse_Byte(fin);}
//int8_t Parse_Value(gzFile fin);
//int16_t Parse_Value(gzFile fin);
//int32_t Parse_Value(gzFile fin);
//int64_t Parse_Value(gzFile fin);
//float Parse_Value(gzFile fin);
//double Parse_Value(gzFile fin);

template<typename T>
NBT_TagList<T> * Parse_TAG_ListItems(gzFile fin)
{
    ++crsr;// element type
}*/

//******************************************************************************

static inline void NBT_Write(gzFile fout, int8_t val) {
    if(gzwrite(fout, &val, 1) != 1) {
        std::cerr << "Could not write data to file" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static inline void NBT_Write(gzFile fout, int16_t val) {
    NBT_Write(fout, (int8_t)((val >> 8) & 0xFF));
    NBT_Write(fout, (int8_t)(val & 0xFF));
}

static inline void NBT_Write(gzFile fout, int32_t val) {
    NBT_Write(fout, (int8_t)((val >> 24) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 16) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 8) & 0xFF));
    NBT_Write(fout, (int8_t)(val & 0xFF));
}

static inline void NBT_Write(gzFile fout, int64_t val) {
    NBT_Write(fout, (int8_t)((val >> 56) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 48) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 40) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 32) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 24) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 16) & 0xFF));
    NBT_Write(fout, (int8_t)((val >> 8) & 0xFF));
    NBT_Write(fout, (int8_t)(val & 0xFF));
}

static inline void NBT_Write(gzFile fout, float val) {
    union {
        int32_t intval;
        float floatval;
    };
    floatval = val;
    NBT_Write(fout, intval);
}

static inline void NBT_Write(gzFile fout, double val) {
    union {
        int64_t intval;
        double dblval;
    };
    dblval = val;
    NBT_Write(fout, intval);
}

static inline void NBT_Write(gzFile fout, const std::vector<uint8_t> & val) {
    NBT_Write(fout, (int32_t)val.size());
    if(gzwrite(fout, &val[0], val.size()) != (int)val.size()) {
        std::cerr << "Could not write byte array data to file" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static inline void NBT_Write(gzFile fout, const std::string & val) {
    NBT_Write(fout, (int16_t)val.length());
    if(gzwrite(fout, &val[0], val.length()) != (int)val.length()) {
        std::cerr << "Could not write string data to file" << std::endl;
        exit(EXIT_FAILURE);
    }
}

#endif // NBTIO_H


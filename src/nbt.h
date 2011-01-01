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

// Note, char strings are used here, but the string class preserves non-ASCII
// values, so conversion to wstrings is trivial for things that require it.

// http://www.minecraft.net/docs/NBT.txt

#ifndef NBT_H
#define NBT_H

#include <cstdlib>
#include <climits>

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <zlib.h>

#include "nbtio.h"


enum nbt_tag_t {
    kNBT_TAG_End = 0,        // NULL
    kNBT_TAG_Byte = 1,       // int8_t
    kNBT_TAG_Short = 2,      // int16_t
    kNBT_TAG_Int = 3,        // int32_t
    kNBT_TAG_Long = 4,       // int64_t
    kNBT_TAG_Float = 5,      // float
    kNBT_TAG_Double = 6,     // double
    kNBT_TAG_Byte_Array = 7, // vector<int8_t> *
    kNBT_TAG_String = 8,     // string *
    kNBT_TAG_List = 9,       // vector<NBT_Tag> *
    kNBT_TAG_Compound = 10   // vector<NBT_Tag> *
};

extern const std::string kTypeNames[];

extern int tagIndentLevel;

std::string TagTab();

//******************************************************************************

struct NBT_Tag {
    std::string name;
    
    NBT_Tag() {}
    NBT_Tag(const std::string nm): name(nm) {}
    virtual ~NBT_Tag() {}
    
//    virtual NBT_Tag * Clone() const = 0;
    
    virtual nbt_tag_t Type() const = 0;
    
    virtual void Print(std::ostream & ostrm) = 0;
    
    virtual void Write(gzFile fout) {}
    virtual void WriteData(gzFile fout) = 0;
//    virtual void Write(gzFile fout) = 0;
};

//******************************************************************************
struct NBT_TagCompound: public NBT_Tag {
    std::vector<NBT_Tag *> tags;
    std::map<std::string, NBT_Tag *> tagsByName;
    
    NBT_TagCompound() {}
    ~NBT_TagCompound() {
        for(std::vector<NBT_Tag *>::iterator t = tags.begin(); t != tags.end(); ++t)
            delete *t;
    }
    
    // Does not check for existing tags
    void AddTag(NBT_Tag * tag) {
        tags.push_back(tag);
        if(tag->name != "")
            tagsByName[tag->name] = tag;
    }
    
    // Insert or replace a tag
    void SetTag(NBT_Tag * tag) {
        std::map<std::string, NBT_Tag *>::iterator t = tagsByName.find(tag->name);
        if(t != tagsByName.end()) {
            // replace existing tag
            for(std::vector<NBT_Tag *>::iterator t2 = tags.begin(); t2 != tags.end(); ++t2) {
                if(*t2 == t->second) {
                    delete *t2;
                    *t2 = tag;
                    t->second = tag;
                    break;
                }
            }
        }
        else {
            tags.push_back(tag);
            if(tag->name != "")
                tagsByName[tag->name] = tag;
        }
    }
    
    // Get tag of given type and name. Give error if no tag with childName exists, or
    // if it exists but has the wrong type.
    template<typename T>
    T * GetTag(const std::string & childName) {
        std::map<std::string, NBT_Tag *>::iterator tag = tagsByName.find(childName);
        if(tag == tagsByName.end()) {
            std::cerr << "No tag \"" << childName << "\" in \"" << name << "\"!" << std::endl;
            exit(EXIT_FAILURE);
        }
        T * child = dynamic_cast<T *>(tag->second);
        if(!child) {
            std::cerr << "Tag \"" << childName << "\" in \"" << name << "\" has wrong type!" << std::endl;
            exit(EXIT_FAILURE);
        }
        return child;
    }
    // const version
    template<typename T>
    const T * GetTag(const std::string & childName) const {
        std::map<std::string, NBT_Tag *>::iterator tag = tagsByName.find(childName);
        if(tag == tagsByName.end()) {
            std::cerr << "No tag \"" << childName << "\" in \"" << name << "\"!" << std::endl;
            exit(EXIT_FAILURE);
        }
        T * child = dynamic_cast<T *>(tag->second);
        if(!child) {
            std::cerr << "Tag \"" << childName << "\" in \"" << name << "\" has wrong type!" << std::endl;
            exit(EXIT_FAILURE);
        }
        return child;
    }
    
    // Get tag of given type and name. Returns given default value if no tag with
    // name exists, or if it exists but has the wrong type.
    template<typename T>
    T * GetTag(const std::string & name, T * defVal) {
        std::map<std::string, NBT_Tag *>::iterator tag = tagsByName.find(name);
        return (tag == tagsByName.end())? defVal : dynamic_cast<T *>(tag->second);
    }
    
    virtual nbt_tag_t Type() const {return kNBT_TAG_Compound;}
    
    virtual void Print(std::ostream & ostrm);
    virtual void Write(gzFile fout);
    virtual void WriteData(gzFile fout);
};

//******************************************************************************

struct NBT_TagList: public NBT_Tag {
  public:
    nbt_tag_t valueType;
    std::vector<NBT_Tag *> values;
    
    NBT_TagList(nbt_tag_t vt = kNBT_TAG_End): valueType(vt) {}
    ~NBT_TagList() {
        for(std::vector<NBT_Tag *>::iterator t = values.begin(); t != values.end(); ++t)
            delete *t;
    }
  
    virtual nbt_tag_t Type() const {return kNBT_TAG_List;}
    virtual nbt_tag_t ValueType() const {return valueType;}
    
    virtual void Print(std::ostream & ostrm);
    virtual void Write(gzFile fout);
    virtual void WriteData(gzFile fout);
};

//******************************************************************************

template<typename T>
struct NBT_TagValue: public NBT_Tag {
    T value;
    
    NBT_TagValue() {}
    NBT_TagValue(T val): NBT_Tag(""), value(val) {}
    NBT_TagValue(const std::string & nm, T val): NBT_Tag(nm), value(val) {}
    ~NBT_TagValue() {}
    
    virtual nbt_tag_t Type() const {return -1;}
    
    virtual void Print(std::ostream & ostrm) {
        ostrm << TagTab() << kTypeNames[Type()] << "(\"" << name << "\"): " << value << std::endl;
    }
    virtual void Write(gzFile fout) {
        NBT_Write(fout, (int8_t)Type());
        NBT_Write(fout, name);
        NBT_Write(fout, value);
    }
    virtual void WriteData(gzFile fout) {
        NBT_Write(fout, value);
    }
};

template<>
inline nbt_tag_t NBT_TagValue<int8_t>::Type() const {return kNBT_TAG_Byte;}

template<>
inline void NBT_TagValue<int8_t>::Print(std::ostream & ostrm) {
    ostrm << TagTab() << kTypeNames[Type()] << "(\"" << name << "\"): " << (int)value << std::endl;
}

template<>
inline nbt_tag_t NBT_TagValue<int16_t>::Type() const {return kNBT_TAG_Short;}

template<>
inline nbt_tag_t NBT_TagValue<int32_t>::Type() const {return kNBT_TAG_Int;}

template<>
inline nbt_tag_t NBT_TagValue<int64_t>::Type() const {return kNBT_TAG_Long;}

template<>
inline nbt_tag_t NBT_TagValue<float>::Type() const {return kNBT_TAG_Float;}

template<>
inline nbt_tag_t NBT_TagValue<double>::Type() const {return kNBT_TAG_Double;}

template<>
inline nbt_tag_t NBT_TagValue<std::vector<uint8_t> >::Type() const {return kNBT_TAG_Byte_Array;}

template<>
inline void NBT_TagValue<std::vector<uint8_t> >::Print(std::ostream & ostrm) {
    ostrm << TagTab() << "TAG_Byte_Array(\"" << name << "\"): " << value.size() << " bytes" << std::endl;
    ostrm << TagTab() << '{' << std::endl;
    ++tagIndentLevel;
    for(std::vector<uint8_t>::iterator t = value.begin(); t != value.end(); ++t) {
        ostrm << (int)*t << ' ';
    }
    --tagIndentLevel;
    ostrm << TagTab() << "\n}" << std::endl;
}

template<>
inline nbt_tag_t NBT_TagValue<std::string>::Type() const {return kNBT_TAG_String;}

typedef NBT_TagValue<int8_t> NBT_TagByte;
typedef NBT_TagValue<int16_t> NBT_TagShort;
typedef NBT_TagValue<int32_t> NBT_TagInt;
typedef NBT_TagValue<int64_t> NBT_TagLong;
typedef NBT_TagValue<float> NBT_TagFloat;
typedef NBT_TagValue<double> NBT_TagDouble;
typedef NBT_TagValue<std::vector<uint8_t> > NBT_TagByteArray;
typedef NBT_TagValue<std::string> NBT_TagString;


//******************************************************************************

NBT_TagCompound * LoadNBT_File(const std::string & path);

//******************************************************************************
#endif // NBT_H

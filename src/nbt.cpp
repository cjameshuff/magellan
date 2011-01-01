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

#include "nbt.h"

#include <vector>

using namespace std;

int tagIndentLevel = 0;

const std::string kTypeNames[] = {
    "TAG_End",
    "TAG_Byte",
    "TAG_Short",
    "TAG_Int",
    "TAG_Long",
    "TAG_Float",
    "TAG_Double",
    "TAG_Byte_Array",
    "TAG_String",
    "TAG_List",
    "TAG_Compound"
};

//******************************************************************************

NBT_Tag * Parse_Tag(gzFile fin);

NBT_TagCompound * Parse_TAG_Compound(const std::string & name, gzFile fin);
NBT_Tag * Parse_TAG_List(const std::string & name, gzFile fin);

NBT_Tag * Parse_TagData(nbt_tag_t type, const std::string & name, gzFile fin);

//std::string TagTab() {return std::string(tagIndentLevel, '\t');}
std::string TagTab() {return std::string(tagIndentLevel*2, ' ');}

//******************************************************************************
NBT_TagCompound * LoadNBT_File(const std::string & path)
{
    gzFile fin = gzopen(path.c_str(), "rb");
    if(!fin) {
        cerr << "Could not open \"" << path << "\"" << endl;
        int errnum;
        cerr << "ZLib error: \"" << gzerror(fin, &errnum) << "\"" << endl;
        gzclose(fin);
        //return NULL;
        exit(EXIT_FAILURE);
    }
    
//    cerr << "Loading file \"" << path << "\"" << endl;
    // Tag format:
    // TYPE:8 STRING:16+n PAYLOAD
    // File contains a single TAG_Compound tag
    
    nbt_tag_t type = (nbt_tag_t)Parse_Byte(fin);
    
    if(type != kNBT_TAG_Compound) {
        cerr << "Bad file format \"" << path << "\"" << endl;
        gzclose(fin);
        //return NULL;
        exit(EXIT_FAILURE);
    }
    
    string name;
    Parse_String(name, fin);
    NBT_TagCompound * tag = Parse_TAG_Compound(name, fin);
    
//    cerr << "File \"" << path << "\" loaded" << endl;
    
    gzclose(fin);
    return tag;
}

//******************************************************************************

NBT_Tag * Parse_TAG_List(const std::string & name, gzFile fin)
{
    nbt_tag_t valueType = (nbt_tag_t)Parse_Byte(fin);
    size_t size = Parse_Int(fin);
    
    NBT_TagList * lst = new NBT_TagList(valueType);
    lst->name = name;
    lst->values.resize(size);
//    cerr << "Loading list \"" << name << "\", elements: " << size << endl;
    
    // Now, parse tag data entries without name or type
    for(size_t j = 0; j < size; ++j)
        lst->values[j] = Parse_TagData(valueType, "", fin);
    
//    cerr << "List \"" << name << "\" loaded" << endl;
    return lst;
}

//******************************************************************************

NBT_TagCompound * Parse_TAG_Compound(const std::string & name, gzFile fin)
{
    NBT_TagCompound * compTag = new NBT_TagCompound;
    compTag->name = name;
//    cerr << "Compound tag: \"" << name << "\"" << endl;
    while(!gzeof(fin)) {
//        cerr << "Loading member of \"" << name << "\"" << endl;
        NBT_Tag * tag = Parse_Tag(fin);
        if(tag) {
            compTag->AddTag(tag);
        }
        else {
//            cerr << "End of compound tag \"" << name << "\"" << endl;
            break;// was kNBT_TAG_End
        }
    }
    return compTag;
}

//******************************************************************************

NBT_Tag * Parse_Tag(gzFile fin)
{
//    cerr << "Parsing tag type" << endl;
    nbt_tag_t type = (nbt_tag_t)Parse_Byte(fin);
//    cerr << "Tag type " << (int)type << ", typename: " << kTypeNames[type] << endl;
    
    string name;
    if(type != kNBT_TAG_End)
        Parse_String(name, fin);
    
//    cerr << "Loading tag \"" << name << "\", type: " << kTypeNames[type] << endl;
    return Parse_TagData(type, name, fin);
}

NBT_Tag * Parse_TagData(nbt_tag_t type, const std::string & name, gzFile fin)
{
    switch(type) {
      case kNBT_TAG_End:
        return NULL;
      break;
      case kNBT_TAG_Byte:
        return new NBT_TagValue<int8_t>(name, Parse_Byte(fin));
      break;
      case kNBT_TAG_Short:
        return new NBT_TagValue<int16_t>(name, Parse_Short(fin));
      break;
      case kNBT_TAG_Int:
        return new NBT_TagValue<int32_t>(name, Parse_Int(fin));
      break;
      case kNBT_TAG_Long:
        return new NBT_TagValue<int64_t>(name, Parse_Long(fin));
      break;
      case kNBT_TAG_Float:
        return new NBT_TagValue<float>(name, Parse_Float(fin));
      break;
      case kNBT_TAG_Double:
        return new NBT_TagValue<double>(name, Parse_Double(fin));
      break;
      case kNBT_TAG_Byte_Array: {
        NBT_TagByteArray * tag = new NBT_TagValue<vector<uint8_t> >();
        tag->name = name;
        Parse_ByteArray(tag->value, fin);
        return tag;
      } break;
      case kNBT_TAG_String: {
        NBT_TagString * tag = new NBT_TagString();
        tag->name = name;
        Parse_String(tag->value, fin);
        return tag;
      } break;
      case kNBT_TAG_List:
        return Parse_TAG_List(name, fin);
      break;
      case kNBT_TAG_Compound:
        return Parse_TAG_Compound(name, fin);
      break;
      default:
        cerr << "Unknown tag type" << endl;
        exit(EXIT_FAILURE);
    }
    return NULL;
}

//******************************************************************************
void NBT_TagCompound::Print(std::ostream & ostrm)
{
    ostrm << TagTab() << "TAG_Compound(\"" << name << "\"): " << tags.size() << " entries" << std::endl;
    ostrm << TagTab() << '{' << std::endl;
    ++tagIndentLevel;
    for(std::vector<NBT_Tag *>::iterator t = tags.begin(); t != tags.end(); ++t)
        (*t)->Print(ostrm);
    --tagIndentLevel;
    ostrm << TagTab() << '}' << std::endl;
}

void NBT_TagCompound::Write(gzFile fout)
{
    NBT_Write(fout, (int8_t)Type());
    NBT_Write(fout, name);
    WriteData(fout);
}

void NBT_TagCompound::WriteData(gzFile fout)
{
    for(std::vector<NBT_Tag *>::iterator t = tags.begin(); t != tags.end(); ++t)
        (*t)->Write(fout);
    NBT_Write(fout, (int8_t)kNBT_TAG_End);
}

//******************************************************************************

void NBT_TagList::Print(std::ostream & ostrm)
{
    ostrm << TagTab() << "TAG_List(\"" << name << "\"): " << values.size()
          << " entries of type " << kTypeNames[valueType] << std::endl;
    ostrm << TagTab() << '{' << std::endl;
    ++tagIndentLevel;
    for(std::vector<NBT_Tag *>::iterator t = values.begin(); t != values.end(); ++t)
        (*t)->Print(ostrm);
    --tagIndentLevel;
    ostrm << TagTab() << '}' << std::endl;
}

void NBT_TagList::Write(gzFile fout)
{
    NBT_Write(fout, (int8_t)Type());
    NBT_Write(fout, name);
    WriteData(fout);
}

void NBT_TagList::WriteData(gzFile fout)
{
    // Single type shared among all list entries, entries are unnamed 
    NBT_Write(fout, (int8_t)ValueType());
    NBT_Write(fout, (int32_t)values.size());
    for(std::vector<NBT_Tag *>::iterator t = values.begin(); t != values.end(); ++t) {
        (*t)->WriteData(fout);
        NBT_Write(fout, (int8_t)kNBT_TAG_End);
    }
}

//******************************************************************************

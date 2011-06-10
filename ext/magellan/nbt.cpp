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

NBT_Tag * Parse_Tag(NBT_I & fin);

NBT_TagCompound * Parse_TAG_Compound(const std::string & name, NBT_I & fin);
NBT_Tag * Parse_TAG_List(const std::string & name, NBT_I & fin);

NBT_Tag * Parse_TagData(nbt_tag_t type, const std::string & name, NBT_I & fin);

//std::string TagTab() {return std::string(tagIndentLevel, '\t');}
std::string TagTab() {return std::string(tagIndentLevel*2, ' ');}

//******************************************************************************
NBT_TagCompound * LoadNBT_File(NBT_I & fin)
{
//    cerr << "Loading file \"" << path << "\"" << endl;
    // Tag format:
    // TYPE:8 STRING:16+n PAYLOAD
    // File contains a single TAG_Compound tag
    
    nbt_tag_t type = (nbt_tag_t)fin.Parse_Byte();
    
    if(type != kNBT_TAG_Compound) {
        cerr << "Bad file format" << endl;
        //return NULL;
        exit(EXIT_FAILURE);
    }
    
    string name;
    fin.Parse_String(name);
    NBT_TagCompound * tag = Parse_TAG_Compound(name, fin);
    
//    cerr << "File \"" << path << "\" loaded" << endl;
    return tag;
}

//******************************************************************************

int WriteNBT_File(const NBT_TagCompound * nbt, const std::string & path)
{
    NBT_gzFile_O fout(path);
    nbt->Write(fout);
    return 0;
}

//******************************************************************************

NBT_Tag * Parse_TAG_List(const std::string & name, NBT_I & fin)
{
    nbt_tag_t valueType = (nbt_tag_t)fin.Parse_Byte();
    size_t size = fin.Parse_Int();
    
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

NBT_TagCompound * Parse_TAG_Compound(const std::string & name, NBT_I & fin)
{
    NBT_TagCompound * compTag = new NBT_TagCompound;
    compTag->name = name;
//    cerr << "Compound tag: \"" << name << "\"" << endl;
    while(!fin.Eof()) {
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

NBT_Tag * Parse_Tag(NBT_I & fin)
{
//    cerr << "Parsing tag type" << endl;
    nbt_tag_t type = (nbt_tag_t)fin.Parse_Byte();
    
    if(type >= kNBT_NumTagTypes) {
        cerr << "Invalid tag type: " << (int)type << endl;
        abort();
    }
//    cerr << "Tag type " << (int)type << ", typename: " << kTypeNames[type] << endl;
    
    string name;
    if(type != kNBT_TAG_End)
        fin.Parse_String(name);
    
//    cerr << "Loading tag \"" << name << "\", type: " << kTypeNames[type] << endl;
    return Parse_TagData(type, name, fin);
}

NBT_Tag * Parse_TagData(nbt_tag_t type, const std::string & name, NBT_I & fin)
{
    switch(type) {
      case kNBT_TAG_End:
        return NULL;
      break;
      case kNBT_TAG_Byte:
        return new NBT_TagValue<int8_t>(name, fin.Parse_Byte());
      break;
      case kNBT_TAG_Short:
        return new NBT_TagValue<int16_t>(name, fin.Parse_Short());
      break;
      case kNBT_TAG_Int:
        return new NBT_TagValue<int32_t>(name, fin.Parse_Int());
      break;
      case kNBT_TAG_Long:
        return new NBT_TagValue<int64_t>(name, fin.Parse_Long());
      break;
      case kNBT_TAG_Float:
        return new NBT_TagValue<float>(name, fin.Parse_Float());
      break;
      case kNBT_TAG_Double:
        return new NBT_TagValue<double>(name, fin.Parse_Double());
      break;
      case kNBT_TAG_Byte_Array: {
        NBT_TagByteArray * tag = new NBT_TagValue<vector<uint8_t> >();
        tag->name = name;
        fin.Parse_ByteArray(tag->value);
        return tag;
      } break;
      case kNBT_TAG_String: {
        NBT_TagString * tag = new NBT_TagString();
        tag->name = name;
        fin.Parse_String(tag->value);
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

void NBT_TagCompound::Write(NBT_O & fout) const
{
    fout.NBT_Write((int8_t)Type());
    fout.NBT_Write(name);
    WriteData(fout);
}

void NBT_TagCompound::WriteData(NBT_O & fout) const
{
    for(std::vector<NBT_Tag *>::const_iterator t = tags.begin(); t != tags.end(); ++t)
        (*t)->Write(fout);
    fout.NBT_Write((int8_t)kNBT_TAG_End);
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

void NBT_TagList::Write(NBT_O & fout) const
{
    fout.NBT_Write((int8_t)Type());
    fout.NBT_Write(name);
    WriteData(fout);
}

void NBT_TagList::WriteData(NBT_O & fout) const
{
    // Single type shared among all list entries, entries are unnamed 
    fout.NBT_Write((int8_t)ValueType());
    fout.NBT_Write((int32_t)values.size());
    for(std::vector<NBT_Tag *>::const_iterator t = values.begin(); t != values.end(); ++t)
        (*t)->WriteData(fout);
}

//******************************************************************************

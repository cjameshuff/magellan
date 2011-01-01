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

#include <iostream>
#include <string>
#include <sstream>

#include <sys/stat.h>
#include <errno.h>

#include "misc.h"

#include "nbt.h"
#include "nbtlua.h"

using namespace std;


static lua_State * lua = NULL;

//******************************************************************************

#define lua_userdata_cast(L, pos, T) reinterpret_cast<T*>(luaL_checkudata((L), (pos), #T))

template<typename T>
int GCMethod(lua_State* L)
{
    reinterpret_cast<T*>(lua_touserdata(L, 1))->~T();
    return 0;
}
// lua_pushstring(L, "__gc");
// lua_pushcfunction(L, GCMethod<MyClass>);
// lua_settable(L, 1);



//******************************************************************************
// new* functions for NBT types
//******************************************************************************
static int Lua_NewNBT(lua_State * L, NBT_Tag * nbt)
{
	NBT_Tag ** nbthdl = (NBT_Tag **)lua_newuserdata(L, sizeof(NBT_Tag *));
    *nbthdl = nbt;
    
    luaL_getmetatable(L, "MC.NBT");
    lua_setmetatable(L, -2);
    
    return 1;
}

//static int Lua_NewNBT_Byte(lua_State * L, int8_t val) {
//    return Lua_NewNBT(L, new NBT_TagByte(val));
//}
static int Lua_NewNBT_Byte(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagByte(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)));
    return 1;
}
static int Lua_NewNBT_Short(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagShort(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)));
    return 1;
}
static int Lua_NewNBT_Integer(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagInt(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)));
    return 1;
}
static int Lua_NewNBT_Long(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagLong(luaL_checkstring(L, 1), luaL_checkinteger(L, 2)));
    return 1;
}
static int Lua_NewNBT_Float(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagFloat(luaL_checkstring(L, 1), luaL_checknumber(L, 2)));
    return 1;
}
static int Lua_NewNBT_Double(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagDouble(luaL_checkstring(L, 1), luaL_checknumber(L, 2)));
    return 1;
}
static int Lua_NewNBT_String(lua_State * L) {
    Lua_NewNBT(L, new NBT_TagString(luaL_checkstring(L, 1), luaL_checkstring(L, 2)));
    return 1;
}
static int Lua_NewNBT_ByteArray(lua_State * L) {
    // byte arrays are initialized with size
    // TODO: initial value as well?
    NBT_TagByteArray * nbt = new NBT_TagByteArray();
    nbt->name = luaL_checkstring(L, 1);
    nbt->value.resize(luaL_checkinteger(L, 2));
    Lua_NewNBT(L, nbt);
    return 1;
}
static int Lua_NewNBT_Compound(lua_State * L) {
    NBT_TagCompound * nbt = new NBT_TagCompound();
    nbt->name = luaL_checkstring(L, 1);
    Lua_NewNBT(L, nbt);
    return 1;
}
static int Lua_NewNBT_List(lua_State * L) {
    NBT_TagList * nbt = new NBT_TagList();
    nbt->name = luaL_checkstring(L, 1);
    Lua_NewNBT(L, nbt);
    return 1;
}

//******************************************************************************
// check* functions for NBT types
//******************************************************************************
static NBT_Tag * checkNBT(lua_State * L, int argn) {
    void * ud = luaL_checkudata(L, argn, "MC.NBT");
    luaL_argcheck(L, ud != NULL, argn, "'NBT' expected");
    return *(NBT_Tag **)ud;
}

static NBT_TagByte * checkNBT_Byte(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagByte * nbt = dynamic_cast<NBT_TagByte *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Byte NBT' expected");
    return nbt;
}
static NBT_TagShort * checkNBT_Short(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagShort * nbt = dynamic_cast<NBT_TagShort *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Short NBT' expected");
    return nbt;
}
static NBT_TagInt * checkNBT_Integer(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagInt * nbt = dynamic_cast<NBT_TagInt *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Integer NBT' expected");
    return nbt;
}
static NBT_TagLong * checkNBT_Long(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagLong * nbt = dynamic_cast<NBT_TagLong *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Long NBT' expected");
    return nbt;
}
static NBT_TagFloat * checkNBT_Float(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagFloat * nbt = dynamic_cast<NBT_TagFloat *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Float NBT' expected");
    return nbt;
}
static NBT_TagDouble * checkNBT_Double(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagDouble * nbt = dynamic_cast<NBT_TagDouble *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Double NBT' expected");
    return nbt;
}
static NBT_TagString * checkNBT_String(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagString * nbt = dynamic_cast<NBT_TagString *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'String NBT' expected");
    return nbt;
}

static NBT_TagByteArray * checkNBT_ByteArray(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagByteArray * nbt = dynamic_cast<NBT_TagByteArray *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'String NBT' expected");
    return nbt;
}

static NBT_TagCompound * checkNBT_Compound(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagCompound * nbt = dynamic_cast<NBT_TagCompound *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'Compound NBT' expected");
    return nbt;
}

static NBT_TagList * checkNBT_List(lua_State * L, int argn) {
    NBT_Tag * ud = checkNBT(L, argn);
    NBT_TagList * nbt = dynamic_cast<NBT_TagList *>(ud);
    luaL_argcheck(L, nbt != NULL, argn, "'List NBT' expected");
    return nbt;
}

//******************************************************************************
// Value getter/setters
//******************************************************************************

static int Lua_GetNBT_ByteVal(lua_State * L) {
    lua_pushinteger(L, checkNBT_Byte(L, 1)->value);
    return 1;
}
static int Lua_SetNBT_ByteVal(lua_State * L) {
    checkNBT_Byte(L, 1)->value = luaL_checkinteger(L, 2);
    return 0;
}

static int Lua_GetNBT_ShortVal(lua_State * L) {
    lua_pushinteger(L, checkNBT_Short(L, 1)->value);
    return 1;
}
static int Lua_SetNBT_ShortVal(lua_State * L) {
    checkNBT_Short(L, 1)->value = luaL_checkinteger(L, 2);
    return 0;
}

static int Lua_GetNBT_IntVal(lua_State * L) {
    NBT_TagInt * nbt = checkNBT_Integer(L, 1);
    lua_pushinteger(L, nbt->value);
    return 1;
}
static int Lua_SetNBT_IntVal(lua_State * L) {
    checkNBT_Integer(L, 1)->value = luaL_checkinteger(L, 2);
    return 0;
}

static int Lua_GetNBT_LongVal(lua_State * L) {
    lua_pushinteger(L, checkNBT_Long(L, 1)->value);
    return 1;
}
static int Lua_SetNBT_LongVal(lua_State * L) {
    checkNBT_Long(L, 1)->value = luaL_checkinteger(L, 2);
    return 0;
}

static int Lua_GetNBT_FloatVal(lua_State * L) {
    lua_pushnumber(L, checkNBT_Float(L, 1)->value);
    return 1;
}
static int Lua_SetNBT_FloatVal(lua_State * L) {
    checkNBT_Float(L, 1)->value = luaL_checknumber(L, 2);
    return 0;
}

static int Lua_GetNBT_DoubleVal(lua_State * L) {
    lua_pushnumber(L, checkNBT_Double(L, 1)->value);
    return 1;
}
static int Lua_SetNBT_DoubleVal(lua_State * L) {
    checkNBT_Double(L, 1)->value = luaL_checknumber(L, 2);
    return 0;
}

static int Lua_GetNBT_StringVal(lua_State * L) {
    lua_pushstring(L, checkNBT_String(L, 1)->value.c_str());
    return 1;
}
static int Lua_SetNBT_StringVal(lua_State * L) {
    checkNBT_String(L, 1)->value = luaL_checkstring(L, 2);
    return 0;
}


static int Lua_GetNBT_Val(lua_State * L) {
    NBT_Tag * nbt = checkNBT(L, 1);
    switch(nbt->Type()) {
        case kNBT_TAG_End: lua_pushnumber(L, 0); break; // return nil?
        case kNBT_TAG_Byte: lua_pushnumber(L, checkNBT_Byte(L, 1)->value); break;
        case kNBT_TAG_Short: lua_pushnumber(L, checkNBT_Short(L, 1)->value); break;
        case kNBT_TAG_Int: lua_pushnumber(L, checkNBT_Integer(L, 1)->value); break;
        case kNBT_TAG_Long: lua_pushnumber(L, checkNBT_Long(L, 1)->value); break;
        case kNBT_TAG_Float: lua_pushnumber(L, checkNBT_Float(L, 1)->value); break;
        case kNBT_TAG_Double: lua_pushnumber(L, checkNBT_Double(L, 1)->value); break;
        case kNBT_TAG_Byte_Array: lua_pushnumber(L, 0); break; // TODO: something more useful
        case kNBT_TAG_String: lua_pushstring(L, checkNBT_String(L, 1)->value.c_str()); break;
        case kNBT_TAG_List: lua_pushnumber(L, 0); break; // TODO: something more useful
        case kNBT_TAG_Compound: lua_pushnumber(L, 0); break; // TODO: something more useful
    }
    return 1;
}

/*static int Lua_GetNBT(lua_State * L) {
    printf("Lua_GetNBT\n");
    NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
    string compName = luaL_checkstring(L, 2);
    printf("Getting element %s\n", compName.c_str());
    NBT_Tag * nbt = nbtcomp->GetTag<NBT_Tag>(compName);
	Lua_NewNBT(L, nbt);
    return 1;
}*/


static int Lua_NBT_index(lua_State * L)
{
    // Lists and byte arrays have numeric indices, compound NBTs have string indices
    // Numeric indices are 0-based.
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        // TODO: allow indexing compound by number, for iteration purposes?
        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
        Lua_NewNBT(L, nbtcomp->GetTag<NBT_Tag>(luaL_checkstring(L, 2)));
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = dynamic_cast<NBT_TagList *>(nbt);
        size_t index = luaL_checkint(L, 2);
        Lua_NewNBT(L, nbtlist->values[index]);
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        size_t index = luaL_checkint(L, 2);
        NBT_TagByteArray * nbtba = checkNBT_ByteArray(L, 1);
        luaL_argcheck(L, 0 <= index && index < nbtba->value.size(), 2, "invalid index");
        lua_pushnumber(L, nbtba->value[index]);
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a compound, byte array, or list");
    }
    return 1;
}

static int Lua_NBT_newindex(lua_State * L)
{
    // Indices are 0-based.
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        // Insert or replace a NBT
        // TODO: key-value variant?
        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
        nbtcomp->SetTag(checkNBT(L, 2));
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        // Replace a NBT
        // TODO: if setting values is allowed to create entries, set type from type of first entry added
        NBT_TagList * nbtlist = checkNBT_List(L, 1);
        size_t index = luaL_checkint(L, 2);
        NBT_Tag * value = checkNBT(L, 3);
        luaL_argcheck(L, 0 <= index && index < nbtlist->values.size(), 2, "invalid index");
        luaL_argcheck(L, value->Type() == nbtlist->ValueType(), 3, "inserted item doesn't match list type");
        nbtlist->values[index] = value;
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        // Set a byte value
        NBT_TagByteArray * nbtba = checkNBT_ByteArray(L, 1);
        size_t index = luaL_checkint(L, 2);
        int value = luaL_checkint(L, 3);
        luaL_argcheck(L, 0 <= index && index < nbtba->value.size(), 2, "invalid index");
        nbtba->value[index] = value;
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a compound, byte array, or list");
    }
    return 0;
}

static int Lua_NBT_push_back(lua_State * L)
{
    // Indices are 0-based.
    NBT_TagList * nbtlist = checkNBT_List(L, 1);
    NBT_Tag * value = checkNBT(L, 2);
    luaL_argcheck(L, value->Type() == nbtlist->ValueType(), 3, "inserted item doesn't match list type");
    nbtlist->values.push_back(value);
    return 0;
}

static int Lua_NBT_erase(lua_State * L)
{
    // Indices are 0-based.
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_Compound)
    {
//        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
        //nbtcomp->SetTag(checkNBT(L, 2));
        luaL_argcheck(L, false, 1, "erasing compound members not yet implemented");
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        // Replace a NBT
        // TODO: if setting values is allowed to create entries, set type from type of first entry added
        NBT_TagList * nbtlist = checkNBT_List(L, 1);
        size_t index = luaL_checkint(L, 2);
        luaL_argcheck(L, 0 <= index && index < nbtlist->values.size(), 2, "invalid index");
        delete nbtlist->values[index];
        nbtlist->values.erase(nbtlist->values.begin() + index);
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a compound or list");
    }
    return 0;
}

static int Lua_NBT_resize(lua_State * L)
{
    // Indices are 0-based.
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = checkNBT_List(L, 1);
        size_t size = luaL_checkint(L, 2);
        if(size < nbtlist->values.size())
        {
            // delete elements if shrinking
            for(size_t j = size - 1; j < nbtlist->values.size(); ++j)
                delete nbtlist->values[j];
        }
        nbtlist->values.resize(size, NULL);
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        // Set a byte value
        NBT_TagByteArray * nbtba = checkNBT_ByteArray(L, 1);
        size_t size = luaL_checkint(L, 2);
        int value = luaL_checkint(L, 3);
        nbtba->value.resize(size, value);
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a list or byte array");
    }
    return 0;
}

static int Lua_NBT_len(lua_State * L)
{
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        NBT_TagList * nbtlist = dynamic_cast<NBT_TagList *>(nbt);
        lua_pushinteger(L, nbtlist->values.size());
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = dynamic_cast<NBT_TagList *>(nbt);
        lua_pushinteger(L, nbtlist->values.size());
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        lua_pushinteger(L, checkNBT_ByteArray(L, 1)->value.size());
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a composite, byte array, or list tag");
    }
    return 1;
}

static int Lua_NBT_names(lua_State * L)
{
    // Return a table of names for a composite tag
    // TODO: implement iterator
    NBT_Tag * nbt = checkNBT(L, 1);
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
        lua_newtable(L);
        for(size_t i = 0; i < nbtcomp->tags.size(); ++i) {
            lua_pushnumber(L, i + 1);
            lua_pushstring(L, nbtcomp->tags[i]->name.c_str());
            lua_settable(L, -3);
        }
    }
    else
    {
        luaL_argcheck(L, false, 1, "NBT must be a composite tag");
    }
    return 1;
}


//******************************************************************************
// Miscellaneous functions
//******************************************************************************

static int Lua_LoadNBT(lua_State * L)
{
    // TODO: return a NBT_Root or something garbage collectable...
    string nbtpath = luaL_checkstring(L, 1);
	Lua_NewNBT(L, LoadNBT_File(nbtpath));
    return 1;
}

static int Lua_DumpNBT(lua_State * L)
{
    NBT_Tag * nbt = checkNBT(L, 1);
//	printf("Dumping NBT object %p\n", nbt);
    nbt->Print(std::cout);
    return 0;
}

static int Lua_NBT_tostring(lua_State * L)
{
    NBT_Tag * nbt = checkNBT(L, 1);
    ostringstream dmp;
    nbt->Print(dmp);
    lua_pushstring(L, dmp.str().c_str());
    return 1;
}

static int Lua_WriteNBT(lua_State * L)
{
    NBT_Tag * nbt = checkNBT(L, 1);
    string outputFilePath = luaL_checkstring(L, 2);
    
    gzFile fout = gzopen(outputFilePath.c_str(), "wb");
    if(!fout) {
        cerr << "Could not open \"" << outputFilePath << "\"" << endl;
        int errnum;
        cerr << "ZLib error: \"" << gzerror(fout, &errnum) << "\"" << endl;
        gzclose(fout);
        abort();
    }
    nbt->Write(fout);
    gzclose(fout);
    return 0;
}

static int Lua_GetNBT_Type(lua_State * L) {
    NBT_Tag * nbt = checkNBT(L, 1);
    lua_pushinteger(L, nbt->Type());
    return 1;
}

static int Lua_GetNBT_Name(lua_State * L) {
    NBT_Tag * nbt = checkNBT(L, 1);
    lua_pushstring(L, nbt->name.c_str());
    return 1;
}

static int Lua_run_script(lua_State * L)
{
    string scriptName = luaL_checkstring(L, 1);
    NBT_RunLuaScript(scriptName);
    return 0;
}
//******************************************************************************

// TODO:
// iteration for byte arrays, lists
// iterate over elements of compound values
// byte array resizing
// bindings for creation/deletion/copying operations
// Copy operation, create/insert NBT objects, garbage collection
// make [] syntax work?

// Use luaL_checkany() to implement variable parameter numbers...

static const struct luaL_reg nbt_classmthds[] = {
    {"load", Lua_LoadNBT},
    {"new_compound", Lua_NewNBT_Compound},
    {"new_list", Lua_NewNBT_List},
    {"new_byte", Lua_NewNBT_Byte},
    {"new_short", Lua_NewNBT_Short},
    {"new_int", Lua_NewNBT_Integer},
    {"new_long", Lua_NewNBT_Long},
    {"new_float", Lua_NewNBT_Float},
    {"new_double", Lua_NewNBT_Double},
    {"new_string", Lua_NewNBT_String},
    {"new_byte_array", Lua_NewNBT_ByteArray},
    {NULL, NULL}
};
static const struct luaL_reg nbt_mthds[] = {
    {"__tostring", Lua_NBT_tostring},
    {"dump", Lua_DumpNBT},
    {"write", Lua_WriteNBT},
    {"get", Lua_NBT_index},
    {"set", Lua_NBT_newindex},
    {"push_back", Lua_NBT_push_back},
    {"erase", Lua_NBT_erase},
    {"resize", Lua_NBT_resize},
    {"size", Lua_NBT_len},
    {"type", Lua_GetNBT_Type},
    {"name", Lua_GetNBT_Name},
    {"names", Lua_NBT_names},
//    {"__index", Lua_NBT_index},
//    {"__newindex", Lua_NBT_newindex},
    {"val", Lua_GetNBT_Val},
    {"get_byte", Lua_GetNBT_ByteVal},// type-specific value extraction
    {"set_byte", Lua_SetNBT_ByteVal},
    {"get_short", Lua_GetNBT_ShortVal},
    {"set_short", Lua_SetNBT_ShortVal},
    {"get_int", Lua_GetNBT_IntVal},
    {"set_int", Lua_SetNBT_IntVal},
    {"get_long", Lua_GetNBT_LongVal},
    {"set_long", Lua_SetNBT_LongVal},
    {"get_float", Lua_GetNBT_FloatVal},
    {"set_float", Lua_SetNBT_FloatVal},
    {"get_double", Lua_GetNBT_DoubleVal},
    {"set_double", Lua_SetNBT_DoubleVal},
    {"get_string", Lua_GetNBT_StringVal},
    {"set_string", Lua_SetNBT_StringVal},
    {NULL, NULL}
};

int Lua_InitLuaNBT(lua_State * L) {
    luaL_newmetatable(L, "MC.NBT");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); // pushes the metatable
    lua_settable(L, -3);  // metatable.__index = metatable
    luaL_openlib(L, NULL, nbt_mthds, 0);
    luaL_openlib(L, "NBT", nbt_classmthds, 0);
    
    lua_pushinteger(L, kNBT_TAG_End);
    lua_setglobal(L, "NBT.TAG.End");
    lua_pushinteger(L, kNBT_TAG_Byte);
    lua_setglobal(L, "NBT.TAG.Byte");
    lua_pushinteger(L, kNBT_TAG_Short);
    lua_setglobal(L, "NBT.TAG.Short");
    lua_pushinteger(L, kNBT_TAG_Int);
    lua_setglobal(L, "NBT.TAG.Int");
    lua_pushinteger(L, kNBT_TAG_Long);
    lua_setglobal(L, "NBT.TAG.Long");
    lua_pushinteger(L, kNBT_TAG_Float);
    lua_setglobal(L, "NBT.TAG.Float");
    lua_pushinteger(L, kNBT_TAG_Double);
    lua_setglobal(L, "NBT.TAG.Double");
    lua_pushinteger(L, kNBT_TAG_Byte_Array);
    lua_setglobal(L, "NBT.TAG.Byte_Array");
    lua_pushinteger(L, kNBT_TAG_String);
    lua_setglobal(L, "NBT.TAG.String");
    lua_pushinteger(L, kNBT_TAG_List);
    lua_setglobal(L, "NBT.TAG.List");
    lua_pushinteger(L, kNBT_TAG_Compound);
    lua_setglobal(L, "NBT.TAG.Compound");
    
    return 1;
}


void NBT_CloseLua()
{
	if(lua)
		lua_close(lua);
}

lua_State * NBT_InitLua()
{
	if(lua)
		return lua;
	
	lua = lua_open();
	if(!lua) {
		printf("Could not open Lua interpreter\n");
		abort();
	}
	
	atexit(NBT_CloseLua);
	
	luaL_openlibs(lua);
	
	lua_register(lua, "run_script", Lua_run_script);
	
    Lua_InitLuaNBT(lua);
    return lua;
}

void NBT_RunLuaScript(const std::string & scriptName)
{
    string path = scriptName;
    
    struct stat stbuf;
    if(stat(path.c_str(), &stbuf) != 0)
    {
        // Couldn't find file, look in magellan directory
        path = MCPath() + "/magellan/scripts/" + scriptName;
//        cerr << scriptName << " doesn't exist, trying " << path << endl;
        
        // If not there, try adding an extension.
        if(stat(path.c_str(), &stbuf) != 0) {
//            cerr << path << " doesn't exist, trying " << (path + ".lua") << endl;
            path = path + ".lua";
        }
    }
    
    if(stat(path.c_str(), &stbuf) != 0)
    {
        // Still can't find it. Maybe Lua can do better?
        path = scriptName;
    }
    
//	printf("Running file: %s\n", scriptPath.c_str());
	if(luaL_loadfile(lua, path.c_str()) != 0) {
        cerr << "Could not load file: " << path << endl;
        cerr << "Error: " << lua_tostring(lua, -1) << endl;
        exit(EXIT_FAILURE);
    }
    if(lua_pcall(lua, 0, LUA_MULTRET, 0) != 0) {
        cerr << "Could not run file: " << path << endl;
        cerr << "Error: " << lua_tostring(lua, -1) << endl;
        exit(EXIT_FAILURE);
    }
}

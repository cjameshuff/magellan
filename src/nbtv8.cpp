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


#include "nbtv8.h"

#include <iostream>
#include <string>
#include <sstream>

#include <sys/stat.h>
#include <errno.h>

#include "misc.h"

#include "nbt.h"

using namespace std;
using namespace v8;

// http://create.tpsitulsa.com/blog/2009/01/29/v8-objects/

static Persistent<FunctionTemplate> nbtCons;


//******************************************************************************

// All simple value types may be constructed with:
// new_value(name, value), new_byte_array(value)
static Handle<Value> NBT_newByte(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        int val = args[1]->IntegerValue();
        return WrapNBT(new NBT_TagByte(tagName, val));
    }
    else {
        int val = args[0]->IntegerValue();
        return WrapNBT(new NBT_TagByte("", val));
    }
}

static Handle<Value> NBT_newShort(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        int val = args[1]->IntegerValue();
        return WrapNBT(new NBT_TagShort(tagName, val));
    }
    else {
        int val = args[0]->IntegerValue();
        return WrapNBT(new NBT_TagShort("", val));
    }
}

static Handle<Value> NBT_newInteger(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        int32_t val = args[1]->IntegerValue();
        return WrapNBT(new NBT_TagInt(tagName, val));
    }
    else {
        int32_t val = args[0]->IntegerValue();
        return WrapNBT(new NBT_TagInt("", val));
    }
}

// TODO: figure out proper support for long integers
static Handle<Value> NBT_newLong(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        int64_t val = args[1]->IntegerValue();
        return WrapNBT(new NBT_TagLong(tagName, val));
    }
    else {
        int64_t val = args[0]->IntegerValue();
        return WrapNBT(new NBT_TagLong("", val));
    }
}

static Handle<Value> NBT_newFloat(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        float val = args[1]->NumberValue();
        return WrapNBT(new NBT_TagFloat(tagName, val));
    }
    else {
        float val = args[0]->NumberValue();
        return WrapNBT(new NBT_TagFloat("", val));
    }
}

static Handle<Value> NBT_newDouble(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        float val = args[1]->NumberValue();
        return WrapNBT(new NBT_TagDouble(tagName, val));
    }
    else {
        float val = args[0]->NumberValue();
        return WrapNBT(new NBT_TagDouble("", val));
    }
}

static Handle<Value> NBT_newString(const Arguments & args) {
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    if(args.Length() == 2) {
        string tagName = StringValue(args[0]);
        string val = StringValue(args[1]);
        return WrapNBT(new NBT_TagString(tagName, val));
    }
    else {
        string val = StringValue(args[0]);
        return WrapNBT(new NBT_TagString("", val));
    }
}

static Handle<Value> NBT_newByteArray(const Arguments & args) {
    // new_byte_array(name, size), new_byte_array(size)
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    NBT_TagByteArray * nbt = new NBT_TagByteArray();
    if(args.Length() == 2) {
        nbt->name = StringValue(args[0]);
        nbt->value.resize(args[1]->IntegerValue());
    }
    else {
        nbt->value.resize(args[0]->IntegerValue());
    }
    return WrapNBT(nbt);
}

static Handle<Value> NBT_newCompound(const Arguments & args) {
    // new_compound(name), new_compound()
    V8_ReturnErrorIf(args.Length() > 1, "Bad parameters");
    NBT_TagCompound * nbt = new NBT_TagCompound();
    if(args.Length() == 1)
        nbt->name = StringValue(args[0]);
    return WrapNBT(nbt);
}

static Handle<Value> NBT_newList(const Arguments & args) {
    // new_list(name, type[, length]), new_list(type[, length])
    // TODO: implement length
    V8_ReturnErrorIf(args.Length() > 3, "Bad parameters");
    nbt_tag_t vt = kNBT_TAG_End;
    size_t length = 0;
    string name;
    if(args.Length() == 1) {
        vt = (nbt_tag_t)args[0]->IntegerValue();
    }
    else if(args.Length() == 2) {
        // either (name, type) or (type, length)
        if(args[0]->IsNumber())
        {
            // (type, length)
            vt = (nbt_tag_t)args[0]->IntegerValue();
            length = args[1]->IntegerValue();
        }
        else {
            // (name, type)
            name = StringValue(args[0]);
            vt = (nbt_tag_t)args[1]->IntegerValue();
        }
    }
    else {// (name, type, length)
        name = StringValue(args[0]);
        vt = (nbt_tag_t)args[1]->IntegerValue();
        length = args[2]->IntegerValue();
    }
    NBT_TagList * nbt = new NBT_TagList(vt);
    nbt->name = name;
    return WrapNBT(nbt);
}

//******************************************************************************



static Handle<Value> NBT_GetValue(Local<String> property, const AccessorInfo & info) {
    Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
    NBT_Tag * nbt = static_cast<NBT_Tag *>(wrap->Value());
    switch(nbt->Type()) {
        case kNBT_TAG_End: return Undefined(); break;
        case kNBT_TAG_Byte: return Integer::New(static_cast<NBT_TagByte *>(nbt)->value); break;
        case kNBT_TAG_Short: return Integer::New(static_cast<NBT_TagShort *>(nbt)->value); break;
        case kNBT_TAG_Int: return Integer::New(static_cast<NBT_TagInt *>(nbt)->value); break;
        case kNBT_TAG_Long: return Integer::New(static_cast<NBT_TagLong *>(nbt)->value); break;
        case kNBT_TAG_Float: return Number::New(static_cast<NBT_TagFloat *>(nbt)->value); break;
        case kNBT_TAG_Double: return Number::New(static_cast<NBT_TagDouble *>(nbt)->value); break;
        case kNBT_TAG_Byte_Array: return Undefined(); break; // TODO: something more useful
        case kNBT_TAG_String:
            return String::New(static_cast<NBT_TagString *>(nbt)->value.c_str());
        break;
        case kNBT_TAG_List: return Undefined(); break; // TODO: something more useful
        case kNBT_TAG_Compound: return Undefined(); break; // TODO: something more useful
        case kNBT_NumTagTypes: return Undefined(); break; // TODO: something more useful
    }
    return Undefined();
}

static void NBT_SetValue(Local<String> property, Local<Value> value, const AccessorInfo & info) {
    Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
    NBT_Tag * nbt = static_cast<NBT_Tag *>(wrap->Value());
    switch(nbt->Type()) {
        case kNBT_TAG_End: break;
        case kNBT_TAG_Byte:
            static_cast<NBT_TagByte *>(nbt)->value = value->IntegerValue();
        break;
        case kNBT_TAG_Short:
            static_cast<NBT_TagShort *>(nbt)->value = value->IntegerValue();
        break;
        case kNBT_TAG_Int:
            static_cast<NBT_TagInt *>(nbt)->value = value->IntegerValue();
        break;
        case kNBT_TAG_Long:
            static_cast<NBT_TagLong *>(nbt)->value = value->IntegerValue();
        break;
        case kNBT_TAG_Float:
            static_cast<NBT_TagFloat *>(nbt)->value = value->NumberValue();
        break;
        case kNBT_TAG_Double:
            static_cast<NBT_TagDouble *>(nbt)->value = value->NumberValue();
        break;
        case kNBT_TAG_Byte_Array: break; // TODO: something more useful
        case kNBT_TAG_String:
            static_cast<NBT_TagString *>(nbt)->value = 
                *String::Utf8Value(value->ToString());
        break;
        case kNBT_TAG_List: break; // TODO: something more useful
        case kNBT_TAG_Compound: break; // TODO: something more useful
        case kNBT_NumTagTypes: break; // TODO: something more useful
    }
}


// "index()" and "newindex()" were originally overrides for the [] operator in
// Lua. index() is a straightforward lookup, newindex() is an assignment.
static Handle<Value> NBT_index(const Arguments & args)
{
    V8_ReturnErrorIf(args.Length() != 1, "Bad parameters");
    Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
    NBT_Tag * nbt = static_cast<NBT_Tag *>(wrap->Value());
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        // TODO: allow indexing compound by number, for iteration purposes?
        NBT_TagCompound * nbtcomp = static_cast<NBT_TagCompound *>(nbt);
        string key = StringValue(args[0]);
        return WrapNBT(nbtcomp->GetTag<NBT_Tag>(key));
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = static_cast<NBT_TagList *>(nbt);
        size_t index = args[0]->IntegerValue();
        return WrapNBT(nbtlist->values[index]);
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        NBT_TagByteArray * nbtba = static_cast<NBT_TagByteArray *>(nbt);
        int index = args[0]->IntegerValue();
        if(index < 0 || index >= (int)nbtba->value.size())
            V8_ReturnError("invalid index");
        return Integer::New(nbtba->value[index]);
    }
    V8_ReturnError("Can't index this type");
}

static Handle<Value> NBT_newindex(const Arguments & args)
{
    V8_ReturnErrorIf(args.Length() != 1 && args.Length() != 2, "Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        // Insert or replace a NBT
        NBT_TagCompound * nbtcomp = static_cast<NBT_TagCompound *>(nbt);
        if(args.Length() == 2) {
            NBT_Tag * value = ExternVal<NBT_Tag>(args[1]);
            value->name = StringValue(args[0]);
            nbtcomp->SetTag(value);
        }
        else {
            nbtcomp->SetTag(ExternVal<NBT_Tag>(args[0]));
        }
        return Undefined();
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        // Replace a NBT
        // TODO: if setting values is allowed to create entries,
        // set type from type of first entry added
        NBT_TagList * nbtlist = static_cast<NBT_TagList *>(nbt);
        int index = args[0]->IntegerValue();
        NBT_Tag * value = ExternVal<NBT_Tag>(args[1]);
        if(index < 0 || index >= (int)nbtlist->values.size())
            V8_ReturnError("invalid index");
        if(value->Type() != nbtlist->ValueType()) {
            ostringstream os;
            os << "Inserted item doesn't match list type. Item type: "
               << value->Type() << ", list type: " << nbtlist->ValueType();
            V8_ReturnError(os.str().c_str());
        }
        
        if(nbtlist->values[index] != NULL)
            delete nbtlist->values[index];
        nbtlist->values[index] = value;
        return Undefined();
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        // Set a byte value
        NBT_TagByteArray * nbtba = static_cast<NBT_TagByteArray *>(nbt);
        int index = args[0]->IntegerValue();
        int value = args[1]->IntegerValue();
        if(index < 0 || index >= (int)nbtba->value.size())
            V8_ReturnError("invalid index");
        nbtba->value[index] = value;
        return Undefined();
    }
    V8_ReturnError("Can't index this type");
}


// Add an item to the end of a list
static Handle<Value> NBT_push_back(const Arguments & args)
{
    V8_ReturnErrorIf(args.Length() != 1, "Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    
    if(nbt->Type() != kNBT_TAG_List)
        V8_ReturnError("push_back() only valid for list tags");
    
    NBT_TagList * nbtlist = static_cast<NBT_TagList *>(nbt);
    NBT_Tag * value = ExternVal<NBT_Tag>(args[0]);
    if(value) {
        // Initialize valueType of empty list from first inserted value
        if(nbtlist->values.size() == 0)
            nbtlist->valueType = value->Type();
        else if(value->Type() != nbtlist->ValueType()) {
            ostringstream os;
            os << "Inserted item doesn't match list type. Item type: "
               << value->Type() << ", list type: " << nbtlist->ValueType();
            V8_ReturnError(os.str().c_str());
        }
        nbtlist->values.push_back(value);
    }
    else {
        V8_ReturnError("Attempted to insert null value into NBT_TagList");
    }
    return Undefined();
}

static Handle<Value> NBT_erase(const Arguments & args)
{
    V8_ReturnErrorIf(args.Length() != 1, "Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    
    if(nbt->Type() == kNBT_TAG_Compound)
    {
//        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
        //nbtcomp->SetTag(checkNBT(L, 2));
        V8_ReturnError("erasing compound members not yet implemented");
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        // erase a NBT list element
        // TODO: if setting values is allowed to create entries,
        // set type from type of first entry added
        NBT_TagList * nbtlist = static_cast<NBT_TagList *>(nbt);
        int index = args[0]->IntegerValue();
        if(index < 0 || index >= (int)nbtlist->values.size())
            V8_ReturnError("invalid index");
        delete nbtlist->values[index];
        nbtlist->values.erase(nbtlist->values.begin() + index);
    }
    else
    {
        V8_ReturnError("NBT must be a compound or list");
    }
    return Undefined();
}

static Handle<Value> NBT_resize(const Arguments & args)
{
    if(args.Length() != 1 && args.Length() != 2)
        V8_ReturnError("Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    
    if(nbt->Type() == kNBT_TAG_List)
    {
        if(args.Length() != 1)
            V8_ReturnError("Bad parameters");
        NBT_TagList * nbtlist = static_cast<NBT_TagList *>(nbt);
        int size = args[0]->IntegerValue();
        if(size < (int)nbtlist->values.size()) {
            // delete elements if shrinking
            for(size_t j = size - 1; j < nbtlist->values.size(); ++j)
                delete nbtlist->values[j];
        }
        nbtlist->values.resize(size, NULL);
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        if(args.Length() != 2)
            V8_ReturnError("Bad parameters");
        // Set a byte value
        NBT_TagByteArray * nbtba = static_cast<NBT_TagByteArray *>(nbt);
        int size = args[0]->IntegerValue();
        int value = args[1]->IntegerValue();
        nbtba->value.resize(size, value);
    }
    else
    {
        V8_ReturnError("NBT must be a list or byte array");
    }
    return Undefined();
}

static Handle<Value> NBT_size(const Arguments & args)
{
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        NBT_TagCompound * nbtcomp = dynamic_cast<NBT_TagCompound *>(nbt);
        return Integer::New(nbtcomp->tagsByName.size());
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = dynamic_cast<NBT_TagList *>(nbt);
        return Integer::New(nbtlist->values.size());
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        NBT_TagByteArray * nbtba = static_cast<NBT_TagByteArray *>(nbt);
        return Integer::New(nbtba->value.size());
    }
    else
    {
        V8_ReturnError("NBT must be a composite, byte array, or list tag");
    }
    return Undefined();
}


static Handle<Value> NBT_GetContents(Local<String> property, const AccessorInfo & info) {
    Local<External> wrap = Local<External>::Cast(info.Holder()->GetInternalField(0));
    NBT_Tag * nbt = static_cast<NBT_Tag *>(wrap->Value());
    if(nbt->Type() == kNBT_TAG_Compound)
    {
        NBT_TagCompound * nbtcomp = dynamic_cast<NBT_TagCompound *>(nbt);
        v8::Handle<v8::Array> contents = v8::Array::New(nbtcomp->tagsByName.size());
        std::map<std::string, NBT_Tag *>::iterator tag;
        for(tag = nbtcomp->tagsByName.begin(); tag != nbtcomp->tagsByName.end(); ++tag)
            contents->Set(String::New(tag->first.c_str()), WrapNBT(tag->second));
        
        return contents;
    }
    else if(nbt->Type() == kNBT_TAG_List)
    {
        NBT_TagList * nbtlist = dynamic_cast<NBT_TagList *>(nbt);
        v8::Handle<v8::Array> contents = v8::Array::New(nbtlist->values.size());
        for(size_t j = 0; j < nbtlist->values.size(); ++j)
            contents->Set(Integer::New(j), WrapNBT(nbtlist->values[j]));
        return contents;
    }
    else if(nbt->Type() == kNBT_TAG_Byte_Array)
    {
        // Note that contents of byte arrays must be returned by value
        NBT_TagByteArray * nbtba = static_cast<NBT_TagByteArray *>(nbt);
        v8::Handle<v8::Array> contents = v8::Array::New(nbtba->value.size());
        for(size_t j = 0; j < nbtba->value.size(); ++j)
            contents->Set(Integer::New(j), Integer::New(nbtba->value[j]));
        return contents;
    }
    V8_ReturnError("NBT must be a composite, byte array, or list tag");
}

//static Handle<Value> NBT_names(const Arguments & args)
//static int Lua_NBT_names(lua_State * L)
//{
//    // Return a table of names for a composite tag
//    // TODO: implement iterator
//    NBT_Tag * nbt = checkNBT(L, 1);
//    if(nbt->Type() == kNBT_TAG_Compound)
//    {
//        NBT_TagCompound * nbtcomp = checkNBT_Compound(L, 1);
//        lua_newtable(L);
//        for(size_t i = 0; i < nbtcomp->tags.size(); ++i) {
//            lua_pushnumber(L, i + 1);
//            lua_pushstring(L, nbtcomp->tags[i]->name.c_str());
//            lua_settable(L, -3);
//        }
//    }
//    else
//    {
//        luaL_argcheck(L, false, 1, "NBT must be a composite tag");
//    }
//    return Undefined();
//}


//******************************************************************************
// Miscellaneous functions
//******************************************************************************
static Handle<Value> NBT_load(const Arguments & args)
{
    if(args.Length() != 1) V8_ReturnError("Bad parameters");
    string nbtpath = StringValue(args[0]);
//    cerr << "Loading \"" << nbtpath << "\"" << endl;
    return WrapNBT(LoadNBT_File(nbtpath));
}

// Frees memory used by the NBT structure. Should only be called on the root
// NBT, and references to the deleted NBT should not be used.
// TODO: more complete bindings, build a new data structure out of JS objects
// rather than wrapping individual parts of an existing NBT structure. This
// is just a quick hack to make it possible to manipulate lots of NBTs without
// causing massive memory leaks.
static Handle<Value> NBT_delete(const Arguments & args)
{
    if(args.Length() != 1) V8_ReturnError("Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args[0]);
    delete nbt;
    return Undefined();
}

static Handle<Value> NBT_dump(const Arguments & args)
{
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
//	printf("Dumping NBT object %p\n", nbt);
    nbt->Print(std::cout);
    return Undefined();
}


static Handle<Value> NBT_write(const Arguments & args)
{
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    string outputFilePath = StringValue(args[0]);
    
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
    return Undefined();
}

static Handle<Value> NBT_type(const Arguments & args) {
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    return Integer::New(nbt->Type());
}

static Handle<Value> NBT_name(const Arguments & args) {
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    return String::New(nbt->name.c_str());
}

static Handle<Value> RunScript(const Arguments & args)
{
    if(args.Length() != 1) V8_ReturnError("Bad parameters");
    string scriptName = StringValue(args[0]);
    RunMagellanScript(scriptName);
    return Undefined();
}
//******************************************************************************

void RunMagellanScript(const std::string & scriptName)
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
//            cerr << path << " doesn't exist, trying " << (path + ".js") << endl;
            path = path + ".js";
        }
    }
    
    if(stat(path.c_str(), &stbuf) != 0) {
        // Still can't find it. Just try the path as is.
        path = scriptName;
    }
    
//	printf("Running file: %s\n", path.c_str());
    ExecuteJS_File(path);
}

//******************************************************************************

static Handle<Value> NBT_toString(const Arguments & args)
{
    if(args.Length() != 0) V8_ReturnError("Bad parameters");
    NBT_Tag * nbt = ExternVal<NBT_Tag>(args.This());
    ostringstream os;
    nbt->Print(os);
    return String::New(os.str().c_str());
}

static void weakNBT_CB(Persistent<Value> obj, void * param)
{
    NBT_Tag * nbt = static_cast<NBT_Tag *>(param);
    delete nbt;
    obj.Dispose();
    obj.Clear();
}

static Handle<Value> ConstructNBT(const Arguments & args)
{
    // Note: not actually used to construct NBT objects from JS.
    HandleScope handle_scope;
    Handle<External> wrapper;
    
	if(!args.IsConstructCall()) V8_ReturnError("Cannot call constructor as function");
    if(args.Length() == 2 || args.Length() > 3) V8_ReturnError("Bad parameters");
    
    if(args[0]->IsExternal())
    {
        // Passed an external: called from C++
        // Does not own NBT_Tag (at this point) and does not delete it when collected.
        wrapper = Handle<External>::Cast(args[0]);
    }
    else
    {
        // Passed JS object(s)
        //NBT_Tag * nbt = ;
        
        //wrapper = External::New(nbt);*/
        
//        Persistent<Object> cleanerHandle = Persistent<Object>::New(args.This());
//        cleanerHandle.MakeWeak(nbt, weakNBT_CB);// NBT_Tag is owned by JS object
    }
    args.This()->SetInternalField(0, wrapper);
    return args.This();
}

// Wrap an existing NBT
Handle<Object> WrapNBT(NBT_Tag * nbt) {
    HandleScope handle_scope;
    Handle<Value> ext = External::New(nbt);
    Handle<Object> new_nbt = nbtCons->GetFunction()->NewInstance(1, &ext);
    return handle_scope.Close(new_nbt);
}

// Create a new NBT
// TODO
//Handle<Object> NewNBT(const NBT_Tag & nbt) {
//    HandleScope handle_scope;
//    NBT_Tag * nbt = ;
//    Handle<Value> ext = External::New(nbt);
//    Handle<Object> new_nbt = nbtCons->GetFunction()->NewInstance(1, &ext);
//    
//    Persistent<Object> cleanerHandle = Persistent<Object>::New(new_nbt);
//    cleanerHandle.MakeWeak(nbt, weakNBT_CB);// NBT_Tag is owned by JS object
//    return handle_scope.Close(new_nbt);
//}


void V8NBT_InitBindings(v8::Handle<v8::ObjectTemplate> & global)
{
    global->Set(String::New("run_script"), FunctionTemplate::New(RunScript));
    
    HandleScope handle_scope;
    Handle<FunctionTemplate> lnbtCons = FunctionTemplate::New(ConstructNBT);
    nbtCons = Persistent<FunctionTemplate>::New(lnbtCons);
    global->Set(String::New("NBT"), nbtCons);
    
    // Class methods
    lnbtCons->Set(String::New("load"), FunctionTemplate::New(NBT_load));
    lnbtCons->Set(String::New("delete"), FunctionTemplate::New(NBT_delete));
    
    lnbtCons->Set(String::New("new_compound"),   FunctionTemplate::New(NBT_newCompound));
    lnbtCons->Set(String::New("new_list"),       FunctionTemplate::New(NBT_newList));
    lnbtCons->Set(String::New("new_byte"),       FunctionTemplate::New(NBT_newByte));
    lnbtCons->Set(String::New("new_short"),      FunctionTemplate::New(NBT_newShort));
    lnbtCons->Set(String::New("new_integer"),    FunctionTemplate::New(NBT_newInteger));
    lnbtCons->Set(String::New("new_long"),       FunctionTemplate::New(NBT_newLong));
    lnbtCons->Set(String::New("new_float"),      FunctionTemplate::New(NBT_newFloat));
    lnbtCons->Set(String::New("new_double"),     FunctionTemplate::New(NBT_newDouble));
    lnbtCons->Set(String::New("new_string"),     FunctionTemplate::New(NBT_newString));
    lnbtCons->Set(String::New("new_byte_array"), FunctionTemplate::New(NBT_newByteArray));
    
    
    Handle<ObjectTemplate> nbtIT = lnbtCons->InstanceTemplate();
    nbtIT->SetInternalFieldCount(1);
    
    nbtIT->Set(String::New("toString"), FunctionTemplate::New(NBT_toString));
    
    nbtIT->Set(String::New("dump"), FunctionTemplate::New(NBT_dump));
    nbtIT->Set(String::New("write"), FunctionTemplate::New(NBT_write));
    
    // May be a better way to do these
    nbtIT->Set(String::New("TAG_End"),       Integer::New(kNBT_TAG_End));
    nbtIT->Set(String::New("TAG_Compound"),  Integer::New(kNBT_TAG_Compound));
    nbtIT->Set(String::New("TAG_List"),      Integer::New(kNBT_TAG_List));
    nbtIT->Set(String::New("TAG_ByteArray"), Integer::New(kNBT_TAG_Byte_Array));
    nbtIT->Set(String::New("TAG_Byte"),      Integer::New(kNBT_TAG_Byte));
    nbtIT->Set(String::New("TAG_Short"),     Integer::New(kNBT_TAG_Short));
    nbtIT->Set(String::New("TAG_Int"),       Integer::New(kNBT_TAG_Int));
    nbtIT->Set(String::New("TAG_Long"),      Integer::New(kNBT_TAG_Long));
    nbtIT->Set(String::New("TAG_Float"),     Integer::New(kNBT_TAG_Float));
    nbtIT->Set(String::New("TAG_Double"),    Integer::New(kNBT_TAG_Double));
    nbtIT->Set(String::New("TAG_String"),    Integer::New(kNBT_TAG_String));
    
    // TODO: make accessors for type, name, size...contents?
    nbtIT->Set(String::New("type"), FunctionTemplate::New(NBT_type));
    nbtIT->Set(String::New("name"), FunctionTemplate::New(NBT_name));
    
    
    nbtIT->SetAccessor(String::New("value"), NBT_GetValue, NBT_SetValue);
    nbtIT->SetAccessor(String::New("contents"), NBT_GetContents);
//    nbtIT->Set(String::New("contents"), FunctionTemplate::New(NBT_contents));
    
    nbtIT->Set(String::New("get"), FunctionTemplate::New(NBT_index));
    nbtIT->Set(String::New("set"), FunctionTemplate::New(NBT_newindex));
    nbtIT->Set(String::New("push_back"), FunctionTemplate::New(NBT_push_back));
    nbtIT->Set(String::New("erase"), FunctionTemplate::New(NBT_erase));
    nbtIT->Set(String::New("resize"), FunctionTemplate::New(NBT_resize));
    nbtIT->Set(String::New("size"), FunctionTemplate::New(NBT_size));
}

//******************************************************************************

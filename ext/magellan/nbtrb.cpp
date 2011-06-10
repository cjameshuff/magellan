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

#include "nbtrb.h"
#include "nbt.h"

#include <string>

#include <ruby.h>
// #include <ruby/intern.h>

using namespace std;

static VALUE class_NBT;
static VALUE mNBT;


static VALUE NBT_initialize(int argc, VALUE *argv, VALUE self);
static VALUE NBT_load(VALUE module, VALUE filePath);
static VALUE NBT_write(VALUE self, VALUE filePath);
static VALUE NBT_dump(VALUE self);


// The problem of getting proper interoperation between Ruby's garbage collection and
// the manual memory management of the NBT library is solved by not even trying.
// Instead, Ruby-side NBTs are simple pure-Ruby objects consisting of an integer NBT
// type and a value that can be numeric, an array, a string, a hash, etc...
static VALUE NBT_ByteToValue(NBT_TagByte * comp);
static VALUE NBT_ShortToValue(NBT_TagShort * comp);
static VALUE NBT_IntToValue(NBT_TagInt * comp);
static VALUE NBT_LongToValue(NBT_TagLong * comp);
static VALUE NBT_FloatToValue(NBT_TagFloat * comp);
static VALUE NBT_DoubleToValue(NBT_TagDouble * comp);
static VALUE NBT_ByteArrayToValue(NBT_TagByteArray * comp);
static VALUE NBT_StringToValue(NBT_TagString * comp);
static VALUE NBT_ListToValue(NBT_TagList * comp);

VALUE CStrToSym(const char * str) {return ID2SYM(rb_intern(str));}
VALUE CStrToSym(const string & str) {return ID2SYM(rb_intern(str.c_str()));}

VALUE NBT_CompoundToValue(NBT_TagCompound * comp)
{
    // A compound is given a hash value
    VALUE contents = rb_hash_new();
    std::vector<NBT_Tag *>::iterator tagi;
    for(tagi = comp->tags.begin(); tagi != comp->tags.end(); ++tagi)
    {
        NBT_Tag * tag = *tagi;
        switch(tag->Type())
        {
            case kNBT_TAG_End:// NULL...shouldn't get this in a compound, but ignore it
            break;
            case kNBT_TAG_Byte:       // int8_t
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_ByteToValue(static_cast<NBT_TagByte *>(tag)));
            break;
            case kNBT_TAG_Short:      // int16_t
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_ShortToValue(static_cast<NBT_TagShort *>(tag)));
            break;
            case kNBT_TAG_Int:        // int32_t
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_IntToValue(static_cast<NBT_TagInt *>(tag)));
            break;
            case kNBT_TAG_Long:       // int64_t
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_LongToValue(static_cast<NBT_TagLong *>(tag)));
            break;
            case kNBT_TAG_Float:      // float
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_FloatToValue(static_cast<NBT_TagFloat *>(tag)));
            break;
            case kNBT_TAG_Double:     // double
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_DoubleToValue(static_cast<NBT_TagDouble *>(tag)));
            break;
            case kNBT_TAG_Byte_Array: // vector<int8_t> *
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_ByteArrayToValue(static_cast<NBT_TagByteArray *>(tag)));
            break;
            case kNBT_TAG_String:     // string *
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_StringToValue(static_cast<NBT_TagString *>(tag)));
            break;
            case kNBT_TAG_List:       // vector<NBT_Tag> *
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_ListToValue(static_cast<NBT_TagList *>(tag)));
            break;
            case kNBT_TAG_Compound:   // vector<NBT_Tag> *
                rb_hash_aset(contents, CStrToSym(tag->name), NBT_CompoundToValue(static_cast<NBT_TagCompound *>(tag)));
            break;
            default:
                // TODO: catch this and delete the tree
                rb_raise(rb_eArgError, "Bad NBT tree");
        }
    }
    
    VALUE argv[] = {rb_str_new2(comp->name.c_str()), INT2FIX(kNBT_TAG_Compound), contents};
    return rb_class_new_instance(3, argv, class_NBT);
}

static VALUE NBT_ByteToValue(NBT_TagByte * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Byte), INT2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}
static VALUE NBT_ShortToValue(NBT_TagShort * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Short), INT2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}
static VALUE NBT_IntToValue(NBT_TagInt * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Int), LONG2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}
static VALUE NBT_LongToValue(NBT_TagLong * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Long), LONG2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}

static VALUE NBT_FloatToValue(NBT_TagFloat * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Float), DBL2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}
static VALUE NBT_DoubleToValue(NBT_TagDouble * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Double), DBL2NUM(tag->value)};
    return rb_class_new_instance(3, argv, class_NBT);
}

static VALUE NBT_ByteArrayToValue(NBT_TagByteArray * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_Byte_Array), rb_str_new((char *)&tag->value[0], tag->value.size())};
    return rb_class_new_instance(3, argv, class_NBT);
}
static VALUE NBT_StringToValue(NBT_TagString * tag)
{
    VALUE argv[] = {rb_str_new2(tag->name.c_str()), INT2FIX(kNBT_TAG_String), rb_str_new2(tag->value.c_str())};
    return rb_class_new_instance(3, argv, class_NBT);
}

static VALUE NBT_ListToValue(NBT_TagList * lst)
{
    // With all the overhead of this binding, there's little need to bother with
    // the optimization reusing a single type value. Members of the list array are
    // full, typed NBT objects.
    
    VALUE contents = rb_ary_new();
    std::vector<NBT_Tag *>::iterator tag;
    switch(lst->ValueType())
    {
        case kNBT_TAG_End:// NULL...shouldn't get this, but ignore it
        break;
        case kNBT_TAG_Byte:       // int8_t
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_ByteToValue(static_cast<NBT_TagByte *>(*tag)));
        break;
        case kNBT_TAG_Short:      // int16_t
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_ShortToValue(static_cast<NBT_TagShort *>(*tag)));
        break;
        case kNBT_TAG_Int:        // int32_t
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_IntToValue(static_cast<NBT_TagInt *>(*tag)));
        break;
        case kNBT_TAG_Long:       // int64_t
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_LongToValue(static_cast<NBT_TagLong *>(*tag)));
        break;
        case kNBT_TAG_Float:      // float
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_FloatToValue(static_cast<NBT_TagFloat *>(*tag)));
        break;
        case kNBT_TAG_Double:     // double
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_DoubleToValue(static_cast<NBT_TagDouble *>(*tag)));
        break;
        case kNBT_TAG_Byte_Array: // vector<int8_t> *
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_ByteArrayToValue(static_cast<NBT_TagByteArray *>(*tag)));
        break;
        case kNBT_TAG_String:     // string *
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_StringToValue(static_cast<NBT_TagString *>(*tag)));
        break;
        case kNBT_TAG_List:       // vector<NBT_Tag> *
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_ListToValue(static_cast<NBT_TagList *>(*tag)));
        break;
        case kNBT_TAG_Compound:   // vector<NBT_Tag> *
            for(tag = lst->values.begin(); tag != lst->values.end(); ++tag)
                rb_ary_push(contents, NBT_CompoundToValue(static_cast<NBT_TagCompound *>(*tag)));
        break;
        default:
            // TODO: catch this and delete the tree
            rb_raise(rb_eArgError, "Bad NBT tree");
    }
    
    VALUE argv[] = {rb_str_new2(lst->name.c_str()), INT2FIX(kNBT_TAG_List), contents, INT2FIX(lst->ValueType())};
    return rb_class_new_instance(4, argv, class_NBT);
}

static int CompoundMemberToNBT_CB(VALUE key, VALUE value, VALUE nbt) {
    NBT_TagCompound * nbtcpd = (NBT_TagCompound *)nbt;
    nbtcpd->AddTag(ValueToNBT(value));
    return ST_CONTINUE;
}

NBT_Tag * ValueToNBT(VALUE rbvalue)
{
    static ID id_next = rb_intern("next");
    static ID id_each = rb_intern("each");
    NBT_Tag * nbt;
    int type = NUM2INT(rb_iv_get(rbvalue, "@type"));
    VALUE rbtagval = rb_iv_get(rbvalue, "@value");
    VALUE rbtagname = rb_iv_get(rbvalue, "@name");
    string name = StringValuePtr(rbtagname);
    switch(type)
    {
        case kNBT_TAG_Byte:      // int8_t
            nbt = new NBT_TagByte(name, NUM2INT(rbtagval));
        break;
        case kNBT_TAG_Short:      // int16_t
            nbt = new NBT_TagShort(name, NUM2INT(rbtagval));
        break;
        case kNBT_TAG_Int:        // int32_t
            nbt = new NBT_TagInt(name, NUM2INT(rbtagval));
        break;
        case kNBT_TAG_Long:       // int64_t
            nbt = new NBT_TagLong(name, NUM2LONG(rbtagval));
        break;
        case kNBT_TAG_Float:      // float
            nbt = new NBT_TagFloat(name, (float)NUM2DBL(rbtagval));
        break;
        case kNBT_TAG_Double:     // double
            nbt = new NBT_TagDouble(name, NUM2DBL(rbtagval));
        break;
        case kNBT_TAG_Byte_Array: {// vector<int8_t> *
            NBT_TagByteArray * nbtba = new NBT_TagByteArray(name);
            nbt = nbtba;
            int n = RSTRING_LENINT(rbtagval);
            char * data = StringValuePtr(rbtagval);
            nbtba->value.resize(n);
            for(int j = 0; j < n; ++j)
                nbtba->value[j] = data[j];
        } break;
        case kNBT_TAG_String: {// string *
            NBT_TagString * nbtstr = new NBT_TagString(name, "");
            nbt = nbtstr;
            int n = RSTRING_LENINT(rbtagval);
            char * data = StringValuePtr(rbtagval);
            nbtstr->value.resize(n);
            for(int j = 0; j < n; ++j)
                nbtstr->value[j] = data[j];
        } break;
        case kNBT_TAG_List: {      // vector<NBT_Tag> *
            // value is an array of NBT objects.
            NBT_TagList * nbtlst = new NBT_TagList(name, (nbt_tag_t)NUM2INT(rb_iv_get(rbvalue, "@entry_type")));
            nbt = nbtlst;
            int n = RARRAY_LENINT(rbtagval);
            nbtlst->values.resize(n);
            for(int j = 0; j < n; ++j)
                nbtlst->values[j] = ValueToNBT(rb_ary_entry(rbtagval, j));
        } break;
        case kNBT_TAG_Compound: {  // vector<NBT_Tag> *
            NBT_TagCompound * nbtcpd = new NBT_TagCompound(name);
            nbt = nbtcpd;
            rb_hash_foreach(rbtagval, (int (*)(...))CompoundMemberToNBT_CB, (VALUE)nbtcpd);
        } break;
        default:
            // TODO: catch this and delete the tree
            rb_raise(rb_eArgError, "Bad NBT tree");
    }
    return nbt;
}

void Init_nbt()
{
    // mNBT = rb_define_module("NBT");
    
    class_NBT = rb_define_class("NBT", rb_cObject);
    rb_define_singleton_method(class_NBT, "load", RUBY_METHOD_FUNC(NBT_load), 1);
    rb_define_method(class_NBT, "initialize", RUBY_METHOD_FUNC(NBT_initialize), -1);
    rb_define_method(class_NBT, "write", RUBY_METHOD_FUNC(NBT_write), 1);
    rb_define_method(class_NBT, "dump", RUBY_METHOD_FUNC(NBT_dump), 0);
}


static VALUE NBT_initialize(int argc, VALUE *argv, VALUE self)
{
    if(argc < 3 || argc > 4)
    	rb_raise(rb_eArgError, "Expected 3 or 4 arguments");
    
    rb_iv_set(self, "@name", argv[0]);
    rb_iv_set(self, "@type", argv[1]);
    rb_iv_set(self, "@value", argv[2]);
    if(argc == 4)
        rb_iv_set(self, "@entry_type", argv[3]);
    return self;
}

static VALUE NBT_load(VALUE module, VALUE filePath)
{
    NBT_gzFile_I fin(StringValueCStr(filePath));
    NBT_TagCompound * nbt = LoadNBT_File(fin);
    VALUE rbnbt = NBT_CompoundToValue(nbt);
    delete nbt;
    return rbnbt;
}

static VALUE NBT_write(VALUE self, VALUE filePath)
{
    NBT_Tag * nbt = ValueToNBT(self);
    // Construct NBT tree
    
    string outputFilePath = StringValueCStr(filePath);
    NBT_gzFile_O fout(outputFilePath);
    // if(!fout) {
    //     rb_raise(rb_eArgError, "Could not write NBT file");
    // }
    nbt->Write(fout);
    delete nbt;
    return self;
}

static VALUE NBT_dump(VALUE self)
{
    NBT_Tag * nbt = ValueToNBT(self);
    nbt->Print(cout);
    delete nbt;
    return self;
}



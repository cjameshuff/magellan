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
//
// The most basic JS initialization and a handful of general-purpose additions.
// 
//******************************************************************************

#ifndef V8BASE_H
#define V8BASE_H

#include <v8.h>
#include <v8-testing.h>

#include <string>

v8::Handle<v8::ObjectTemplate> V8_InitBase();

bool ExecuteJS_File(const std::string & path);
bool ExecuteJS_File(const v8::Persistent<v8::Context> & context, const std::string & path);

template<typename T>
T * ExternVal(v8::Local<v8::Object> val) {
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(val->GetInternalField(0));
    return static_cast<T *>(wrap->Value());
}
template<typename T>
T * ExternVal(v8::Local<v8::Value> val) {
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
    return static_cast<T *>(wrap->Value());
}


static inline bool Has(v8::Handle<v8::Object> obj, const std::string & key) {
    v8::HandleScope handle_scope;
    return obj->Has(v8::String::New(key.c_str()));
}

static inline v8::Local<v8::Value> Get(v8::Handle<v8::Object> obj, const std::string & key) {
    v8::HandleScope handle_scope;
    v8::Local<v8::Value> val = obj->Get(v8::String::New(key.c_str()));
    return handle_scope.Close(val);
}

static inline bool TryGet(v8::Handle<v8::Object> obj, const std::string & keyStr,
                          v8::Local<v8::Value> & val)
{
    v8::HandleScope handle_scope;
    v8::Handle<v8::String> key = v8::String::New(keyStr.c_str());
    if(obj->Has(key)) {
        val = obj->Get(key);
        return true;
    }
    return false;
}

static inline std::string StringValue(v8::Handle<v8::Value> val) {
    return std::string(*v8::String::Utf8Value(val->ToString()));
}

//******************************************************************************
#endif // V8BASE_H

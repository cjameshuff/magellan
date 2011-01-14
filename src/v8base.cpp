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

#include "v8base.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>


using namespace v8;
using namespace std;

//******************************************************************************
Handle<Value> Print(const Arguments & args) {
    if(args.Length() != 1) return ThrowException(String::New("Bad parameters"));
    char * str = *String::Utf8Value(args[0]->ToString());
    printf("%s", str);
    return Undefined();
}
Handle<Value> PrintLn(const Arguments & args) {
    if(args.Length() != 1) return ThrowException(String::New("Bad parameters"));
    char * str = *String::Utf8Value(args[0]->ToString());
    printf("%s\n", str);
    return Undefined();
}

Handle<Value> Format(const Arguments & args) {
    if(args.Length() < 1) return ThrowException(String::New("Bad parameters"));
    string str = *String::Utf8Value(args[0]->ToString());
    ostringstream os;
    int arg = 1, ch = 0;
    while(ch < (int)str.length())
    {
        if(arg < args.Length() && str[ch] == '%' && str[ch + 1] == '@') {
            os << *String::Utf8Value(args[arg]->ToString());
            ch += 2;
            ++arg;
        }
        else {
            os << str[ch];
            ++ch;
        }
    }
    return String::New(os.str().c_str());
}
Handle<Value> PrintF(const Arguments & args) {
    if(args.Length() < 1) return ThrowException(String::New("Bad parameters"));
    char * str = *String::Utf8Value(Format(args)->ToString());
    printf("%s", str);
    return Undefined();
}

Handle<Value> Exit(const Arguments & args) {
    int retval = EXIT_SUCCESS;
    if(args.Length() == 1)
        retval = args[0]->IntegerValue();
    exit(retval);
    return Undefined();
}


Handle<Value> OS_open(const Arguments & args) {
    if(args.Length() < 1) return ThrowException(String::New("Bad parameters"));
    string path = *String::Utf8Value(args[0]->ToString());
    int flags = args[1]->IntegerValue();
    return Integer::New(open(path.c_str(), flags));
}

Handle<Value> OS_close(const Arguments & args) {
    if(args.Length() < 1) return ThrowException(String::New("Bad parameters"));
    int fd = args[0]->IntegerValue();
    return Integer::New(close(fd));
}

Handle<Value> OS_writestr(const Arguments & args) {
    if(args.Length() < 1) return ThrowException(String::New("Bad parameters"));
    int fd = args[0]->IntegerValue();
    string data = *String::Utf8Value(args[1]->ToString());
    return Integer::New(write(fd, data.c_str(), data.size()));
}

Handle<Value> OS_rename(const Arguments & args) {
    if(args.Length() != 2) return ThrowException(String::New("Bad parameters"));
    string oldPath = *String::Utf8Value(args[0]->ToString());
    string newPath = *String::Utf8Value(args[1]->ToString());
    return Integer::New(rename(oldPath.c_str(), newPath.c_str()));
}

Handle<ObjectTemplate> V8_InitBase()
{
    Local<ObjectTemplate> global = ObjectTemplate::New();
    
    global->Set(String::New("print"), FunctionTemplate::New(Print));
    global->Set(String::New("println"), FunctionTemplate::New(PrintLn));
    global->Set(String::New("printf"), FunctionTemplate::New(PrintF));
    global->Set(String::New("format"), FunctionTemplate::New(Format));
    
    Local<ObjectTemplate> os = ObjectTemplate::New();
    global->Set(String::New("os"), os);
    os->Set(String::New("exit"), FunctionTemplate::New(Exit));
    global->Set(String::New("exit"), FunctionTemplate::New(Exit));
    
    // might move variable definitions to a .js file
    os->Set(String::New("O_RDONLY"), Integer::New(O_RDONLY));
    os->Set(String::New("O_WRONLY"), Integer::New(O_WRONLY));
    os->Set(String::New("O_RDWR"), Integer::New(O_RDWR));
    os->Set(String::New("O_NONBLOCK"), Integer::New(O_NONBLOCK));
    os->Set(String::New("O_APPEND"), Integer::New(O_APPEND));
    os->Set(String::New("O_CREAT"), Integer::New(O_CREAT));
    os->Set(String::New("O_TRUNC"), Integer::New(O_TRUNC));
    os->Set(String::New("O_EXCL"), Integer::New(O_EXCL));
    os->Set(String::New("O_SHLOCK"), Integer::New(O_SHLOCK));
    os->Set(String::New("O_EXLOCK"), Integer::New(O_EXLOCK));
    os->Set(String::New("O_NOFOLLOW"), Integer::New(O_NOFOLLOW));
    os->Set(String::New("O_SYMLINK"), Integer::New(O_SYMLINK));
    os->Set(String::New("O_EVTONLY"), Integer::New(O_EVTONLY));
    
    os->Set(String::New("open"), FunctionTemplate::New(OS_open));
    os->Set(String::New("close"), FunctionTemplate::New(OS_close));
    os->Set(String::New("writestr"), FunctionTemplate::New(OS_writestr));
    os->Set(String::New("rename"), FunctionTemplate::New(OS_rename));
    os->Set(String::New("rename"), FunctionTemplate::New(OS_rename));
    
    return global;
}


// Some code from the V8 samples, need to rewrite
v8::Handle<v8::String> ReadFile(const char * name)
{
    FILE * file = fopen(name, "rb");
    if (file == NULL) return Handle<String>();
    
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    
    char * chars = new char[size + 1];
    chars[size] = '\0';
    for(int i = 0; i < size;) {
        int read = fread(&chars[i], 1, size - i, file);
        i += read;
    }
    fclose(file);
    Handle<String> result = String::New(chars, size);
    delete[] chars;
    return result;
}

// Extracts a C string from a V8 Utf8Value.
const char * ToCString(const v8::String::Utf8Value & value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(TryCatch * try_catch)
{
    HandleScope handle_scope;
    String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    Handle<Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        printf("%s\n", exception_string);
    }
    else
    {
        // Print (filename):(line number): (message).
        String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        printf("%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        printf("%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++) {
            printf(" ");
        }
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++) {
            printf("^");
        }
        printf("\n");
        String::Utf8Value stack_trace(try_catch->StackTrace());
        if (stack_trace.length() > 0) {
            const char* stack_trace_string = ToCString(stack_trace);
            printf("%s\n", stack_trace_string);
        }
    }
}

bool ExecuteJS_File(const v8::Persistent<v8::Context> & context, const std::string & path)
{
    Context::Scope context_scope(context);
    return ExecuteJS_File(path);
}

bool ExecuteJS_File(const std::string & path)
{
    HandleScope handle_scope;
    Handle<String> file_name = String::New(path.c_str());
    Handle<String> source = ReadFile(path.c_str());
    if(source.IsEmpty()) {
        printf("Error reading '%s'\n", path.c_str());
        return false;
    }
    
//    printf("Read file: %s\n", path.c_str());
    
    TryCatch try_catch;
    Handle<Script> script = Script::Compile(source, file_name);
//    printf("Compiled\n");
    if(script.IsEmpty())
    {
        ReportException(&try_catch);
        return false;
    }
    else
    {
//        printf("Running file:\n");
        Handle<Value> result = script->Run();
        if(result.IsEmpty()) {
            ReportException(&try_catch);
            return false;
        }
    }
    return true;
}

//******************************************************************************


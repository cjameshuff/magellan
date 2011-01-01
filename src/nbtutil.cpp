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

// NBT Utility
// Contact: Christopher James Huff <cjameshuff@gmail.com>

#include "nbt.h"
#include <iostream>
#include <string>
#include <sstream>
#include <getopt.h>

#include "nbtlua.h"

#include "misc.h"

using namespace std;

//******************************************************************************

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "nbtutil COMMAND|SCRIPT [OPTIONS]" << endl;
        return EXIT_FAILURE;
    }
    
    lua_State * lua = NBT_InitLua();
    
    lua_newtable(lua);
    for(int i = 0; i < argc; ++i) {
        lua_pushnumber(lua, i + 1);
        lua_pushstring(lua, argv[i]);
        lua_settable(lua, -3);
    }
    lua_setglobal(lua, "ARGV");
    
    lua_pushstring(lua, MCPath().c_str());
    lua_setglobal(lua, "MCPATH");
    
    // TODO: search scripts directory if command isn't an absolute or local path.
    NBT_RunLuaScript(argv[1]);
	return EXIT_SUCCESS;
}

//******************************************************************************

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

#include "magellan.h"
#include "misc.h"

#include "nbtlua.h"

using namespace std;

//******************************************************************************

static int MCNewWorld(lua_State * L, MC_World * map);
static MC_World * checkMC_World(lua_State * L, int argn);

//******************************************************************************


static int MCMap_load(lua_State * L)
{
    string mcWorldPath = luaL_checkstring(L, 1);
    MC_World * world = new MC_World;
    world->Load(mcWorldPath, "");// FIXME
    MCNewWorld(L, world);
    return 0;
}

static int MCMap_write(lua_State * L)
{
//    MC_World * world = checkMC_World(L, 1);
    return 0;
}

static int MCMap_render(lua_State * L)
{
//    MC_World * world = checkMC_World(L, 1);
//    
//    lua_getfield(L, 1, "xmin");
//    if(lua_isnil(L,-1))
//    double xmin = luaL_checknumber(L,-1);
//    
//    lua_getfield(L, 1, "foo");
//    string foo = luaL_checkstring(L,-1);
//    
//    
//    cout << "foo: " << foo << endl;
//    cout << "bar: " << bar << endl;
    
    return 0;
}

static int MCMap_stats(lua_State * L)
{
//    MC_World * world = checkMC_World(L, 1);
    return 0;
}

//******************************************************************************

static void LuaInitMC_World(lua_State * L)
{
    static const struct luaL_reg mcmap_classmthds[] = {
        {"load", MCMap_load},
        {NULL, NULL}
    };
    static const struct luaL_reg mcmap_mthds[] = {
        {"write", MCMap_write},
        {"stats", MCMap_stats},
        {"render", MCMap_render},
        {NULL, NULL}
    };
    
    luaL_newmetatable(L, "MC.Map");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); // pushes the metatable
    lua_settable(L, -3);  // metatable.__index = metatable
    luaL_openlib(L, NULL, mcmap_mthds, 0);
    luaL_openlib(L, "Map", mcmap_classmthds, 0);
}

static int MCNewWorld(lua_State * L, MC_World * map)
{
	MC_World ** maphdl = (MC_World **)lua_newuserdata(L, sizeof(MC_World *));
    *maphdl = map;
    
    luaL_getmetatable(L, "MC.Map");
    lua_setmetatable(L, -2);
    
    return 1;
}

static MC_World * checkMC_World(lua_State * L, int argn) {
    void * ud = luaL_checkudata(L, argn, "MC.Map");
    luaL_argcheck(L, ud != NULL, argn, "'Map' expected");
    return *(MC_World **)ud;
}

//******************************************************************************

// TODO: get rid of this quick and dirty hack and implement real API for laoding/rendering/etc.

static int LoadWorld(lua_State * L)
{
    std::string mcWorldName;
    int worldNum;
    int dimension;
    
    lua_getfield(L, 1, "world");
    if(!lua_isnil(L,-1)) worldNum = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "dim");
    if(!lua_isnil(L,-1)) dimension = luaL_checkinteger(L,-1);
    
    if(worldNum < 1 || worldNum > 5) {
        cout << "Bad world number (must be from 1 to 5)" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Generate world name/path
    ostringstream wnm;
    wnm << "World" << worldNum;
    if(dimension != 0)
        wnm << "/DIM-" << dimension;
    mcWorldName = wnm.str();
    
    string mcWorldPath = MCPath() + "/saves";
    cout << "mcWorldPath: " << mcWorldPath << endl;
    world.Load(mcWorldPath, mcWorldName);
    
    return 0;
}

void GetRegionOptions(lua_State * L, MagellanOptions & opts)
{
    lua_getfield(L, 1, "xmin");
    if(!lua_isnil(L,-1)) opts.xMin = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "xmax");
    if(!lua_isnil(L,-1)) opts.xMax = luaL_checkinteger(L,-1);
    
    
    lua_getfield(L, 1, "ymin");
    if(!lua_isnil(L,-1)) opts.yMin = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "ymax");
    if(!lua_isnil(L,-1)) opts.yMax = luaL_checkinteger(L,-1);
    
    
    lua_getfield(L, 1, "zmin");
    if(!lua_isnil(L,-1)) opts.zMin = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "zmax");
    if(!lua_isnil(L,-1)) opts.zMax = luaL_checkinteger(L,-1);
    
    
    lua_getfield(L, 1, "north");
    if(!lua_isnil(L,-1)) opts.xMin = -luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "south");
    if(!lua_isnil(L,-1)) opts.xMax = -luaL_checkinteger(L,-1);
    
    
    lua_getfield(L, 1, "east");
    if(!lua_isnil(L,-1)) opts.zMin = -luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "west");
    if(!lua_isnil(L,-1)) opts.zMax = -luaL_checkinteger(L,-1);
    
    // Set draw range properly (ranges are inclusive)
    opts.xMin = max(opts.xMin, world.xChunkMin);
    opts.xMax = min(opts.xMax, world.xChunkMax);
    opts.zMin = max(opts.zMin, world.zChunkMin);
    opts.zMax = min(opts.zMax, world.zChunkMax);
    
    opts.zMaxBlock = opts.zMax*16 + 15;
    opts.xMaxBlock = opts.xMax*16 + 15;
    opts.zMinBlock = opts.zMin*16;
    opts.xMinBlock = opts.xMin*16;
    
    opts.xSize = opts.xMax - opts.xMin + 1;
    opts.zSize = opts.zMax - opts.zMin + 1;
}


static int PrintWorldStats(lua_State * L)
{
    MagellanOptions opts;
    GetRegionOptions(L, opts);
    ComputeStats(opts);
    return 0;
}

static int RenderMap(lua_State * L)
{
    MagellanOptions opts;
    lua_getfield(L, 1, "scale");
    if(!lua_isnil(L,-1)) opts.scale = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "peel");
    if(!lua_isnil(L,-1)) opts.peel = luaL_checkinteger(L,-1);
    
    lua_getfield(L, 1, "layers");
    if(!lua_isnil(L,-1)) opts.layers = lua_toboolean(L,-1);
    
    GetRegionOptions(L, opts);
    
    lua_getfield(L, 1, "output_file");
    if(!lua_isnil(L,-1)) opts.outputFile = luaL_checkstring(L,-1);
    
    string dmode;
    lua_getfield(L, 1, "render_mode");
    if(!lua_isnil(L,-1)) dmode = luaL_checkstring(L,-1);
    
    
    if(dmode == "alt") {
        opts.lightingMode = kLightingAltitude;
    }
    else if(dmode == "altgray") {
        opts.lightingMode = kLightingAltitudeGray;
    }
    else if(dmode == "day") {
        opts.lightingMode = kLightingDay;
    }
    else if(dmode == "night") {
        opts.lightingMode = kLightingNight;
    }
    else if(dmode == "morning") {
        opts.lightingMode = kLightingMorning;
    }
    else if(dmode == "evening") {
        opts.lightingMode = kLightingEvening;
    }
    else {
        cerr << "Display mode " << dmode << " not supported!" << endl;
        abort();
    }
    
    if(opts.scale != 16 && opts.scale != 8 &&
       opts.scale != 4 && opts.scale != 2 &&
       opts.scale != 1)
    {
        cerr << "Scale value not supported!" << endl;
        abort();
    }
    
    RenderMap(opts);
    
    return 0;
}

void MGL_Init(lua_State * lua)
{
    lua_pushstring(lua, MCPath().c_str());
    lua_setglobal(lua, "MCPATH");
    
    LuaInitMC_World(lua);
	lua_register(lua, "load_world", LoadWorld);
	lua_register(lua, "print_world_stats", PrintWorldStats);
	lua_register(lua, "render_map", RenderMap);
}


//******************************************************************************

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

#include "nbtv8.h"

using namespace std;
using namespace v8;


static Persistent<FunctionTemplate> mcworldCons;

//******************************************************************************

Handle<Object> WrapMC_World(MC_World * map);
//static MC_World * checkMC_World(lua_State * L, int argn);

//******************************************************************************


static Handle<Value> MCMap_load(const Arguments & args) {
//    if(args.Length() != 1) return ThrowException(String::New("Bad parameters"));
//    string mcWorldPath = *String::Utf8Value(args[0]->ToString());
//    world->Load(mcWorldPath, "");// FIXME
//    return WrapMC_World(world);
    return Undefined();
}

static Handle<Value> MCMap_write(const Arguments & args) {
//    MC_World * world = checkMC_World(L, 1);
    return Undefined();
}

static Handle<Value> MCMap_render(const Arguments & args) {
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
    return Undefined();
}

static Handle<Value> MCMap_stats(const Arguments & args) {
////    MC_World * world = checkMC_World(L, 1);
    return Undefined();
}

//******************************************************************************


static Handle<Value> ConstructMC_World(const Arguments & args)
{
    HandleScope handle_scope;
    Handle<External> wrapper;
    
	if(!args.IsConstructCall()) return ThrowException(String::New("Cannot call constructor as function"));
    if(args.Length() == 2 || args.Length() > 3) return ThrowException(String::New("Bad parameters"));
    
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

Handle<Object> WrapMC_World(MC_World * map)
{
    HandleScope handle_scope;
    Handle<Value> ext = External::New(map);
    Handle<Object> new_map = mcworldCons->GetFunction()->NewInstance(1, &ext);
    return handle_scope.Close(new_map);
}

//static MC_World * checkMC_World(lua_State * L, int argn) {
//    void * ud = luaL_checkudata(L, argn, "MC.Map");
//    luaL_argcheck(L, ud != NULL, argn, "'Map' expected");
//    return *(MC_World **)ud;
//}

//******************************************************************************

// TODO: get rid of this quick and dirty hack and implement real API for laoding/rendering/etc.

static Handle<Value> LoadWorld(const Arguments & args)
{
    // load_world(world_number[, dimension])
    if(args.Length() != 1 && args.Length() != 2)
        return ThrowException(String::New("Bad parameters"));
    
    int worldNum = args[0]->IntegerValue();
    int dimension = 0;
    if(args.Length() > 1)
        dimension = args[1]->IntegerValue();
    
    if(worldNum < 1 || worldNum > 5)
        return ThrowException(String::New("Bad world number (must be from 1 to 5)"));
    
    // Generate world name/path
    std::string mcWorldName;
    ostringstream wnm;
    wnm << "World" << worldNum;
    if(dimension != 0)
        wnm << "/DIM-" << dimension;
    mcWorldName = wnm.str();
    
    string mcWorldPath = MCPath() + "/saves";
    cout << "mcWorldPath: " << mcWorldPath << endl;
    world.Load(mcWorldPath, mcWorldName);
    
    return Undefined();
}

void GetRegionOptions(Handle<Object> optsObj, MagellanOptions & opts)
{
    v8::Local<v8::Value> val;
    
    if(TryGet(optsObj, "xmin", val)) opts.xMin = val->IntegerValue();
    if(TryGet(optsObj, "xmax", val)) opts.xMax = val->IntegerValue();
    
    if(TryGet(optsObj, "ymin", val)) opts.yMin = val->IntegerValue();
    if(TryGet(optsObj, "ymax", val)) opts.yMax = val->IntegerValue();
    
    if(TryGet(optsObj, "zmin", val)) opts.zMin = val->IntegerValue();
    if(TryGet(optsObj, "zmax", val)) opts.zMax = val->IntegerValue();
    
    if(TryGet(optsObj, "north", val)) opts.xMin = -val->IntegerValue();
    if(TryGet(optsObj, "south", val)) opts.xMax = -val->IntegerValue();
    if(TryGet(optsObj, "east", val)) opts.zMin = -val->IntegerValue();
    if(TryGet(optsObj, "west", val)) opts.zMax = -val->IntegerValue();
    
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


static Handle<Value> PrintWorldStats(const Arguments & args)
{
    if(args.Length() != 1)
        return ThrowException(String::New("Bad parameters"));
    MagellanOptions opts;
    Handle<Object> optsObj = Handle<Object>::Cast(args[0]);
    GetRegionOptions(optsObj, opts);
    ComputeStats(opts);
    return Undefined();
}

static Handle<Value> RenderMap(const Arguments & args)
{
    Handle<Object> optsObj = Handle<Object>::Cast(args[0]);
    v8::Local<v8::Value> val;
    MagellanOptions opts;
    
    GetRegionOptions(optsObj, opts);
    
    if(TryGet(optsObj, "scale", val)) opts.scale = val->IntegerValue();
    if(TryGet(optsObj, "peel", val)) opts.peel = val->IntegerValue();
    if(TryGet(optsObj, "layers", val)) opts.layers = val->BooleanValue();
    if(TryGet(optsObj, "output_file", val)) opts.outputFile = StringValue(val);
    
    string dmode;
    if(TryGet(optsObj, "render_mode", val)) dmode = StringValue(val);
    
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
    
    return Undefined();
}

void MGV8_InitBindings(v8::Handle<v8::ObjectTemplate> & global)
{
    HandleScope handle_scope;
    
    Handle<FunctionTemplate> lmcworldCons = FunctionTemplate::New(ConstructMC_World);
    mcworldCons = Persistent<FunctionTemplate>::New(lmcworldCons);
    global->Set(String::New("MC_World"), mcworldCons);
    
    lmcworldCons->Set(String::New("load"), FunctionTemplate::New(MCMap_load));
    
    Handle<ObjectTemplate> mapIT = lmcworldCons->InstanceTemplate();
    mapIT->SetInternalFieldCount(1);
    
    mapIT->Set(String::New("write"), FunctionTemplate::New(MCMap_write));
    mapIT->Set(String::New("stats"), FunctionTemplate::New(MCMap_stats));
    mapIT->Set(String::New("render"), FunctionTemplate::New(MCMap_render));
    
    global->Set(String::New("load_world"), FunctionTemplate::New(LoadWorld));
    global->Set(String::New("print_world_stats"), FunctionTemplate::New(PrintWorldStats));
    global->Set(String::New("render_map"), FunctionTemplate::New(RenderMap));
}


//******************************************************************************

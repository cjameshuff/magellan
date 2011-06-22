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

// Magellan Minecraft Mapper
// Contact: Christopher James Huff <cjameshuff@gmail.com>


#include <iostream>
#include <fstream>

#include <cmath>
#include <climits>
#include <stdint.h>

#include <dirent.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "nbtrb.h"
#include "nbtio.h"

#include "magellan.h"

#include "misc.h"


using namespace std;

void ParseArgs(int argc, char * argv[]);
void PrintUsage();

//******************************************************************************
// Globals
//******************************************************************************

MC_Stats stats;
MC_World world;


//******************************************************************************

void WriteImage(SimpleImage & outputImage, const string & path);

VALUE class_MCRegion;
VALUE class_MCWorld;

inline NBT_Region_IO * GetMCRegion(VALUE value) {
    NBT_Region_IO * val; Data_Get_Struct(value, NBT_Region_IO, val);
    return val;
}

void MCRegion_Free(void * st) {delete static_cast<NBT_Region_IO *>(st);}

static VALUE MCRegion_allocate(VALUE klass) {
    NBT_Region_IO * cval = new NBT_Region_IO;
    return Data_Wrap_Struct(klass, NULL, MCRegion_Free, (void *)cval);
}

static VALUE MCRegion_init(int argc, VALUE *argv, VALUE self)
{
    return self;
}

static VALUE MCRegion_open(VALUE self, VALUE rbfpath) {
    int err = GetMCRegion(self)->Open(StringValueCStr(rbfpath));
    return INT2FIX(err);
}

// TODO: compute and return stats, instead of printing to cout
static VALUE MCRegion_stats(VALUE self) {
    GetMCRegion(self)->PrintStats(cout);
    return self;
}

static VALUE MCRegion_chunk_exists(VALUE self, VALUE rb_x, VALUE rb_z) {
    NBT_Region_IO * rgn = GetMCRegion(self);
    if(rgn->ChunkExists(NUM2INT(rb_x), NUM2INT(rb_z)))
        return Qtrue;
    else
        return Qfalse;
}

static VALUE MCRegion_read_chunk_nbt(VALUE self, VALUE rb_x, VALUE rb_z) {
    NBT_Region_IO * rgn = GetMCRegion(self);
    if(rgn->ReadChunk(NUM2INT(rb_x), NUM2INT(rb_z)) != 0)
        return Qnil;
    
    NBT_TagCompound * nbt = LoadNBT_File(*rgn);
    VALUE rbnbt = NBT_CompoundToValue(nbt);
    delete nbt;
    return rbnbt;
}

static VALUE MCRegion_write_chunk_nbt(VALUE self, VALUE rb_x, VALUE rb_z, VALUE rb_nbt) {
    NBT_Region_IO * rgn = GetMCRegion(self);
    NBT_Tag * nbt = ValueToNBT(rb_nbt);
    nbt->Write(*rgn);
    delete nbt;
    rgn->WriteChunk(NUM2INT(rb_x), NUM2INT(rb_z));
    return self;
}


static VALUE MCWorld_compute_lights(VALUE self) {
    return Qnil;
}
static VALUE MCWorld_compute_heights(VALUE self) {
    return Qnil;
}


extern "C" void Init_magellan()
{
    VALUE mMGLN = rb_define_module("Magellan");
    Init_nbt();
    rb_define_const(mMGLN, "MCPATH", rb_str_new2(MCPath().c_str()));
//     global->Set(v8::String::New("MCPATH"), v8::String::New(MCPath().c_str()));
    
    class_MCRegion = rb_define_class("MCRegion", rb_cObject);
    
    rb_define_alloc_func(class_MCRegion, MCRegion_allocate);
    rb_define_method(class_MCRegion, "initialize", RUBY_METHOD_FUNC(MCRegion_init), -1);
    rb_define_method(class_MCRegion, "open", RUBY_METHOD_FUNC(MCRegion_open), 1);
    rb_define_method(class_MCRegion, "printstats", RUBY_METHOD_FUNC(MCRegion_stats), 0);
    
    rb_define_method(class_MCRegion, "chunk_exists", RUBY_METHOD_FUNC(MCRegion_chunk_exists), 2);
    rb_define_method(class_MCRegion, "read_chunk_nbt", RUBY_METHOD_FUNC(MCRegion_read_chunk_nbt), 2);
    rb_define_method(class_MCRegion, "write_chunk_nbt", RUBY_METHOD_FUNC(MCRegion_write_chunk_nbt), 2);
    
    class_MCWorld = rb_define_class("MCWorld", rb_cObject);
    rb_define_method(class_MCWorld, "compute_lights_intern", RUBY_METHOD_FUNC(MCWorld_compute_lights), 0);
    rb_define_method(class_MCWorld, "compute_heights_intern", RUBY_METHOD_FUNC(MCWorld_compute_heights), 0);
}
// int main(int argc, char * argv[])
// {
// //    TestImages();
//     if(argc < 2)
//     {
//         cout << "Magellan Minecraft Mapper" << endl;
//         
//         cout << "Usage:" << endl;
//         cout << "magellan COMMAND|SCRIPT [OPTIONS]" << endl;
//         return EXIT_FAILURE;
//     }
//     
//     //ParseArgs(argc, argv);
//     
//  LoadTextures();
//  //cout << "Textures loaded" << endl;
//     
//     v8::HandleScope handle_scope;
//     v8::Handle<v8::ObjectTemplate> global_tmpl = V8_InitBase();
//     V8NBT_InitBindings(global_tmpl);
//     MGV8_InitBindings(global_tmpl);
//     
//     v8::Persistent<v8::Context> context = v8::Context::New(NULL, global_tmpl);
//     v8::Context::Scope context_scope(context);
//     v8::Handle<v8::Object> global = context->Global();
//     
//     global->Set(v8::String::New("MCPATH"), v8::String::New(MCPath().c_str()));
//     
//     v8::Handle<v8::Array> V8ARGV = v8::Array::New(argc);
//     for(int i = 0; i < argc; ++i)
//         V8ARGV->Set(v8::Number::New(i), v8::String::New(argv[i]));
//     global->Set(v8::String::New("ARGV"), V8ARGV);
//     
//     RunMagellanScript(argv[1]);
//     
//  return EXIT_SUCCESS;
// }


void WriteImage(SimpleImage & outputImage, const string & path)
{
    cout << "Writing output image" << endl;
    PNG_FileInfo outputFI;
    if(!outputFI.Write(path, outputImage)) {
        cerr << "Could not write PNG image!" << endl;
        exit(EXIT_FAILURE);
    }
}


void RenderMap(const MagellanOptions & opts)
{
    SimpleImage outputImage(16*opts.zSize*opts.scale, 16*opts.xSize*opts.scale, 4);
    outputImage.Clear(0, 0, 0, 255);
    DrawTop(outputImage, opts);
    WriteImage(outputImage, opts.outputFile.c_str());
    cout << "Done." << endl;
}



void ComputeStats(const MagellanOptions & opts)
{
    stats.numBlocks = 0;
    for(int j = 0; j < kNumBlockTypes; ++j)
        stats.blockCounts[j] = 0;
    
    stats.yMax = 0;
    stats.yMin = 128;
    for(int x = opts.xMin; x <= opts.xMax; ++x)
    for(int z = opts.zMin; z <= opts.zMax; ++z)
    {
        MC_Chunk * chunk = world.ChunkAt(x, z);
        if(!chunk) continue;
        
        stats.numBlocks += 16*16*128;
        
        for(int bx = 0; bx < 16; ++bx)
        for(int bz = 0; bz < 16; ++bz)
        {
            MC_Block block;
            for(int by = opts.yMin; by <= opts.yMax; ++by)
            {
                chunk->GetBlock(block, bx, by, bz);
                ++stats.blockCounts[block.type];
                if(block.type != kBT_Air)
                {
                    if(by < 0) continue;
                    if(by > stats.yMax) stats.yMax = by;
                    if(by < stats.yMin) stats.yMin = by;
                }
            }
        }
    }
    
    cout << "\nStatistics:" << endl;
    
    cout << "World boundaries" << endl;
    cout << "xChunkMin: " << world.xChunkMin << endl;
    cout << "xChunkMax: " << world.xChunkMax << endl;
    cout << "zChunkMin: " << world.zChunkMin << endl;
    cout << "zChunkMax: " << world.zChunkMax << endl;
    cout << "xSize: " << world.xSize << endl;
    cout << "zSize: " << world.zSize << endl;
    
    cout << "\nRendered range:" << endl;
    cout << "xMin: " << opts.xMin << endl;
    cout << "xMax: " << opts.xMax << endl;
    cout << "zMin: " << opts.zMin << endl;
    cout << "zMax: " << opts.zMax << endl;
    cout << "xSize: " << opts.xSize << endl;
    cout << "zSize: " << opts.zSize << endl;
    
    cout << "Number of blocks: " << stats.numBlocks << endl;
    
    size_t maxLength = 0;
    for(int j = 0; j < kNumBlockTypes; ++j)
        maxLength = max(maxLength, kBlockTypeNames[j].length());
    
    cout << "\n\nBlock types:" << endl;
    for(int j = 0; j < kNumBlockTypes; ++j)
        cout << string(maxLength + 1 - kBlockTypeNames[j].length(), ' ')
             << kBlockTypeNames[j] << ": "
             << stats.blockCounts[j] << endl;
}


void RenderBlock(SimpleImage & outputImage, int scale, uint8_t type, int x, int y, int z, float light)
{
/*    if(x < opts.xMinBlock || x > opts.xMaxBlock ||
        z < opts.zMinBlock || z > opts.zMaxBlock)
    {
        cerr << "Block coordinates out of range: " << x << ", " << y << ", " << z << endl;
        return;
    }*/
    
    if(scale == 1)
    {
        uint8_t color[4] = {
            blockTypes[type].color[0]*light,
            blockTypes[type].color[1]*light,
            blockTypes[type].color[2]*light,
            blockTypes[type].color[3]*light,
        };
        outputImage.BlendPixel(z, x, color[0], color[1], color[2], color[3]);
    }
    else
    {
        SimpleImage * blockTex = blockTypes[type].texture[kTextureScaleMap[scale]];
        if(blockTex == NULL) {
            if(type != kBT_Air)
                cerr << "No texture for block type " << (int)type << " (" << kBlockTypeNames[type] << ")" << endl;
            return;
        }
        // Chunk coordinates are in world space chunk units, convert to block units
        // Blit texture at location
        outputImage.Blit(*blockTex, z*scale, x*scale, light);
    }
    ++stats.blocksDrawn;
}

void DrawTop(SimpleImage & outputImage, const MagellanOptions & opts)
{
    stats.blocksDrawn = 0;
    for(int x = opts.xMin; x <= opts.xMax; ++x)
    for(int z = opts.zMin; z <= opts.zMax; ++z)
    {
        MC_Chunk * chunk = world.ChunkAt(x, z);
        if(!chunk) continue;
        
        for(int bx = 0; bx < 16; ++bx)
        for(int bz = 0; bz < 16; ++bz)
        {
            stack<int32_t> drawStack;
            int32_t block = MC_Chunk::GetIdx(bx, opts.yMax + 1, bz);
            int32_t low = MC_Chunk::GetIdx(bx, opts.yMin, bz);
            if(opts.peel)
            {
                int peelcount = opts.peel;
                while(block >= low && peelcount)
                {
                    // Search for non-air block
                    while(--block >= low) {
                        if(chunk->GetType(block) != kBT_Air)
                            break;
                    }
                    // Bump count and search for air block
                    while(--block >= opts.yMin) {
                        if(chunk->GetType(block) == kBT_Air)
                            break;
                    }
                    --peelcount;
                }
                // Didn't hit anything, reset and take first hit
//                if(block < low)
//                    block = MC_Chunk::GetIdx(bx, opts.yMax + 1, bz);
            }
            
            // find highest non-air block
            while(--block >= low) {
                if(chunk->GetType(block) != kBT_Air) {
                    drawStack.push(block);
                    break;
                }
            }
            if(block < low) continue;// fell through the bottom
            
            // find highest *opaque* block
            while(!blockTypes[chunk->GetType(drawStack.top())].isOpaque) {
                if(--block < low)
                    break;
                drawStack.push(block);
            }
            if(block < low) continue;// hit transparent stuff, then fell through the bottom
            
            while(!drawStack.empty()) {
                block = drawStack.top(); drawStack.pop();
                int32_t by = block - low + opts.yMin;
                // FIXME: can go out of range at topmost layer
                float light = 1;
                switch(opts.lightingMode) {
                  case kLightingAltitude:
                  case kLightingAltitudeGray:// TODO: implement gray
                    light = (float)(by - opts.yMin)/(opts.yMax - opts.yMin);
                  break;
                  case kLightingDay: {
                    float blocklight = (float)chunk->GetBlocklight(block + 1)/15.0f;
                    float skylight = (float)chunk->GetSkylight(block + 1)/15.0f;
                    light = fminf(1.0f, blocklight + skylight);
                  } break;
                  case kLightingNight:
                    light = (float)chunk->GetBlocklight(block + 1)/15.0f;
                  break;
                  case kLightingMorning:// halflight with fog
                  case kLightingEvening: {// halflight without fog TODO
                    float blocklight = (float)chunk->GetBlocklight(block + 1)/15.0f;
                    float skylight = (float)chunk->GetSkylight(block + 1)/15.0f;
                    light = fminf(1.0f, blocklight + skylight*0.5f);
                  } break;
                }
                RenderBlock(outputImage, opts.scale, chunk->GetType(block),
                    opts.xMaxBlock - (x*16 + bx), by, opts.zMaxBlock - (z*16 + bz),
                    light);
            }
        }
    }
    cout << "Done. Blocks drawn: " << stats.blocksDrawn << endl;
}


/*void DrawLayer(SimpleImage & outputImage, const MagellanOptions & opts, int by)
{
    stats.blocksDrawn = 0;
    for(int x = opts.xMin; x <= opts.xMax; ++x)
    for(int z = opts.zMin; z <= opts.zMax; ++z)
    {
        MC_Chunk * chunk = world.ChunkAt(x, z);
        if(!chunk) continue;
        
        for(int bx = 0; bx < 16; ++bx)
        for(int bz = 0; bz < 16; ++bz) {
            MC_Block block;
            chunk->GetBlock(block, bx, by, bz);
            RenderBlock(outputImage, opts.scale, block.type, x*16 + bx, by, z*16 + bz, 1.0f);
        }
    }
    cout << "Done. Blocks drawn: " << stats.blocksDrawn << endl;
}*/


// 3D rendering:
// For each world chunk, collect blocks with at least one face exposed
// Generate 6 display lists, one for each face of all blocks that have that face
// exposed.
// Draw all faces that might be visible in a chunk, given current camera position
// and direction. Let the depth buffer sort it all out.


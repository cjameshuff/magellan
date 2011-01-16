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

#include "blocktypes.h"

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "misc.h"

using namespace std;

//******************************************************************************
// Statics and globals
//******************************************************************************

BlockType blockTypes[256];

static SimpleImage texturesImage;

const std::string kBlockTypeNames[] = {
    "Air",          // 0x00
    "Stone",        // 0x01
    "Grass",        // 0x02
    "Dirt",         // 0x03
    "Cobblestone",  // 0x04
    "Wood",         // 0x05
    "Sapling",      // 0x06
    "Bedrock",      // 0x07
    "Water",        // 0x08
    "WaterPooled",  // 0x09
    "Lava",         // 0x0A
    "LavaPooled",   // 0x0B
    "Sand",         // 0x0C
    "Gravel",       // 0x0D
    "GoldOre",      // 0x0E
    "IronOre",      // 0x0F
    
    "CoalOre",      // 0x10
    "Log",          // 0x11
    "Leaves",       // 0x12
    "Sponge",       // 0x13
    "Glass",        // 0x14
    
    "LapisOre",     // 0x15
    "LapisBlock",   // 0x16
    "Dispenser",    // 0x17
    "Sandstone",    // 0x18
    "NoteBlock",    // 0x19
    "AquaGreenCloth", // 0x1A
    "CyanCloth",    // 0x1B
    "BlueCloth",    // 0x1C
    "PurpleCloth",  // 0x1D
    "IndigoCloth",  // 0x1E
    "VioletCloth",  // 0x1F
    "MagentaCloth", // 0x20
    "PinkCloth",    // 0x21
    "BlackCloth",   // 0x22
    "GrayCloth",    // 0x23
    "WhiteCloth",   // 0x24
    
    "YellowFlower", // 0x25
    "RedRose",      // 0x26
    "BrownMushroom", // 0x27
    "RedMushroom",  // 0x28
    "GoldBlock",    // 0x29
    "IronBlock",    // 0x2A
    "DoubleStep",   // 0x2B
    "Step",         // 0x2C
    "Brick",        // 0x2D
    "TNT",          // 0x2E
    "BookCase",     // 0x2F
    
    "MossyCobblestone", // 0x30
    "Obsidian",     // 0x31
    "Torch",        // 0x32
    "Fire",         // 0x33
    "MobSpawner",   // 0x34
    "WoodStairs",   // 0x35
    "Chest",        // 0x36
    "RedstoneWire", // 0x37
    "DiamondOre",   // 0x38
    "DiamondBlock", // 0x39
    "Workbench",    // 0x3A
    "Crops",        // 0x3B
    "Soil",         // 0x3C
    "Furnace",      // 0x3D
    "BurningFurnace", // 0x3E
    "SignPost",     // 0x3F
    
    "WoodDoor",     // 0x40
    "Ladder",       // 0x41
    "MinecartTrack", // 0x42
    "CobblestoneStairs", // 0x43
    "WallSign",     // 0x44
    "Lever",        // 0x45
    "StonePressurePlate", // 0x46
    "IronDoor",     // 0x47
    "WoodPressurePlate", // 0x48
    "RedstoneOre", // 0x49
    "GlowingRedstoneOre", // 0x4A
    "RedstoneTorchOff", // 0x4B
    "RedstoneTorchOn", // 0x4C
    "StoneButton",  // 0x4D
    "Snow",         // 0x4E
    "Ice",          // 0x4F
    
    "SnowBlock",    // 0x50
    "Cactus",       // 0x51
    "Clay",         // 0x52
    "Reed",         // 0x53
    "Jukebox",      // 0x54
    "Fence",        // 0x55
    "Pumpkin",      // 0x56
    "Netherstone",  // 0x57
    "SlowSand",     // 0x58
    "LightStone",   // 0x59
    "Portal",       // 0x5A
    "GlowingPumpkin", // 0x5B
    "CakeBlock"     // 0x5C
};

//******************************************************************************
ScaledTexture::ScaledTexture()
{
    for(int j = 0; j < kNumTextureScales; ++j)
        texture[kNumTextureScales] = NULL;
}

void ScaledTexture::Compute(SimpleImage * base)
{
    texture[0] = base;
    for(int j = 1; j < kNumTextureScales; ++j) {
        texture[j] = new SimpleImage(*texture[j - 1]);
        texture[j]->Halve();
    }
}
//******************************************************************************

// Extract texture from main image and map its ID.
// Textures are indexed left to right, top down. Coordinates are in 16x16 texture
// tiles, not pixels.
void LoadTexture(int id, int x, int y, bool isOpaque = true, float tr = 1.0f, float tg = 1.0f, float tb = 1.0f)
{
    // Extract a 16x16 tile at the given coordinates
    SimpleImage * texture = texturesImage.Slice(x*16, (15-y)*16, 16, 16);
    texture->Mul(tr, tg, tb, 1.0f);
    
    // Generate smaller versions and overall color
    blockTypes[id].texture.Compute(texture);
    
    // pick smallest texture and average its pixels to get overall color
    texture = blockTypes[id].texture[kNumTextureScales - 1];
    int32_t n = texture->NumPixels();
    int col[] = {0, 0, 0, 0};
    for(int p = 0; p < n; ++p) {
        for(int c = 0; c < 4; ++c)
            col[c] += texture->pixels[p*4 + c];
    }
    for(int c = 0; c < 4; ++c)
        blockTypes[id].color[c] = col[c]/n;
    
    blockTypes[id].isOpaque = isOpaque;
}

void LoadTextures()
{
    PNG_FileInfo pngFileInfo;
    if(!pngFileInfo.Read(MCPath() + "/magellan/terrain.png", texturesImage)) {
        cerr << "Could not read terrain.png!" << endl;
        exit(EXIT_FAILURE);
    }
    
    blockTypes[kBT_Air].color[0] = 255;
    blockTypes[kBT_Air].color[1] = 255;
    blockTypes[kBT_Air].color[2] = 255;
    blockTypes[kBT_Air].color[3] = 0;
    blockTypes[kBT_Air].isOpaque = false;
    
    //LoadTexture(kBT_Air, 14, 0, false);
    LoadTexture(kBT_Grass, 0, 0, true, 0.25f, 0.75f, 0.05f);
    LoadTexture(kBT_Stone, 1, 0);
    LoadTexture(kBT_Dirt, 2, 0);
    // grassy dirt, side
    LoadTexture(kBT_Wood, 4, 0);
    // Doublestep side
    LoadTexture(kBT_DoubleStep, 6, 0);
    LoadTexture(kBT_Step, 6, 0);
    LoadTexture(kBT_Brick, 7, 0);
    LoadTexture(kBT_TNT, 8, 0);
    // 9, 0: TNT kaboom
    // 10, 0: TNT top
    // 11, 0: Glass shatter?
    LoadTexture(kBT_RedRose, 12, 0, false);
    LoadTexture(kBT_YellowFlower, 13, 0, false);
    // Unused
    LoadTexture(kBT_Sapling, 15, 0, false);
    
    LoadTexture(kBT_Cobblestone, 0, 1);
    LoadTexture(kBT_CobblestoneStairs, 0, 1);// Do something special here?
    LoadTexture(kBT_Bedrock, 1, 1);
    LoadTexture(kBT_Sand, 2, 1);
    LoadTexture(kBT_Gravel, 3, 1);
    // Log side
    LoadTexture(kBT_Log, 5, 1);
    LoadTexture(kBT_IronBlock, 6, 1);
    LoadTexture(kBT_GoldBlock, 7, 1);
    LoadTexture(kBT_DiamondBlock, 8, 1);
    // chest side
    // chest back
    LoadTexture(kBT_Chest, 11, 1);
    LoadTexture(kBT_RedMushroom, 12, 1, false);
    LoadTexture(kBT_BrownMushroom, 13, 1, false);
    // blank
    LoadTexture(kBT_Fire, 15, 1);
    
    LoadTexture(kBT_GoldOre, 0, 2);
    LoadTexture(kBT_IronOre, 1, 2);
    LoadTexture(kBT_CoalOre, 2, 2);
    LoadTexture(kBT_BookCase, 3, 2);
    LoadTexture(kBT_MossyCobblestone, 4, 2);
    LoadTexture(kBT_Obsidian, 5, 2);
    //LoadTexture(kBT_, 6, 2);// unused
    //LoadTexture(kBT_, 7, 2);// unused
    //LoadTexture(kBT_, 8, 2);// unused
    //LoadTexture(kBT_, 9, 2);// Large chest front left
    //LoadTexture(kBT_, 10, 2);// Large chest front right
    LoadTexture(kBT_Workbench, 11, 2);// Workbench top
    //LoadTexture(kBT_Furnace, 12, 2);// Furnace front dark
    //LoadTexture(kBT_, 13, 2);// Furnace side/back
    LoadTexture(kBT_Dispenser, 14, 2);// unused
    //LoadTexture(kBT_, 15, 2);// Fire 2
    
    LoadTexture(kBT_Sponge, 0, 3);
    LoadTexture(kBT_Glass, 1, 3, false);
    LoadTexture(kBT_DiamondOre, 2, 3);
    LoadTexture(kBT_RedstoneOre, 3, 3);
    // TODO: color these properly, along with grass
    LoadTexture(kBT_Leaves, 4, 3, false, 0.35f, 0.65f, 0.1f);
    //LoadTexture(kBT_, 6, 3);// unused
    //LoadTexture(kBT_, 7, 3);// unused
    //LoadTexture(kBT_, 8, 3);// unused
    //LoadTexture(kBT_, 9, 3);// large chest back 1
    //LoadTexture(kBT_, 10, 3);// large chest back 2
    //LoadTexture(kBT_, 11, 3);// workbench, side 1
    //LoadTexture(kBT_, 12, 3);// workbench, side 2
    LoadTexture(kBT_Furnace, 13, 3);// Furnace front fired
    //LoadTexture(kBT_, 14, 3);// Furnace top
    //LoadTexture(kBT_, 15, 3);// unused
    
    LoadTexture(kBT_Wool, 0, 4);
    LoadTexture(kBT_MobSpawner, 1, 4, false);// Fire cage..."mob spawner"?
    LoadTexture(kBT_Snow, 2, 4);
    LoadTexture(kBT_Ice, 3, 4, false);
    //LoadTexture(kBT_, 4, 4);// Snowy dirt, side
    LoadTexture(kBT_Cactus, 5, 4, false);// cactus top
    //LoadTexture(kBT_, 6, 4);// cactus side
    //LoadTexture(kBT_, 7, 4);// cactus slice top/bottom
    LoadTexture(kBT_Clay, 8, 4);
    LoadTexture(kBT_Reed, 9, 4, false);
    LoadTexture(kBT_NoteBlock, 10, 4);// 
    LoadTexture(kBT_Jukebox, 11, 4);// jukebox top
    //LoadTexture(kBT_, 12, 4);// unused
    //LoadTexture(kBT_, 13, 4);// unused
    //LoadTexture(kBT_, 14, 4);// unused
    //LoadTexture(kBT_, 15, 4);// unused
    
    LoadTexture(kBT_Torch, 0, 5, false);
    LoadTexture(kBT_WoodDoor, 1, 5, false);
    LoadTexture(kBT_IronDoor, 2, 5, false);
    LoadTexture(kBT_Ladder, 3, 5, false);
    LoadTexture(kBT_Fence, 3, 5, false);// same as ladder for now
    LoadTexture(kBT_Soil, 6, 5);
    LoadTexture(kBT_Crops, 15, 5, false);
    
    LoadTexture(kBT_RedstoneTorchOn, 3, 6, false);
    LoadTexture(kBT_RedstoneWire, 4, 6, false);// Need to do more intelligent redstone rendering
    //LoadTexture(kBT_, 5, 6);// redstone wire, one way
    //LoadTexture(kBT_, 6, 6);// Pumpkin top
    LoadTexture(kBT_Netherstone, 7, 6);
    LoadTexture(kBT_SlowSand, 8, 6);
    LoadTexture(kBT_LightStone, 9, 6);
    
    LoadTexture(kBT_RedstoneTorchOff, 3, 7, false);
    //LoadTexture(kBT_, 4, 7);// Dark bark
    //LoadTexture(kBT_, 5, 7);// Birch bark
    //LoadTexture(kBT_, 6, 7);// Pumpkin side
    LoadTexture(kBT_Pumpkin, 7, 7);// Pumpkin face
    LoadTexture(kBT_GlowingPumpkin, 8, 7);
    
    LoadTexture(kBT_MinecartTrack, 0, 8, false);
    LoadTexture(kBT_CakeBlock, 12, 8, false);
    
    LoadTexture(kBT_LapisBlock, 0, 9);
    
    LoadTexture(kBT_LapisOre, 0, 10);
    
//    LoadTexture(kBT_Sandstone, 0, 11);// sandstone top
    
    LoadTexture(kBT_Sandstone, 0, 12);// sandstone side
    LoadTexture(kBT_Water, 13, 12, false);
    LoadTexture(kBT_WaterPooled, 13, 12, false);
    
    LoadTexture(kBT_Lava, 15, 15);
    LoadTexture(kBT_LavaPooled, 15, 15);
    
    // TODO: need to support multiple textures per block type, multiple cloth colors and log types
}

//******************************************************************************

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
#include "magellan.h"

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

//******************************************************************************
// Statics and globals
//******************************************************************************

BlockType blockTypes[256];

static SimpleImage texturesImage;

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

// TODO: Load textures from Ruby instead
// Texture types:
// Plain block: Same texture mapped on all sides.
// Fancy block: Separate texture for each side (furnace, workbench, chest...)
// Need to specify texture coordinates and orientation for each of 6 faces, and rotate to one of 4 positions.
// Model: block entities that aren't blocks. Levers, torches, ladders, rails, etc.
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
    LoadTexture(kBT_WoodStairs, 4, 0);// Do something special here?
    // Doublestep side
    LoadTexture(kBT_DoubleSlab, 6, 0);
    LoadTexture(kBT_Slab, 6, 0);
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
    LoadTexture(kBT_GlowingRedstoneOre, 3, 3);
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
    LoadTexture(kBT_SugarCaneBlock, 9, 4, false);
    LoadTexture(kBT_NoteBlock, 10, 4);// 
    LoadTexture(kBT_Jukebox, 11, 4);// jukebox top
    //LoadTexture(kBT_, 12, 4);// unused
    //LoadTexture(kBT_, 13, 4);// unused
    //LoadTexture(kBT_, 14, 4);// unused
    //LoadTexture(kBT_, 15, 4);// unused
    
    LoadTexture(kBT_Torch, 0, 5, false);
    LoadTexture(kBT_WoodDoorBlock, 1, 5, false);
    LoadTexture(kBT_IronDoorBlock, 2, 5, false);
    LoadTexture(kBT_Ladder, 3, 5, false);
    LoadTexture(kBT_Soil, 6, 5);
    LoadTexture(kBT_Crops, 15, 5, false);
    
    LoadTexture(kBT_Lever, 0, 6, false);
    LoadTexture(kBT_RedstoneTorch, 3, 6, false);
    LoadTexture(kBT_RedstoneWire, 4, 6, false);// Need to do more intelligent redstone rendering
    //LoadTexture(kBT_, 5, 6);// redstone wire, one way
    //LoadTexture(kBT_, 6, 6);// Pumpkin top
    LoadTexture(kBT_Netherstone, 7, 6);
    LoadTexture(kBT_SlowSand, 8, 6);
    LoadTexture(kBT_Glowstone, 9, 6);
    
    LoadTexture(kBT_RedstoneTorchOff, 3, 7, false);
    //LoadTexture(kBT_, 4, 7);// Dark bark
    //LoadTexture(kBT_, 5, 7);// Birch bark
    //LoadTexture(kBT_, 6, 7);// Pumpkin side
    LoadTexture(kBT_Pumpkin, 7, 7);// Pumpkin face
    LoadTexture(kBT_GlowingPumpkin, 8, 7);
    
    LoadTexture(kBT_Rail, 0, 8, false);
    LoadTexture(kBT_CakeBlock, 12, 8, false);
    
    LoadTexture(kBT_LapisBlock, 0, 9);
    
    LoadTexture(kBT_LapisOre, 0, 10);
    
//    LoadTexture(kBT_Sandstone, 0, 11);// sandstone top
    
    LoadTexture(kBT_Sandstone, 0, 12);// sandstone side
    LoadTexture(kBT_Water, 13, 12, false);
    LoadTexture(kBT_WaterPooled, 13, 12, false);
    
    LoadTexture(kBT_Lava, 15, 15);
    LoadTexture(kBT_LavaPooled, 15, 15);
    
    // Placeholders
    LoadTexture(kBT_Fence, 3, 5, false);// same as ladder for now
    LoadTexture(kBT_SignPost, 0, 6, false);// Actually a lever
    
    // TODO: need to support multiple textures per block type, multiple cloth colors and log types
}

//******************************************************************************

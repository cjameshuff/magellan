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

#ifndef BLOCKTYPES_H
#define BLOCKTYPES_H

enum {
    kBS_Top,
    kBS_Bottom,
    kBS_North,
    kBS_South,
    kBS_East,
    kBS_West
};

// TODO: block sides, oddball entities like doors
struct BlockType {
    bool isOpaque;
    SimpleImage * texture16;
    SimpleImage * texture8;
    SimpleImage * texture4;
    SimpleImage * texture2;
    uint8_t color[4];
    BlockType():
        isOpaque(true),
        texture16(NULL),
        texture8(NULL),
        texture4(NULL),
        texture2(NULL) {}
};


extern const std::string kBlockTypeNames[];
extern BlockType blockTypes[];

// http://minecraftwiki.net/wiki/Blocks,Items_%26_Data_values
enum {
    kBT_Air = 0x00,
    kBT_Stone = 0x01,
    kBT_Grass = 0x02,
    kBT_Dirt = 0x03,
    kBT_Cobblestone = 0x04,
    kBT_Wood = 0x05,
    kBT_Sapling = 0x06,//*
    kBT_Bedrock = 0x07,
    kBT_Water = 0x08,
    kBT_WaterPooled = 0x09,
    kBT_Lava = 0x0A,
    kBT_LavaPooled = 0x0B,
    kBT_Sand = 0x0C,
    kBT_Gravel = 0x0D,
    kBT_GoldOre = 0x0E,
    kBT_IronOre = 0x0F,
    
    kBT_CoalOre = 0x10,
    kBT_Log = 0x11,
    kBT_Leaves = 0x12,
    kBT_Sponge = 0x13,
    kBT_Glass = 0x14,
    
    // Cloth
    kBT_RedCloth = 0x15,
    kBT_OrangeCloth = 0x16,
    kBT_YellowCloth = 0x17,
    kBT_LimeCloth = 0x18,
    kBT_GreenCloth = 0x19,
    kBT_AquaGreenCloth = 0x1A,
    kBT_CyanCloth = 0x1B,
    kBT_BlueCloth = 0x1C,
    kBT_PurpleCloth = 0x1D,
    kBT_IndigoCloth = 0x1E,
    kBT_VioletCloth = 0x1F,
    kBT_MagentaCloth = 0x20,
    kBT_PinkCloth = 0x21,
    kBT_BlackCloth = 0x22,
    kBT_GrayCloth = 0x23,
    kBT_WhiteCloth = 0x24,
    
    kBT_YellowFlower = 0x25,
    kBT_RedRose = 0x26,
    kBT_BrownMushroom = 0x27,
    kBT_RedMushroom = 0x28,
    kBT_GoldBlock = 0x29,
    kBT_IronBlock = 0x2A,
    kBT_DoubleStep = 0x2B,
    kBT_Step = 0x2C,
    kBT_Brick = 0x2D,
    kBT_TNT = 0x2E,
    kBT_BookCase = 0x2F,
    
    kBT_MossyCobblestone = 0x30,
    kBT_Obsidian = 0x31,
    kBT_Torch = 0x32,
    kBT_Fire = 0x33,
    kBT_MobSpawner = 0x34,
    kBT_WoodStairs = 0x35,
    kBT_Chest = 0x36,
    kBT_RedstoneWire = 0x37,
    kBT_DiamondOre = 0x38,
    kBT_DiamondBlock = 0x39,
    kBT_Workbench = 0x3A,
    kBT_Crops = 0x3B,
    kBT_Soil = 0x3C,
    kBT_Furnace = 0x3D,
    kBT_BurningFurnace = 0x3E,
    kBT_SignPost = 0x3F,
    
    kBT_WoodDoor = 0x40,
    kBT_Ladder = 0x41,
    kBT_MinecartTrack = 0x42,
    kBT_CobblestoneStairs = 0x43,
    kBT_WallSign = 0x44,
    kBT_Lever = 0x45,
    kBT_StonePressurePlate = 0x46,
    kBT_IronDoor = 0x47,
    kBT_WoodPressurePlate = 0x48,
    kBT_RedstoneOre = 0x49,
    kBT_GlowingRedstoneOre = 0x4A,
    kBT_RedstoneTorchOff = 0x4B,
    kBT_RedstoneTorchOn = 0x4C,
    kBT_StoneButton = 0x4D,
    kBT_Snow = 0x4E,
    kBT_Ice = 0x4F,
    
    kBT_SnowBlock = 0x50,
    kBT_Cactus = 0x51,
    kBT_Clay = 0x52,
    kBT_Reed = 0x53,
    kBT_Jukebox = 0x54,
    kBT_Fence = 0x55,
    kBT_Pumpkin = 0x56,
    kBT_Netherstone = 0x57,
    kBT_SlowSand = 0x58,
    kBT_LightStone = 0x59,
    kBT_Portal = 0x5A,
    kBT_GlowingPumpkin = 0x5B,
    kNumBlockTypes
};
#endif // BLOCKTYPES_H

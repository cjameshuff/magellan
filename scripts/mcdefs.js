// http://www.minecraftwiki.net/wiki/Data_values

// inventory slot numbers:
// 0-8: equipped inventory (any item)
// 9-35: main inventory (any item)
// 80, 81, 82, 83: work area (any item)
// 103: helmet
// 102: torso
// 101: leggings
// 100: boots

var ARMOR_SLOTS = {
    "boots": 100,
    "legs": 101,
    "torso": 102,
    "helmet": 103,
};

// Item IDs referenced by item name
// TODO:
// Possibly store more block/item information in table. Whether an item is
// inventory-safe, etc.
var BLOCK_NAMES = {}
var BLOCK_IDS = {
    "Air":           0x00,
    "Stone":         0x01,
    "Grass":         0x02,
    "Dirt":          0x03,
    "Cobblestone":   0x04,
    "Wood":          0x05,
    "Sapling":       0x06,
    "Bedrock":       0x07,
    "Water":         0x08,
    "WaterPooled":   0x09,
    "Lava":          0x0A,
    "LavaPooled":    0x0B,
    "Sand":          0x0C,
    "Gravel":        0x0D,
    "GoldOre":       0x0E,
    "IronOre":       0x0F,
    
    "CoalOre":       0x10,
    "Log":           0x11,
    "Leaves":        0x12,
    "Sponge":        0x13,
    "Glass":         0x14,
    
    "LapisOre":      0x15,
    "LapisBlock":    0x16,
    "Dispenser":     0x17,
    "Sandstone":     0x18,
    "NoteBlock":     0x19,
    // 0x1A-0x22 and 0x24 are unused
    "Wool":          0x23,
    
    "YellowFlower":   0x25,
    "RedRose":        0x26,
    "BrownMushroom":  0x27,
    "RedMushroom":    0x28,
    "GoldBlock":      0x29,
    "IronBlock":      0x2A,
    "DoubleStep":     0x2B,
    "Step":           0x2C,
    "Brick":          0x2D,
    "TNT":            0x2E,
    "BookCase":       0x2F,
    
    "MossyCobblestone":  0x30,
    "Obsidian":          0x31,
    "Torch":             0x32,
    "Fire":              0x33,
    "MobSpawner":        0x34,
    "WoodStairs":        0x35,
    "Chest":             0x36,
    "RedstoneWire":      0x37,
    "DiamondOre":        0x38,
    "DiamondBlock":      0x39,
    "Workbench":         0x3A,
    "Crops":             0x3B,
    "Soil":              0x3C,
    "Furnace":           0x3D,
    "BurningFurnace":    0x3E,
    "SignPost":          0x3F,// sign placed on a post
    
    "WoodDoorBlock":       0x40,
    "Ladder":              0x41,
    "MinecartTrack":       0x42,
    "CobblestoneStairs":   0x43,
    "WallSign":            0x44,// sign hung on a wall
    "Lever":               0x45,
    "StonePressurePlate":  0x46,
    "IronDoorBlock":       0x47,
    "WoodPressurePlate":   0x48,
    "RedstoneOre":         0x49,
    "GlowingRedstoneOre":  0x4A,
    "RedstoneTorchOff":    0x4B,
    "RedstoneTorchOn":     0x4C,
    "StoneButton":         0x4D,
    "Snow":                0x4E,
    "Ice":                 0x4F,
    
    "SnowBlock":       0x50,
    "Cactus":          0x51,
    "Clay":            0x52,
    "SugarCaneBlock":  0x53,
    "Jukebox":         0x54,
    "Fence":           0x55,
    "Pumpkin":         0x56,
    "Netherstone":     0x57,
    "SlowSand":        0x58,
    "LightStone":      0x59,
    "Portal":          0x5A,
    "GlowingPumpkin":  0x5B,
    "CakeBlock":       0x5C
};


var ITEM_NAMES = {}

var ITEM_IDS = {
    "IronSpade":           0x100,
    "IronPickaxe":         0x101,
    "IronAxe":             0x102,
    "Flint&Steel":         0x103,
    "Apple":               0x104,
    "Bow":                 0x105,
    "Arrow":               0x106,
    "Coal":                0x107,
    "Diamond":             0x108,
    "IronIngot":           0x109,
    "GoldIngot":           0x10A,
    "IronSword":           0x10B,
    "WoodenSword":         0x10C,
    "WoodenSpade":         0x10D,
    "WoodenPickaxe":       0x10E,
    "WoodenAxe":           0x10F,
    "StoneSword":          0x110,
    "StoneSpade":          0x111,
    "StonePickaxe":        0x112,
    "StoneAxe":            0x113,
    "DiamondSword":        0x114,
    "DiamondSpade":        0x115,
    "DiamondPickaxe":      0x116,
    "DiamondAxe":          0x117,
    "Stick":               0x118,
    "Bowl":                0x119,
    "MushroomSoup":        0x11A,
    "GoldSword":           0x11B,
    "GoldSpade":           0x11C,
    "GoldPickaxe":         0x11D,
    "GoldAxe":             0x11E,
    "String":              0x11F,
    "Feather":             0x120,
    "Gunpowder":           0x121,
    "WoodenHoe":           0x122,
    "StoneHoe":            0x123,
    "IronHoe":             0x124,
    "DiamondHoe":          0x125,
    "GoldHoe":             0x126,
    "Seeds":               0x127,
    "Wheat":               0x128,
    "Bread":               0x129,
    "LeatherHelmet":       0x12A,
    "LeatherChestplate":   0x12B,
    "LeatherLeggings":     0x12C,
    "LeatherBoots":        0x12D,
    "ChainmailHelmet":     0x12E,
    "ChainmailChestplate": 0x12F,
    "ChainmailLeggings":   0x130,
    "ChainmailBoots":      0x131,
    "IronHelmet":          0x132,
    "IronChestplate":      0x133,
    "IronLeggings":        0x134,
    "IronBoots":           0x135,
    "DiamondHelmet":       0x136,
    "DiamondChestplate":   0x137,
    "DiamondLeggings":     0x138,
    "DiamondBoots":        0x139,
    "GoldHelmet":          0x13A,
    "GoldChestplate":      0x13B,
    "GoldLeggings":        0x13C,
    "GoldBoots":           0x13D,
    "Flint":               0x13E,
    "Pork":                0x13F,
    "GrilledPork":         0x140,
    "Painting":            0x141,
    "GoldenApple":         0x142,
    "Sign":                0x143,
    "WoodenDoor":          0x144,
    "Bucket":              0x145,
    "WaterBucket":         0x146,
    "LavaBucket":          0x147,
    "MineCart":            0x148,
    "Saddle":              0x149,
    "IronDoor":            0x14A,
    "Redstone":            0x14B,
    "Snowball":            0x14C,
    "Boat":                0x14D,
    "Leather":             0x14E,
    "MilkBucket":          0x14F,
    "ClayBrick":           0x150,
    "ClayBalls":           0x151,
    "SugarCane":           0x152,
    "Paper":               0x153,
    "Book":                0x154,
    "SlimeBall":           0x155,
    "StorageMinecart":     0x156,
    "PoweredMinecart":     0x157,
    "Egg":                 0x158,
    "Compass":             0x159,
    "FishingRod":          0x15A,
    "Watch":               0x15B,
    "LightstoneDust":      0x15C,
    "RawFish":             0x15D,
    "CookedFish":          0x15E,
    "Dye":                 0x15F,
    "Bone":                0x160,
    "Sugar":               0x161,
    "GoldRecord":          0x8D0,
    "GreenRecord":         0x8D1
};
// Note: Dyes require additional data, stored in the "damage" field.

var DYE_IDS = {
    "InkSack":         0x0,
    "RoseRed":         0x1,
    "CactusGreen":     0x2,
    "CocoBeans":       0x3,
    "LapisLazuli":     0x4,
    "PurpleDye":       0x5,
    "CyanDye":         0x6,
    "LightGrayDye":    0x7,
    "GrayDye":         0x8,
    "PinkDye":         0x9,
    "LimeDye":         0xA,
    "DandelionYellow": 0xB,
    "LightBlueDye":    0xC,
    "MagentaDye":      0xD,
    "Orange":          0xE,
    "BoneMeal":        0xF
};

// Generate reverse lookup table mapping IDs to names
for(var i in BLOCK_IDS)
    BLOCK_NAMES[BLOCK_IDS[i]] = i;

// Add block names to table of item IDs
for(var i in BLOCK_NAMES)
    ITEM_IDS[BLOCK_NAMES[i]] = i;

// Generate reverse lookup table mapping IDs to names
for(var i in ITEM_IDS)
    ITEM_NAMES[ITEM_IDS[i]] = i;


function LoadLevelDat(worldNum)
{
    var WORLDPATH = MCPATH + "/saves/World" + worldNum;
    var LEVELDATPATH = WORLDPATH + "/level.dat";
    return NBT.load(LEVELDATPATH);
}

function WriteLevelDat(level_dat, worldNum) {
    // Make backup of old version and write new level.dat
    var WORLDPATH = MCPATH + "/saves/World" + worldNum;
    var LEVELDATPATH = WORLDPATH + "/level.dat";
    var err = os.rename(LEVELDATPATH, LEVELDATPATH + ".bkp");
    if(err != 0) {
        printf("Could not backup data file: %@\n", LEVELDATPATH);
        os.exit()
    }
    level_dat.write(LEVELDATPATH);
}

function GetFreeInvSlots(level_dat)
{
    // Empty slots have no entries, so we mark all the filled ones and then return
    // the unmarked ones.
    var invslots = [];
    for(var i = 0; i < 36; ++i)
        invslots[i] = true;
    
    var inventory = level_dat.get("Data").get("Player").get("Inventory");
    for(var i = 0; i < inventory.size(); ++i) {
        var item = inventory.get(i);
        var slot = item.get("Slot").value;
    //    printf("%@ of %@ in slot %@, damage %@", count, ITEM_NAMES[id], slot, damage);
        invslots[slot] = false;
    }
    
    var freeslots = [];
    for(var i = 0; i < 36; ++i)
        if(invslots[i])
            freeslots[freeslots.length] = i;
    
    return freeslots;
}

function AddInvItem(level_dat, itemID, damage, count, slot) {
    var inventory = level_dat.get("Data").get("Player").get("Inventory");
    printf("Adding %@ to slot %@\n", ITEM_NAMES[itemID], slot);
    var item = NBT.new_compound("");
    item.set("id", NBT.new_short(itemID))
    item.set("Damage", NBT.new_short(damage))
    item.set("Count", NBT.new_byte(count))
    item.set("Slot", NBT.new_byte(slot))
//    item.set(NBT.new_short("id", itemID))
//    item.set(NBT.new_short("Damage", damage))
//    item.set(NBT.new_byte("Count", count))
//    item.set(NBT.new_byte("Slot", slot))
    inventory.push_back(item);
}

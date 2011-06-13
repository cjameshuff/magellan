require 'magellan/magellan'

module Magellan
# http://www.minecraftwiki.net/wiki/Data_values

# inventory slot numbers:
# 0-8: equipped inventory (any item)
# 9-35: main inventory (any item)
# 80, 81, 82, 83: work area (any item)
# 103: helmet
# 102: torso
# 101: leggings
# 100: boots

# A map from integers < 72 to base36 strings, used for putting chunks in the
# right directories or for scanning through a world's chunk files.
CHUNK_DIRS = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h",# 0-17
    "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",# 18-35
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "1g", "1h",# 36-53
    "1i", "1j", "1k", "1l", "1m", "1n", "1o", "1p", "1q", "1r", "1s", "1t", "1u", "1v", "1w", "1x", "1y", "1z" # 54-72
];

ARMOR_SLOTS = {
    boots: 100,
    legs: 101,
    torso: 102,
    helmet: 103,
}

# Item IDs referenced by item name
# TODO:
# Possibly store more block/item information in table. Whether an item is
# inventory-safe, etc.
BLOCK_NAMES = {}

BLOCK_IDS = {
    Air:           0x00,
    Stone:         0x01,
    Grass:         0x02,
    Dirt:          0x03,
    Cobblestone:   0x04,
    Wood:          0x05,
    Sapling:       0x06,
    Bedrock:       0x07,
    Water:         0x08,
    WaterPooled:   0x09,
    Lava:          0x0A,
    LavaPooled:    0x0B,
    Sand:          0x0C,
    Gravel:        0x0D,
    GoldOre:       0x0E,
    IronOre:       0x0F,
    
    CoalOre:       0x10,
    Log:           0x11,
    Leaves:        0x12,
    Sponge:        0x13,
    Glass:         0x14,
    
    LapisOre:      0x15,
    LapisBlock:    0x16,
    Dispenser:     0x17,
    Sandstone:     0x18,
    NoteBlock:     0x19,
    BedBlock:      0x1A,
    PoweredRail:   0x1B,
    DetectorRail:  0x1C,
    Web:           0x1E,
    TallGrass:     0x1F,
    DeadShrub:     0x20,
    Wool:          0x23,
    
    YellowFlower:   0x25,
    RedRose:        0x26,
    BrownMushroom:  0x27,
    RedMushroom:    0x28,
    GoldBlock:      0x29,
    IronBlock:      0x2A,
    DoubleSlab:     0x2B,
    Slab:           0x2C,
    Brick:          0x2D,
    TNT:            0x2E,
    BookCase:       0x2F,
    
    MossyCobblestone:  0x30,
    Obsidian:          0x31,
    Torch:             0x32,
    Fire:              0x33,
    MobSpawner:        0x34,
    WoodStairs:        0x35,
    Chest:             0x36,
    RedstoneWire:      0x37,
    DiamondOre:        0x38,
    DiamondBlock:      0x39,
    Workbench:         0x3A,
    Crops:             0x3B,
    Soil:              0x3C,
    Furnace:           0x3D,
    BurningFurnace:    0x3E,
    SignPost:          0x3F,# sign placed on a post
    
    WoodDoorBlock:       0x40,
    Ladder:              0x41,
    Rail:                0x42,
    CobblestoneStairs:   0x43,
    WallSign:            0x44,# sign hung on a wall
    Lever:               0x45,
    StonePressurePlate:  0x46,
    IronDoorBlock:       0x47,
    WoodPressurePlate:   0x48,
    RedstoneOre:         0x49,
    GlowingRedstoneOre:  0x4A,
    RedstoneTorchOff:    0x4B,
    RedstoneTorchOn:     0x4C,
    StoneButton:         0x4D,
    Snow:                0x4E,
    Ice:                 0x4F,
    
    SnowBlock:       0x50,
    Cactus:          0x51,
    Clay:            0x52,
    SugarCaneBlock:  0x53,
    Jukebox:         0x54,
    Fence:           0x55,
    Pumpkin:         0x56,
    Netherstone:     0x57,
    SlowSand:        0x58,
    LightStone:      0x59,
    Portal:          0x5A,
    GlowingPumpkin:  0x5B,
    CakeBlock:       0x5C,
    LockedChest:     0x5F,
    Trapdoor:        0x60
};


ITEM_NAMES = {}

ITEM_IDS = {
    IronSpade:           0x100,
    IronPickaxe:         0x101,
    IronAxe:             0x102,
    FlintSteel:          0x103,
    Apple:               0x104,
    Bow:                 0x105,
    Arrow:               0x106,
    Coal:                0x107,
    Diamond:             0x108,
    IronIngot:           0x109,
    GoldIngot:           0x10A,
    IronSword:           0x10B,
    WoodenSword:         0x10C,
    WoodenSpade:         0x10D,
    WoodenPickaxe:       0x10E,
    WoodenAxe:           0x10F,
    StoneSword:          0x110,
    StoneSpade:          0x111,
    StonePickaxe:        0x112,
    StoneAxe:            0x113,
    DiamondSword:        0x114,
    DiamondSpade:        0x115,
    DiamondPickaxe:      0x116,
    DiamondAxe:          0x117,
    Stick:               0x118,
    Bowl:                0x119,
    MushroomSoup:        0x11A,
    GoldSword:           0x11B,
    GoldSpade:           0x11C,
    GoldPickaxe:         0x11D,
    GoldAxe:             0x11E,
    String:              0x11F,
    Feather:             0x120,
    Gunpowder:           0x121,
    WoodenHoe:           0x122,
    StoneHoe:            0x123,
    IronHoe:             0x124,
    DiamondHoe:          0x125,
    GoldHoe:             0x126,
    Seeds:               0x127,
    Wheat:               0x128,
    Bread:               0x129,
    LeatherHelmet:       0x12A,
    LeatherChestplate:   0x12B,
    LeatherLeggings:     0x12C,
    LeatherBoots:        0x12D,
    ChainmailHelmet:     0x12E,
    ChainmailChestplate: 0x12F,
    ChainmailLeggings:   0x130,
    ChainmailBoots:      0x131,
    IronHelmet:          0x132,
    IronChestplate:      0x133,
    IronLeggings:        0x134,
    IronBoots:           0x135,
    DiamondHelmet:       0x136,
    DiamondChestplate:   0x137,
    DiamondLeggings:     0x138,
    DiamondBoots:        0x139,
    GoldHelmet:          0x13A,
    GoldChestplate:      0x13B,
    GoldLeggings:        0x13C,
    GoldBoots:           0x13D,
    Flint:               0x13E,
    Pork:                0x13F,
    GrilledPork:         0x140,
    Painting:            0x141,
    GoldenApple:         0x142,
    Sign:                0x143,
    WoodenDoor:          0x144,
    Bucket:              0x145,
    WaterBucket:         0x146,
    LavaBucket:          0x147,
    MineCart:            0x148,
    Saddle:              0x149,
    IronDoor:            0x14A,
    Redstone:            0x14B,
    Snowball:            0x14C,
    Boat:                0x14D,
    Leather:             0x14E,
    MilkBucket:          0x14F,
    ClayBrick:           0x150,
    ClayBalls:           0x151,
    SugarCane:           0x152,
    Paper:               0x153,
    Book:                0x154,
    SlimeBall:           0x155,
    StorageMinecart:     0x156,
    PoweredMinecart:     0x157,
    Egg:                 0x158,
    Compass:             0x159,
    FishingRod:          0x15A,
    Watch:               0x15B,
    LightstoneDust:      0x15C,
    RawFish:             0x15D,
    CookedFish:          0x15E,
    Dye:                 0x15F,
    Bone:                0x160,
    Sugar:               0x161,
    Cake:                0x162,
    Bed:                 0x163,
    Repeater:            0x164,
    Cookie:              0x165,
    Map:                 0x166,
    GoldRecord:          0x8D0,
    GreenRecord:         0x8D1
};
# Note: Dyes, logs, slabs, fuel, * require additional data, stored in the "damage" field.

DYE_IDS = {
    InkSac:          0x0,
    RoseRed:         0x1,
    CactusGreen:     0x2,
    CocoBeans:       0x3,
    LapisLazuli:     0x4,
    PurpleDye:       0x5,
    CyanDye:         0x6,
    LightGrayDye:    0x7,
    GrayDye:         0x8,
    PinkDye:         0x9,
    LimeDye:         0xA,
    DandelionYellow: 0xB,
    LightBlueDye:    0xC,
    MagentaDye:      0xD,
    Orange:          0xE,
    BoneMeal:        0xF
};

SLAB_IDS = {
    StoneSlab:       0x0,
    SandstoneSlab:   0x1,
    WoodSlab:        0x2,
    CobblestoneSlab: 0x3
}

WOOD_IDS = {
    OakLog:   0x0,
    PineLog:  0x1,
    BirchLog: 0x2
}

FUEL_IDS = {
    Coal:   0x0,
    Charcoal:  0x1
}

SHRUB_IDS = {
    DeadShrubGrass: 0x0, # Identical in appearance to DeadShrub, but acts like tall grass
    TallGrass:      0x1,
    Fern:           0x2
}

# Generate reverse lookup table mapping block IDs to block names
BLOCK_IDS.each {|name, id| BLOCK_NAMES[id] = name.to_s}

# Blocks are items as well
# Add block names to table of item IDs
# TODO: rather than mix the tables like this, add logic to the lookup functions
BLOCK_IDS.each {|name, id| ITEM_IDS[name] = id}

# Generate reverse lookup table mapping item IDs to item names
ITEM_IDS.each {|name, id| ITEM_NAMES[id] = name.to_s}


def load_level_dat(worldName)
    NBT.load(MCPATH + "/saves/" + worldName + "/level.dat");
end

def write_level_dat(level_dat, worldName)
    # Make backup of old version and write new level.dat
    level_dat_path = MCPATH + "/saves/" + worldName + "/level.dat";
    File.rename(level_dat_path, level_dat_path + ".bkp");
    level_dat.write(level_dat_path);
end

class MC_Player
    def initialize()
        @player_nbt = NBT.new_compound("Player")
        @player_nbt.insert(NBT.new_int("Score", 0))
        @player_nbt.insert(NBT.new_int("Dimension", 0))
        @player_nbt.insert(NBT.new_byte("OnGround", 1))
        @player_nbt.insert(NBT.new_list("Pos", [
            NBT.new_double("", 0.0),
            NBT.new_double("", 0.0),
            NBT.new_double("", 0.0)
        ], NBT::TAG_DOUBLE))
        @player_nbt.insert(NBT.new_list("Motion", [
            NBT.new_double("", 0.0),
            NBT.new_double("", 0.0),
            NBT.new_double("", 0.0)
        ], NBT::TAG_DOUBLE))
        @player_nbt.insert(NBT.new_list("Rotation", [
            NBT.new_float("", 0.0),
            NBT.new_float("", 0.0)
        ], NBT::TAG_FLOAT))
        @player_nbt.insert(NBT.new_int("SpawnX", 0))
        @player_nbt.insert(NBT.new_int("SpawnY", 0))
        @player_nbt.insert(NBT.new_int("SpawnZ", 0))
        @player_nbt.insert(NBT.new_short("AttackTime", 0))
        @player_nbt.insert(NBT.new_short("HurtTime", 0))
        @player_nbt.insert(NBT.new_short("DeathTime", 0))
        @player_nbt.insert(NBT.new_short("Fire", -20))
        @player_nbt.insert(NBT.new_short("Health", 19))
        @player_nbt.insert(NBT.new_short("Air", 300))
        @player_nbt.insert(NBT.new_float("FallDistance", 0.0))
        @player_nbt.insert(NBT.new_byte("Sleeping", 0))
        @player_nbt.insert(NBT.new_short("SleepTimer", 0))
        @player_nbt.insert(NBT.new_list("Inventory", [], NBT::TAG_COMPOUND))
    end
    
    def onground=(val)
        @player_nbt[:OnGround] = val
    end
    
    def onground()
        @player_nbt[:OnGround].value
    end
    
    def pos=(pt)
        @player_nbt[:Pos][0] = pt[0]
        @player_nbt[:Pos][1] = pt[1]
        @player_nbt[:Pos][2] = pt[2]
    end
    
    def pos()
        [@player_nbt[:Pos][0].value, @player_nbt[:Pos][1].value, @player_nbt[:Pos][2].value]
    end
    
    def motion=(vel)
        @player_nbt[:Motion][0] = vel[0]
        @player_nbt[:Motion][1] = vel[1]
        @player_nbt[:Motion][2] = vel[2]
    end
    
    def motion()
        [@player_nbt[:Motion][0].value, @player_nbt[:Motion][1].value, @player_nbt[:Motion][2].value]
    end
    
    def rotation=(pt)
        @player_nbt[:Rotation][0] = pt[0]
        @player_nbt[:Rotation][1] = pt[1]
    end
    
    def rotation()
        [@player_nbt[:Rotation][0].value, @player_nbt[:Rotation][1].value]
    end
    
    def spawn=(pt)
        @player_nbt[:SpawnX] = pt[0]
        @player_nbt[:SpawnY] = pt[1]
        @player_nbt[:SpawnZ] = pt[2]
    end
    
    def spawn()
        [@player_nbt[:SpawnX].value, @player_nbt[:SpawnY].value, @player_nbt[:SpawnZ].value]
    end
    
    def health=(h)
        @player_nbt[:Health] = h
    end
    
    def health()
        @player_nbt[:Health].value
    end
    
    def sleeping=(s)
        @player_nbt[:Sleeping] = s
    end
    
    def health()
        @player_nbt[:Sleeping].value
    end
    
    def get_nbt()
        @player_nbt
    end
    
    def inventory()
        @player_nbt[:Inventory]
    end
    
    def get_free_inv_slots(level_dat)
        # Empty slots have no entries and entries can have any order, so we start with all
        # slots and then remove the used ones.
        invslots = (0..35).to_a
        inventory.value.each {|item|
            id = item[:id].value
            slot = item[:Slot].value
            count = item[:Count].value
            damage = item[:Damage].value
            #puts "%d of %s in slot %d, damage %d" % [count, ITEM_NAMES[id], slot, damage]
            invslots.delete(slot);
        }
        invslots
    end
    
    def add_inv_item(level_dat, itemID, damage, count, slot)
        puts "Adding %d %s to slot %d\n" % [count, ITEM_NAMES[itemID], slot]
        item = NBT.new_compound("");
        item.insert(NBT.new_short("id", itemID))
        item.insert(NBT.new_short("Damage", damage))
        item.insert(NBT.new_byte("Count", count))
        item.insert(NBT.new_byte("Slot", slot))
        inventory.value.push(item)
    end
end

def get_free_inv_slots(level_dat)
    # Empty slots have no entries, so we mark all the filled ones and then return
    # the unmarked ones.
    invslots = []
    (0..35).each {|i| invslots[i] = true}
    
    inventory = level_dat[:Data][:Player][:Inventory]
    inventory.value.each {|item|
        id = item[:id].value
        slot = item[:Slot].value
        count = item[:Count].value
        damage = item[:Damage].value
        puts "%d of %s in slot %d, damage %d" % [count, ITEM_NAMES[id], slot, damage]
        invslots[slot] = false;
    }
    
    freeslots = [];
    (0..35).each {|i|
        if(invslots[i])
            freeslots.push(i)
        end
    }
    
    freeslots
end

def add_inv_item(level_dat, itemID, damage, count, slot)
    inventory = level_dat[:Data][:Player][:Inventory]
    puts "Adding %d %s to slot %d\n" % [count, ITEM_NAMES[itemID], slot]
    item = NBT.new_compound("");
    item.insert(NBT.new_short("id", itemID))
    item.insert(NBT.new_short("Damage", damage))
    item.insert(NBT.new_byte("Count", count))
    item.insert(NBT.new_byte("Slot", slot))
    inventory.value.push(item)
end

class MC_LevelDat
    attr_accessor :player
    
    def initialize()
        @level_dat_nbt = NBT.new_compound("")
        @level_dat_nbt.insert(NBT.new_string("LevelName", "Unnamed World"))
        @level_dat_nbt.insert(NBT.new_long("SizeOnDisk", 0))
        @level_dat_nbt.insert(NBT.new_long("LastPlayed", 0))
        @level_dat_nbt.insert(NBT.new_long("RandomSeed", 0))
        @level_dat_nbt.insert(NBT.new_byte("thundering", 0))
        @level_dat_nbt.insert(NBT.new_int("thunderTime", 0))
        @level_dat_nbt.insert(NBT.new_byte("raining", 0))
        @level_dat_nbt.insert(NBT.new_int("rainTime", 0))
        @level_dat_nbt.insert(NBT.new_int("version", 19132))# 1.6.6
        @level_dat_nbt.insert(NBT.new_long("Time", 0))
        @level_dat_nbt.insert(NBT.new_int("SpawnX", 0))
        @level_dat_nbt.insert(NBT.new_int("SpawnY", 0))
        @level_dat_nbt.insert(NBT.new_int("SpawnZ", 0))
        
        @player = MC_Player.new
        @level_dat_nbt.insert(@player.get_nbt())
    end
    
    def get_nbt()
        @player_nbt
    end
end # class MC_LevelDat

end # module Magellan

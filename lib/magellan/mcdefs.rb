
module Magellan
# http://www.minecraftwiki.net/wiki/Data_values


# A map from integers < 72 to base36 strings, used for putting chunks in the
# right directories or for scanning through a world's chunk files.
# Only relevant for old Alpha maps
CHUNK_DIRS = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h",# 0-17
    "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",# 18-35
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "1g", "1h",# 36-53
    "1i", "1j", "1k", "1l", "1m", "1n", "1o", "1p", "1q", "1r", "1s", "1t", "1u", "1v", "1w", "1x", "1y", "1z" # 54-72
]


BLOCKS_BY_ID = {}
BLOCKS_BY_NAME = {}

BLOCK_TYPES = [
    {name: 'Air',          id: 0x00},
    {name: 'Stone',        id: 0x01},
    {name: 'Grass',        id: 0x02},
    {name: 'Dirt',         id: 0x03},
    {name: 'Cobblestone',  id: 0x04},
    {name: 'Wood',         id: 0x05},
    {name: 'Sapling',      id: 0x06},
    {name: 'Bedrock',      id: 0x07},
    {name: 'Water',        id: 0x08},
    {name: 'WaterPooled',  id: 0x09},
    {name: 'Lava',         id: 0x0A},
    {name: 'LavaPooled',   id: 0x0B},
    {name: 'Sand',         id: 0x0C},
    {name: 'Gravel',       id: 0x0D},
    {name: 'GoldOre',      id: 0x0E},
    {name: 'IronOre',      id: 0x0F},
    
    {name: 'CoalOre',      id: 0x10},
    {name: 'Log',          id: 0x11},
    {name: 'Leaves',       id: 0x12},
    {name: 'Sponge',       id: 0x13},
    {name: 'Glass',        id: 0x14},
    
    {name: 'LapisOre',     id: 0x15},
    {name: 'LapisBlock',   id: 0x16},
    {name: 'Dispenser',    id: 0x17},
    {name: 'Sandstone',    id: 0x18},
    {name: 'NoteBlock',    id: 0x19},
    {name: 'BedBlock',     id: 0x1A},
    {name: 'PoweredRail',  id: 0x1B},
    {name: 'DetectorRail', id: 0x1C},
    {name: 'Web',          id: 0x1E},
    {name: 'TallGrass',    id: 0x1F},
    {name: 'DeadShrub',    id: 0x20},
    {name: 'Wool',         id: 0x23},
    
    {name: 'YellowFlower',  id: 0x25},
    {name: 'RedRose',       id: 0x26},
    {name: 'BrownMushroom', id: 0x27},
    {name: 'RedMushroom',   id: 0x28},
    {name: 'GoldBlock',     id: 0x29},
    {name: 'IronBlock',     id: 0x2A},
    {name: 'DoubleSlab',    id: 0x2B},
    {name: 'Slab',          id: 0x2C},
    {name: 'Brick',         id: 0x2D},
    {name: 'TNT',           id: 0x2E},
    {name: 'BookCase',      id: 0x2F},
    
    {name: 'MossyCobblestone', id: 0x30},
    {name: 'Obsidian',         id: 0x31},
    {name: 'Torch',            id: 0x32},
    {name: 'Fire',             id: 0x33},
    {name: 'MobSpawner',       id: 0x34},
    {name: 'WoodStairs',       id: 0x35},
    {name: 'Chest',            id: 0x36},
    {name: 'RedstoneWire',     id: 0x37},
    {name: 'DiamondOre',       id: 0x38},
    {name: 'DiamondBlock',     id: 0x39},
    {name: 'Workbench',        id: 0x3A},
    {name: 'Crops',            id: 0x3B},
    {name: 'Soil',             id: 0x3C},
    {name: 'Furnace',          id: 0x3D},
    {name: 'BurningFurnace',   id: 0x3E},
    {name: 'SignPost',         id: 0x3F},# sign placed on a post
    
    {name: 'WoodDoorBlock',      id: 0x40},
    {name: 'Ladder',             id: 0x41},
    {name: 'Rail',               id: 0x42},
    {name: 'CobblestoneStairs',  id: 0x43},
    {name: 'WallSign',           id: 0x44},# sign hung on a wall
    {name: 'Lever',              id: 0x45},
    {name: 'StonePressurePlate', id: 0x46},
    {name: 'IronDoorBlock',      id: 0x47},
    {name: 'WoodPressurePlate',  id: 0x48},
    {name: 'RedstoneOre',        id: 0x49},
    {name: 'GlowingRedstoneOre', id: 0x4A},
    {name: 'RedstoneTorchOff',   id: 0x4B},
    {name: 'RedstoneTorch',      id: 0x4C},
    {name: 'StoneButton',        id: 0x4D},
    {name: 'Snow',               id: 0x4E},
    {name: 'Ice',                id: 0x4F},
    
    {name: 'SnowBlock',      id: 0x50},
    {name: 'Cactus',         id: 0x51},
    {name: 'Clay',           id: 0x52},
    {name: 'SugarCaneBlock', id: 0x53},
    {name: 'Jukebox',        id: 0x54},
    {name: 'Fence',          id: 0x55},
    {name: 'Pumpkin',        id: 0x56},
    {name: 'Netherstone',    id: 0x57},
    {name: 'SlowSand',       id: 0x58},
    {name: 'LightStone',     id: 0x59},
    {name: 'Portal',         id: 0x5A},
    {name: 'GlowingPumpkin', id: 0x5B},
    {name: 'CakeBlock',      id: 0x5C},
    {name: 'LockedChest',    id: 0x5F},
    {name: 'Trapdoor',       id: 0x60}
]

# Generate lookup tables mapping block IDs to block names and vice versa
BLOCK_TYPES.each {|block|
    BLOCKS_BY_ID[block[:id]] = block
    BLOCKS_BY_NAME[block[:name].to_sym] = block
}

# These blocks are fully transparent
[
    :Air,
    :Sapling,
    :Glass,
    :Web,
    :TallGrass,
    :DeadShrub,
    :Fire,
    :Torch,
    :SignPost,
    :WallSign,
    :Lever,
    :Fence,
    :Portal
].each {|item_name|
    BLOCKS_BY_NAME[item_name.to_sym][:opacity] = 0
}

# These blocks are partially transparent
[
    :Water,
    :WaterPooled,
    :Leaves,
    :Trapdoor
].each {|item_name|
    BLOCKS_BY_NAME[item_name.to_sym][:opacity] = 1
}

BLOCK_TYPES.each {|block|
    if(block[:opacity] == nil)
        block[:opacity] = 15
    end
}

# These blocks can't usefully be put in the inventory
[
    :BedBlock,
    :WoodDoorBlock,
    :BurningFurnace,
    :Portal,
    :RedstoneTorchOff,
    :GlowingRedstoneOre
].each {|item_name|
    BLOCKS_BY_NAME[item_name.to_sym][:inventory_safe] = false
}

BLOCK_TYPES.each {|block|
    if(block[:inventory_safe] == nil)
        block[:inventory_safe] = true
    end
}


ITEMS_BY_ID = {}
ITEMS_BY_NAME = {}
OTHER_ITEM_TYPES = [
    {name: "IronSpade",         id: 0x100},
    {name: "IronPickaxe",       id: 0x101},
    {name: "IronAxe",           id: 0x102},
    {name: "FlintSteel",        id: 0x103},
    {name: "Apple",             id: 0x104},
    {name: "Bow",               id: 0x105},
    {name: "Arrow",             id: 0x106},
    {name: "Coal",              id: 0x107},
    {name: "Diamond",           id: 0x108},
    {name: "IronIngot",         id: 0x109},
    {name: "GoldIngot",         id: 0x10A},
    {name: "IronSword",         id: 0x10B},
    {name: "WoodenSword",       id: 0x10C},
    {name: "WoodenSpade",       id: 0x10D},
    {name: "WoodenPickaxe",     id: 0x10E},
    {name: "WoodenAxe",         id: 0x10F},
    {name: "StoneSword",        id: 0x110},
    {name: "StoneSpade",        id: 0x111},
    {name: "StonePickaxe",      id: 0x112},
    {name: "StoneAxe",          id: 0x113},
    {name: "DiamondSword",      id: 0x114},
    {name: "DiamondSpade",      id: 0x115},
    {name: "DiamondPickaxe",    id: 0x116},
    {name: "DiamondAxe",        id: 0x117},
    {name: "Stick",             id: 0x118},
    {name: "Bowl",              id: 0x119},
    {name: "MushroomSoup",      id: 0x11A},
    {name: "GoldSword",         id: 0x11B},
    {name: "GoldSpade",         id: 0x11C},
    {name: "GoldPickaxe",       id: 0x11D},
    {name: "GoldAxe",           id: 0x11E},
    {name: "String",            id: 0x11F},
    {name: "Feather",           id: 0x120},
    {name: "Gunpowder",         id: 0x121},
    {name: "WoodenHoe",         id: 0x122},
    {name: "StoneHoe",          id: 0x123},
    {name: "IronHoe",           id: 0x124},
    {name: "DiamondHoe",        id: 0x125},
    {name: "GoldHoe",           id: 0x126},
    {name: "Seeds",             id: 0x127},
    {name: "Wheat",             id: 0x128},
    {name: "Bread",             id: 0x129},
    {name: "LeatherHelmet",     id: 0x12A},
    {name: "LeatherChestplate", id: 0x12B},
    {name: "LeatherLeggings",   id: 0x12C},
    {name: "LeatherBoots",      id: 0x12D},
    {name: "ChainmailHelmet",   id: 0x12E},
    {name: "ChainmailChestplate",   id: 0x12F},
    {name: "ChainmailLeggings",     id: 0x130},
    {name: "ChainmailBoots",    id: 0x131},
    {name: "IronHelmet",        id: 0x132},
    {name: "IronChestplate",    id: 0x133},
    {name: "IronLeggings",      id: 0x134},
    {name: "IronBoots",         id: 0x135},
    {name: "DiamondHelmet",     id: 0x136},
    {name: "DiamondChestplate",     id: 0x137},
    {name: "DiamondLeggings",   id: 0x138},
    {name: "DiamondBoots",      id: 0x139},
    {name: "GoldHelmet",        id: 0x13A},
    {name: "GoldChestplate",    id: 0x13B},
    {name: "GoldLeggings",      id: 0x13C},
    {name: "GoldBoots",         id: 0x13D},
    {name: "Flint",             id: 0x13E},
    {name: "Pork",              id: 0x13F},
    {name: "GrilledPork",       id: 0x140},
    {name: "Painting",          id: 0x141},
    {name: "GoldenApple",       id: 0x142},
    {name: "Sign",              id: 0x143},
    {name: "WoodenDoor",        id: 0x144},
    {name: "Bucket",            id: 0x145},
    {name: "WaterBucket",       id: 0x146},
    {name: "LavaBucket",        id: 0x147},
    {name: "MineCart",          id: 0x148},
    {name: "Saddle",            id: 0x149},
    {name: "IronDoor",          id: 0x14A},
    {name: "Redstone",          id: 0x14B},
    {name: "Snowball",          id: 0x14C},
    {name: "Boat",              id: 0x14D},
    {name: "Leather",           id: 0x14E},
    {name: "MilkBucket",        id: 0x14F},
    {name: "ClayBrick",         id: 0x150},
    {name: "ClayBalls",         id: 0x151},
    {name: "SugarCane",         id: 0x152},
    {name: "Paper",             id: 0x153},
    {name: "Book",              id: 0x154},
    {name: "SlimeBall",         id: 0x155},
    {name: "StorageMinecart",   id: 0x156},
    {name: "PoweredMinecart",   id: 0x157},
    {name: "Egg",               id: 0x158},
    {name: "Compass",           id: 0x159},
    {name: "FishingRod",        id: 0x15A},
    {name: "Watch",             id: 0x15B},
    {name: "LightstoneDust",    id: 0x15C},
    {name: "RawFish",           id: 0x15D},
    {name: "CookedFish",        id: 0x15E},
    {name: "Dye",               id: 0x15F},
    {name: "Bone",              id: 0x160},
    {name: "Sugar",             id: 0x161},
    {name: "Cake",              id: 0x162},
    {name: "Bed",               id: 0x163},
    {name: "Repeater",          id: 0x164},
    {name: "Cookie",            id: 0x165},
    {name: "Map",               id: 0x166},
    {name: "GoldRecord",        id: 0x8D0},
    {name: "GreenRecord",       id: 0x8D1}
]

OTHER_ITEM_TYPES.each {|item|
    if(item[:inventory_safe] == nil)
        item[:inventory_safe] = true
    end
}


# Blocks are items as well
# Add block types to table of item types
# TODO: rather than mix the tables like this, add logic to the lookup functions
ITEM_TYPES = BLOCK_TYPES + OTHER_ITEM_TYPES

# Generate lookup tables mapping item IDs to item names and vice versa
ITEM_TYPES.each {|item|
    ITEMS_BY_ID[item[:id]] = item
    ITEMS_BY_NAME[item[:name].to_sym] = item
}


# Note: Dyes, logs, slabs, fuel, * require additional data,
# stored in the "damage" field when in the inventory.

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
}

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

# inventory slot numbers:
# 0-8: equipped inventory (any item)
# 9-35: main inventory (any item)
# 80, 81, 82, 83: work area (any item)
# 103: helmet
# 102: torso
# 101: leggings
# 100: boots
ARMOR_SLOTS = {
    boots: 100,
    legs: 101,
    torso: 102,
    helmet: 103,
}


def load_level_dat(world_name)
    NBT.load(MCPATH + "/saves/" + world_name + "/level.dat")
end

def write_level_dat(level_dat, world_name)
    # Make backup of old version and write new level.dat
    level_dat_path = MCPATH + "/saves/" + world_name + "/level.dat"
    File.rename(level_dat_path, level_dat_path + ".bkp")
    level_dat.write(level_dat_path)
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
        puts "%d of %s in slot %d, damage %d" % [count, ITEMS_BY_ID[id][:name], slot, damage]
        invslots[slot] = false
    }
    
    freeslots = []
    (0..35).each {|i|
        if(invslots[i])
            freeslots.push(i)
        end
    }
    
    freeslots
end

def add_inv_item(level_dat, item_name, count, slot)
    item = nil
    damage = 0
    item_sym = item_name.to_sym
    if(DYE_IDS[item_sym] != nil)
        item = ITEMS_BY_NAME[:Dye]
        damage = DYE_IDS[item_sym]
    elsif(WOOD_IDS[item_sym] != nil)
        item = ITEMS_BY_NAME[:Log]
        damage = WOOD_IDS[item_sym]
    elsif(SLAB_IDS[item_sym] != nil)
        item = ITEMS_BY_NAME[:Slab]
        damage = SLAB_IDS[item_sym]
    elsif(FUEL_IDS[item_sym] != nil)
        item = ITEMS_BY_NAME[:Coal]
        damage = FUEL_IDS[item_sym]
    elsif(SHRUB_IDS[item_sym] != nil)
        item = ITEMS_BY_NAME[:TallGrass]
        damage = SHRUB_IDS[item_sym]
    else
        item = ITEMS_BY_NAME[item_sym]
    end

    if(item == nil)
        puts "Unknown item type specified."
        exit()
    end
    
    inventory = level_dat[:Data][:Player][:Inventory]
    puts "Adding %d %s to slot %d\n" % [count, item[:name], slot]
    item_nbt = NBT.new_compound("")
    item_nbt.insert(NBT.new_short("id", item[:id]))
    item_nbt.insert(NBT.new_short("Damage", damage))
    item_nbt.insert(NBT.new_byte("Count", count))
    item_nbt.insert(NBT.new_byte("Slot", slot))
    inventory.value.push(item_nbt)
end

end # module Magellan

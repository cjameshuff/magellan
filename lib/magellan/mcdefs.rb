
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
# Only relevant for old Alpha maps
CHUNK_DIRS = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g", "h",# 0-17
    "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",# 18-35
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "1g", "1h",# 36-53
    "1i", "1j", "1k", "1l", "1m", "1n", "1o", "1p", "1q", "1r", "1s", "1t", "1u", "1v", "1w", "1x", "1y", "1z" # 54-72
]

# Item IDs referenced by item name
# TODO:
# Possibly store more block/item information in table. Whether an item is
# inventory-safe, etc.
BLOCKS_BY_ID = {}
BLOCKS_BY_NAME = {}


BLOCK_TYPES = [
    {name: 'Air',          id: 0x00, opacity: 0},
    {name: 'Stone',        id: 0x01, opacity: 15},
    {name: 'Grass',        id: 0x02, opacity: 15},
    {name: 'Dirt',         id: 0x03, opacity: 15},
    {name: 'Cobblestone',  id: 0x04, opacity: 15},
    {name: 'Wood',         id: 0x05, opacity: 15},
    {name: 'Sapling',      id: 0x06, opacity: 0},
    {name: 'Bedrock',      id: 0x07, opacity: 15},
    {name: 'Water',        id: 0x08, opacity: 1},
    {name: 'WaterPooled',  id: 0x09, opacity: 1},
    {name: 'Lava',         id: 0x0A, opacity: 15},
    {name: 'LavaPooled',   id: 0x0B, opacity: 15},
    {name: 'Sand',         id: 0x0C, opacity: 15},
    {name: 'Gravel',       id: 0x0D, opacity: 15},
    {name: 'GoldOre',      id: 0x0E, opacity: 15},
    {name: 'IronOre',      id: 0x0F, opacity: 15},
    
    {name: 'CoalOre',      id: 0x10, opacity: 15},
    {name: 'Log',          id: 0x11, opacity: 15},
    {name: 'Leaves',       id: 0x12, opacity: 1},
    {name: 'Sponge',       id: 0x13, opacity: 15},
    {name: 'Glass',        id: 0x14, opacity: 0},
    
    {name: 'LapisOre',     id: 0x15, opacity: 15},
    {name: 'LapisBlock',   id: 0x16, opacity: 15},
    {name: 'Dispenser',    id: 0x17, opacity: 15},
    {name: 'Sandstone',    id: 0x18, opacity: 15},
    {name: 'NoteBlock',    id: 0x19, opacity: 15},
    {name: 'BedBlock',     id: 0x1A, opacity: 15},
    {name: 'PoweredRail',  id: 0x1B, opacity: 15},
    {name: 'DetectorRail', id: 0x1C, opacity: 15},
    {name: 'Web',          id: 0x1E, opacity: 0},
    {name: 'TallGrass',    id: 0x1F, opacity: 0},
    {name: 'DeadShrub',    id: 0x20, opacity: 0},
    {name: 'Wool',         id: 0x23, opacity: 15},
    
    {name: 'YellowFlower',  id: 0x25, opacity: 15},
    {name: 'RedRose',       id: 0x26, opacity: 15},
    {name: 'BrownMushroom', id: 0x27, opacity: 15},
    {name: 'RedMushroom',   id: 0x28, opacity: 15},
    {name: 'GoldBlock',     id: 0x29, opacity: 15},
    {name: 'IronBlock',     id: 0x2A, opacity: 15},
    {name: 'DoubleSlab',    id: 0x2B, opacity: 15},
    {name: 'Slab',          id: 0x2C, opacity: 15},
    {name: 'Brick',         id: 0x2D, opacity: 15},
    {name: 'TNT',           id: 0x2E, opacity: 15},
    {name: 'BookCase',      id: 0x2F, opacity: 15},
    
    {name: 'MossyCobblestone', id: 0x30, opacity: 15},
    {name: 'Obsidian',         id: 0x31, opacity: 15},
    {name: 'Torch',            id: 0x32, opacity: 0},
    {name: 'Fire',             id: 0x33, opacity: 15},
    {name: 'MobSpawner',       id: 0x34, opacity: 15},
    {name: 'WoodStairs',       id: 0x35, opacity: 15},
    {name: 'Chest',            id: 0x36, opacity: 15},
    {name: 'RedstoneWire',     id: 0x37, opacity: 15},
    {name: 'DiamondOre',       id: 0x38, opacity: 15},
    {name: 'DiamondBlock',     id: 0x39, opacity: 15},
    {name: 'Workbench',        id: 0x3A, opacity: 15},
    {name: 'Crops',            id: 0x3B, opacity: 15},
    {name: 'Soil',             id: 0x3C, opacity: 15},
    {name: 'Furnace',          id: 0x3D, opacity: 15},
    {name: 'BurningFurnace',   id: 0x3E, opacity: 15},
    {name: 'SignPost',         id: 0x3F, opacity: 15},# sign placed on a post
    
    {name: 'WoodDoorBlock',      id: 0x40, opacity: 15},
    {name: 'Ladder',             id: 0x41, opacity: 15},
    {name: 'Rail',               id: 0x42, opacity: 15},
    {name: 'CobblestoneStairs',  id: 0x43, opacity: 15},
    {name: 'WallSign',           id: 0x44, opacity: 0},# sign hung on a wall
    {name: 'Lever',              id: 0x45, opacity: 0},
    {name: 'StonePressurePlate', id: 0x46, opacity: 15},
    {name: 'IronDoorBlock',      id: 0x47, opacity: 15},
    {name: 'WoodPressurePlate',  id: 0x48, opacity: 15},
    {name: 'RedstoneOre',        id: 0x49, opacity: 15},
    {name: 'GlowingRedstoneOre', id: 0x4A, opacity: 15},
    {name: 'RedstoneTorchOff',   id: 0x4B, opacity: 15},
    {name: 'RedstoneTorchOn',    id: 0x4C, opacity: 15},
    {name: 'StoneButton',        id: 0x4D, opacity: 0},
    {name: 'Snow',               id: 0x4E, opacity: 15},
    {name: 'Ice',                id: 0x4F, opacity: 15},
    
    {name: 'SnowBlock',      id: 0x50, opacity: 15},
    {name: 'Cactus',         id: 0x51, opacity: 15},
    {name: 'Clay',           id: 0x52, opacity: 15},
    {name: 'SugarCaneBlock', id: 0x53, opacity: 0},
    {name: 'Jukebox',        id: 0x54, opacity: 15},
    {name: 'Fence',          id: 0x55, opacity: 0},
    {name: 'Pumpkin',        id: 0x56, opacity: 15},
    {name: 'Netherstone',    id: 0x57, opacity: 15},
    {name: 'SlowSand',       id: 0x58, opacity: 15},
    {name: 'LightStone',     id: 0x59, opacity: 15},
    {name: 'Portal',         id: 0x5A, opacity: 0},
    {name: 'GlowingPumpkin', id: 0x5B, opacity: 15},
    {name: 'CakeBlock',      id: 0x5C, opacity: 15},
    {name: 'LockedChest',    id: 0x5F, opacity: 15},
    {name: 'Trapdoor',       id: 0x60, opacity: 0}
]


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
}

# Generate reverse lookup table mapping block IDs to block names
BLOCK_TYPES.each {|block|
    BLOCKS_BY_ID[block[:id]] = block
    BLOCKS_BY_NAME[block[:name]] = block
}

# Blocks are items as well
# Add block names to table of item IDs
# TODO: rather than mix the tables like this, add logic to the lookup functions
BLOCKS_BY_NAME.each {|name, id| ITEM_IDS[name] = id}

# Generate reverse lookup table mapping item IDs to item names
ITEM_IDS.each {|name, id| ITEM_NAMES[id] = name.to_s}


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

ARMOR_SLOTS = {
    boots: 100,
    legs: 101,
    torso: 102,
    helmet: 103,
}


def load_level_dat(worldName)
    NBT.load(MCPATH + "/saves/" + worldName + "/level.dat")
end

def write_level_dat(level_dat, worldName)
    # Make backup of old version and write new level.dat
    level_dat_path = MCPATH + "/saves/" + worldName + "/level.dat"
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
        puts "%d of %s in slot %d, damage %d" % [count, ITEM_NAMES[id], slot, damage]
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

def add_inv_item(level_dat, itemID, damage, count, slot)
    inventory = level_dat[:Data][:Player][:Inventory]
    puts "Adding %d %s to slot %d\n" % [count, ITEM_NAMES[itemID], slot]
    item = NBT.new_compound("")
    item.insert(NBT.new_short("id", itemID))
    item.insert(NBT.new_short("Damage", damage))
    item.insert(NBT.new_byte("Count", count))
    item.insert(NBT.new_byte("Slot", slot))
    inventory.value.push(item)
end

end # module Magellan

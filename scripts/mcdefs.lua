-- http://www.minecraftwiki.net/wiki/Data_values
--print("mcdefs.lua")

-- TODO: this is what it is for historical reasons, reformat table.
-- Possibly store more block/item information in table.
BLOCK_NAMES = {
    "Air",          -- 0x00
    "Stone",        -- 0x01
    "Grass",        -- 0x02
    "Dirt",         -- 0x03
    "Cobblestone",  -- 0x04
    "Wood",         -- 0x05
    "Sapling",      -- 0x06
    "Bedrock",      -- 0x07
    "Water",        -- 0x08
    "WaterPooled",  -- 0x09
    "Lava",         -- 0x0A
    "LavaPooled",   -- 0x0B
    "Sand",         -- 0x0C
    "Gravel",       -- 0x0D
    "GoldOre",      -- 0x0E
    "IronOre",      -- 0x0F
    
    "CoalOre",      -- 0x10
    "Log",          -- 0x11
    "Leaves",       -- 0x12
    "Sponge",       -- 0x13
    "Glass",        -- 0x14
    
    -- Cloth
    "RedCloth",     -- 0x15
    "OrangeCloth",  -- 0x16
    "YellowCloth",  -- 0x17
    "LimeCloth",    -- 0x18
    "GreenCloth",   -- 0x19
    "AquaGreenCloth", -- 0x1A
    "CyanCloth",    -- 0x1B
    "BlueCloth",    -- 0x1C
    "PurpleCloth",  -- 0x1D
    "IndigoCloth",  -- 0x1E
    "VioletCloth",  -- 0x1F
    "MagentaCloth", -- 0x20
    "PinkCloth",    -- 0x21
    "BlackCloth",   -- 0x22
    "GrayCloth",    -- 0x23
    "WhiteCloth",   -- 0x24
    
    "YellowFlower", -- 0x25
    "RedRose",      -- 0x26
    "BrownMushroom", -- 0x27
    "RedMushroom",  -- 0x28
    "GoldBlock",    -- 0x29
    "IronBlock",    -- 0x2A
    "DoubleStep",   -- 0x2B
    "Step",         -- 0x2C
    "Brick",        -- 0x2D
    "TNT",          -- 0x2E
    "BookCase",     -- 0x2F
    
    "MossyCobblestone", -- 0x30
    "Obsidian",     -- 0x31
    "Torch",        -- 0x32
    "Fire",         -- 0x33
    "MobSpawner",   -- 0x34
    "WoodStairs",   -- 0x35
    "Chest",        -- 0x36
    "RedstoneWire", -- 0x37
    "DiamondOre",   -- 0x38
    "DiamondBlock", -- 0x39
    "Workbench",    -- 0x3A
    "Crops",        -- 0x3B
    "Soil",         -- 0x3C
    "Furnace",      -- 0x3D
    "BurningFurnace", -- 0x3E
    "SignPost",     -- 0x3F
    
    "WoodDoor",     -- 0x40
    "Ladder",       -- 0x41
    "MinecartTrack", -- 0x42
    "CobblestoneStairs", -- 0x43
    "WallSign",     -- 0x44
    "Lever",        -- 0x45
    "StonePressurePlate", -- 0x46
    "IronDoor",     -- 0x47
    "WoodPressurePlate", -- 0x48
    "RedstoneOre", -- 0x49
    "GlowingRedstoneOre", -- 0x4A
    "RedstoneTorchOff", -- 0x4B
    "RedstoneTorchOn", -- 0x4C
    "StoneButton",  -- 0x4D
    "Snow",         -- 0x4E
    "Ice",          -- 0x4F
    
    "SnowBlock",    -- 0x50
    "Cactus",       -- 0x51
    "Clay",         -- 0x52
    "Reed",         -- 0x53
    "Jukebox",      -- 0x54
    "Fence",        -- 0x55
    "Pumpkin",      -- 0x56
    "Netherstone",  -- 0x57
    "SlowSand",     -- 0x58
    "LightStone",   -- 0x59
    "Portal",       -- 0x5A
    "GlowingPumpkin" -- 0x5B
};

ITEM_NAMES = {
    [0x100] = "IronSpade",
    [0x101] = "IronPickaxe",
    [0x102] = "IronAxe",
    [0x103] = "Flint&Steel",
    [0x104] = "Apple",
    [0x105] = "Bow",
    [0x106] = "Arrow",
    [0x107] = "Coal",
    [0x108] = "Diamond",
    [0x109] = "IronIngot",
    [0x10A] = "GoldIngot",
    [0x10B] = "IronSword",
    [0x10C] = "WoodenSword",
    [0x10D] = "WoodenSpade",
    [0x10E] = "WoodenPickaxe",
    [0x10F] = "WoodenAxe",
    [0x110] = "StoneSword",
    [0x111] = "StoneSpade",
    [0x112] = "StonePickaxe",
    [0x113] = "StoneAxe",
    [0x114] = "DiamondSword",
    [0x115] = "DiamondSpade",
    [0x116] = "DiamondPickaxe",
    [0x117] = "DiamondAxe",
    [0x118] = "Stick",
    [0x119] = "Bowl",
    [0x11A] = "MushroomSoup",
    [0x11B] = "GoldSword",
    [0x11C] = "GoldSpade",
    [0x11D] = "GoldPickaxe",
    [0x11E] = "GoldAxe",
    [0x11F] = "String",
    [0x120] = "Feather",
    [0x121] = "Gunpowder",
    [0x122] = "WoodenHoe",
    [0x123] = "StoneHoe",
    [0x124] = "IronHoe",
    [0x125] = "DiamondHoe",
    [0x126] = "GoldHoe",
    [0x127] = "Seeds",
    [0x128] = "Wheat",
    [0x129] = "Bread",
    [0x12A] = "LeatherHelmet",
    [0x12B] = "LeatherChestplate",
    [0x12C] = "LeatherLeggings",
    [0x12D] = "LeatherBoots",
    [0x12E] = "ChainmailHelmet",
    [0x12F] = "ChainmailChestplate",
    [0x130] = "ChainmailLeggings",
    [0x131] = "ChainmailBoots",
    [0x132] = "IronHelmet",
    [0x133] = "IronChestplate",
    [0x134] = "IronLeggings",
    [0x135] = "IronBoots",
    [0x136] = "DiamondHelmet",
    [0x137] = "DiamondChestplate",
    [0x138] = "DiamondLeggings",
    [0x139] = "DiamondBoots",
    [0x13A] = "GoldHelmet",
    [0x13B] = "GoldChestplate",
    [0x13C] = "GoldLeggings",
    [0x13D] = "GoldBoots",
    [0x13E] = "Flint",
    [0x13F] = "Pork",
    [0x140] = "GrilledPork",
    [0x141] = "Painting",
    [0x142] = "GoldenApple",
    [0x143] = "Sign",
    [0x144] = "WoodenDoor",
    [0x145] = "Bucket",
    [0x146] = "WaterBucket",
    [0x147] = "LavaBucket",
    [0x148] = "MineCart",
    [0x149] = "Saddle",
    [0x14A] = "IronDoor",
    [0x14B] = "Redstone",
    [0x14C] = "Snowball",
    [0x14D] = "Boat",
    [0x14E] = "Leather",
    [0x14F] = "MilkBucket",
    [0x150] = "ClayBrick",
    [0x151] = "ClayBalls",
    [0x152] = "Reed",
    [0x153] = "Paper",
    [0x154] = "Book",
    [0x155] = "SlimeBall",
    [0x156] = "StorageMinecart",
    [0x157] = "PoweredMinecart",
    [0x158] = "Egg",
    [0x159] = "Compass",
    [0x15A] = "FishingRod",
    [0x15B] = "Watch",
    [0x15C] = "LightstoneDust",
    [0x15D] = "RawFish",
    [0x15E] = "CookedFish",
    [0x8D0] = "GoldRecord",
    [0x8D1] = "GreenRecord"
};

ITEM_IDS = {}

-- Generate reverse lookup table mapping names to IDs
for i, v in ipairs(ITEM_NAMES) do
    ITEM_IDS[v] = i;
end
for i = 1, #BLOCK_NAMES, 1 do
    ITEM_IDS[BLOCK_NAMES[i]] = i - 1;
end


function ItemID(id)
    return ITEM_IDS[name];
end


function ItemName(id)
    if(ITEM_NAMES[id] ~= nil) then
        return ITEM_NAMES[id];
    end
    if(BLOCK_NAMES[id + 1] ~= nil) then
        return BLOCK_NAMES[id + 1];
    end
    return "UNKNOWN";
end

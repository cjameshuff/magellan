-- Add item(s) to inventory

if(#ARGV < 4) then
    print("\naddinv usage:");
    print("\tmagellan addinv WORLDNUM ITEM ITEMCOUNT");
    print("\nItem and block names are those listed in mcdefs.lua");
    print("\nIf item count is not specified, 1 is assumed.");
    print("\nItem(s) will be placed in first empty inventory position encountered.");
    os.exit()
end

run_script("mcdefs.lua");

WORLDPATH = MCPATH .. "/saves/World" .. ARGV[3]

LEVELDATPATH = WORLDPATH .. "/level.dat"

ITEM = ARGV[4];
ITEM_ID = ItemID(ITEM);
ITEMCOUNT = ARGV[5] or 1;

if(ITEM_ID == nil) then
    print("Unknown item type specified.");
    os.exit();
end

if(os.rename(LEVELDATPATH, LEVELDATPATH .. ".bkp") == nil) then
    print("Could not backup data file: " .. LEVELDATPATH);
    os.exit()
end

nbt = NBT.load(LEVELDATPATH .. ".bkp");
data = nbt:get("Data");
player = data:get("Player");
inventory = player:get("Inventory");
--print(inventory);


print(string.format("Inventory items: %d", inventory:size()));

invslots = {};
for i = 0, 35, 1 do
    invslots[i] = true;
end

for i = 0, inventory:size() - 1, 1 do
    local item = inventory:get(i);
    local slot = item:get("Slot"):val();
--    print(string.format("%d of %s in slot %d, damage %d", count, ItemName(id), slot, damage));
    invslots[slot] = false;
end

itemAdded = false;
for i = 0, 35, 1 do
    if(invslots[i] == true) then
        print(string.format("Adding item to slot %d", i));
        item = NBT.new_compound("");
        item:set(NBT.new_short("id", ITEM_ID))
        item:set(NBT.new_short("Damage", 0))
        item:set(NBT.new_byte("Count", ITEMCOUNT))
        item:set(NBT.new_byte("Slot", i))
        inventory:push_back(item);
        itemAdded = true;
        break;
    end
end

if(not itemAdded) then
    print("No empty item slots!");
    os.exit();
end

nbt:write(LEVELDATPATH);

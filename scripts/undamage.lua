-- Remove damage from all inventory items, including armor. One parameter, world number.

if(#ARGV < 3) then
    print("\nundamage.lua usage:");
    print("\tmagellan undamage.lua WORLDNUM");
    os.exit()
end

run_script("alphadefs.lua");

WORLDPATH = MCPATH .. "/saves/World" .. ARGV[3]

LEVELDATPATH = WORLDPATH .. "/level.dat"

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

for i = 0, inventory:size() - 1, 1 do
    local item = inventory:get(i);
    local id = item:get("id"):val();
    local damage = item:get("Damage"):val();
    local count = item:get("Count"):val();
    local slot = item:get("Slot"):val();
    if(damage > 0) then
        print(string.format("Repairing %s in slot %d, damage %d", ItemName(id), slot, damage));
        item:get("Damage"):set_short(0);
    end
end

nbt:write(LEVELDATPATH);

--print(inventory);

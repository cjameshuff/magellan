-- Dump inventory. One parameter, world number.

if(#ARGV < 3) then
    print("\ndumpinv.lua usage:");
    print("\tnbtutil dumpinv.lua WORLDNUM");
    os.exit()
end

run_script("mcdefs.lua");

WORLDPATH = MCPATH .. "/saves/World" .. ARGV[3]

LEVELDATPATH = WORLDPATH .. "/level.dat"

nbt = NBT.load(LEVELDATPATH);
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
    print(string.format("%d of %s in slot %d, damage %d", count, ItemName(id), slot, damage));
--    print(item);
end

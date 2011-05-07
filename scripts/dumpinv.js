// Dump inventory. One parameter, world name.

if(ARGV.length < 3) {
    println("\ndumpinv usage:");
    println("\tnbtutil dumpinv WORLD");
    exit()
}

run_script("mcdefs.js");

var WORLDPATH = MCPATH + "/saves/" + ARGV[2]

var LEVELDATPATH = WORLDPATH + "/level.dat"

nbt = NBT.load(LEVELDATPATH);
data = nbt.get("Data");
player = data.get("Player");
inventory = player.get("Inventory");


print(nbt);

printf("Inventory items: %@\n", inventory.size());

for(var i = 0; i < inventory.size(); ++i) {
    var item = inventory.get(i);
    var id = item.get("id").value;
    var damage = item.get("Damage").value;
    var count = item.get("Count").value;
    var slot = item.get("Slot").value;
    printf("%@ of %@ in slot %@, damage %@\n", count, ITEM_NAMES[id], slot, damage);
//    println("" + count + " of " + ITEM_NAMES[id] + " in slot " + slot + ", damage " + damage + "");
}

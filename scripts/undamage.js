// Remove damage from all inventory items, including armor. One parameter, world number.

if(ARGV.length < 3) {
    println("\nundamage usage:");
    println("\tmagellan undamage WORLDNUM");
    exit();
}

run_script("mcdefs.js");

WORLDPATH = MCPATH + "/saves/World" + ARGV[2]

LEVELDATPATH = WORLDPATH + "/level.dat"


nbt = NBT.load(LEVELDATPATH);
data = nbt.get("Data");
player = data.get("Player");
inventory = player.get("Inventory");
//println(inventory);

//printf("Inventory items: %@\n", inventory.size());

for(var i = 0; i < inventory.size(); ++i) {
    var item = inventory.get(i);
    var id = item.get("id").value;
    var damage = item.get("Damage").value;
    var count = item.get("Count").value;
    var slot = item.get("Slot").value;
    // Dyes use the damage field to store the dye subtype.
    if(damage > 0 && id != ITEM_IDS["Dye"]) {
//        println("Repairing " + ITEM_NAMES[id] + " in slot " + slot + ", damage " + damage);
        printf("Repairing %@ in slot %@, damage %@\n", ITEM_NAMES[id], slot, damage);
        item.get("Damage").value = 0;
    }
}

var err = os.rename(LEVELDATPATH, LEVELDATPATH + ".bkp");
if(err != 0) {
    printf("Could not backup data file: %@, error %@\n", LEVELDATPATH, err);
    exit()
}
nbt.write(LEVELDATPATH);

//println(inventory);

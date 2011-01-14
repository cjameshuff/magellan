// Add item(s) to inventory

if(ARGV.length < 4) {
    println("\naddinv usage:");
    println("\tmagellan addinv WORLDNUM ITEM ITEMCOUNT");
    println("\nItem and block names are those listed in mcdefs.lua");
    println("\nIf item count is not specified, 1 is assumed.");
    println("\nItem(s) will be placed in first empty inventory position encountered.");
    os.exit()
}

run_script("mcdefs.js");

var WORLDPATH = MCPATH + "/saves/World" + ARGV[2];

var LEVELDATPATH = WORLDPATH + "/level.dat";

var ITEM = ARGV[3];
var ITEM_ID = ITEM_IDS[ITEM];
var ITEMCOUNT = (ARGV.length >= 4)? ARGV[4] : 1;

if(ITEM_ID == null) {
    println("Unknown item type specified.");
    os.exit();
}

var nbt = NBT.load(LEVELDATPATH);
var data = nbt.get("Data");
var player = data.get("Player");
var inventory = player.get("Inventory");
//println(inventory);


printf("Inventory items: %@\n", inventory.size());

invslots = {};
for(var i = 0; i < 36; ++i)
    invslots[i] = true;

for(var i = 0; i < inventory.size(); ++i) {
    var item = inventory.get(i);
    var slot = item.get("Slot").value;
//    printf("%@ of %@ in slot %@, damage %@", count, ITEM_NAMES[id], slot, damage);
    invslots[slot] = false;
}

itemAdded = false;
for(var i = 0; i < 36; ++i)
{
    if(invslots[i] == true)
    {
        printf("Adding %@ to slot %@\n", ITEM_NAMES[ITEM_ID], i);
        item = NBT.new_compound("");
        item.set(NBT.new_short("id", ITEM_ID))
        item.set(NBT.new_short("Damage", 0))
        item.set(NBT.new_byte("Count", ITEMCOUNT))
        item.set(NBT.new_byte("Slot", i))
        inventory.push_back(item);
        itemAdded = true;
        break;
    }
}

if(!itemAdded) {
    println("No empty item slots!");
    os.exit();
}

// Make backup and write new level.dat
var err = os.rename(LEVELDATPATH, LEVELDATPATH + ".bkp");
if(err != 0) {
    printf("Could not backup data file: %@\n", LEVELDATPATH);
    os.exit()
}
nbt.write(LEVELDATPATH);

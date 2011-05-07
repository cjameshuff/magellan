// Add item(s) to inventory

if(ARGV.length < 4) {
    println("\naddinv usage:");
    println("\tmagellan addinv WORLD ITEM ITEMCOUNT");
    println("\nItem and block names are those listed in mcdefs.js");
    println("\nIf item count is not specified, 1 is assumed.");
    println("\nItem(s) will be placed in first empty inventory position encountered.");
    os.exit()
}

run_script("mcdefs.js");

var damage = 0;
var worldName = ARGV[2];

var itemID = null;
if(DYE_IDS[ARGV[3]] != null) {
    itemID = ITEM_IDS["Dye"];
    damage = DYE_IDS[ARGV[3]];
}
else if(WOOD_IDS[ARGV[3]] != null) {
    itemID = ITEM_IDS["Log"];
    damage = WOOD_IDS[ARGV[3]];
}
else if(SLAB_IDS[ARGV[3]] != null) {
    itemID = ITEM_IDS["Slab"];
    damage = SLAB_IDS[ARGV[3]];
}
else if(FUEL_IDS[ARGV[3]] != null) {
    itemID = ITEM_IDS["Coal"];
    damage = FUEL_IDS[ARGV[3]];
}
else {
    itemID = ITEM_IDS[ARGV[3]];
}
if(itemID == null) {
    println("Unknown item type specified.");
    os.exit();
}

var itemCount = (ARGV.length >= 4)? ARGV[4] : 1;

level_dat = LoadLevelDat(worldName);
freeslots = GetFreeInvSlots(level_dat);

var inventory = level_dat.get("Data").get("Player").get("Inventory");
//println(inventory);
printf("Inventory items: %@\n", inventory.size());

if(freeslots.length == 0) {
    println("No empty item slots!");
    os.exit();
}

AddInvItem(level_dat, itemID, damage, itemCount, freeslots[0]);

WriteLevelDat(level_dat, worldName);

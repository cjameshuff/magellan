// Add item(s) to inventory

if(ARGV.length < 4) {
    println("\naddinv usage:");
    println("\tmagellan addinv WORLDNUM ITEM ITEMCOUNT");
    println("\nItem and block names are those listed in mcdefs.js");
    println("\nIf item count is not specified, 1 is assumed.");
    println("\nItem(s) will be placed in first empty inventory position encountered.");
    os.exit()
}

run_script("mcdefs.js");

var worldNum = ARGV[2];
var itemID = ITEM_IDS[ARGV[3]];
var itemCount = (ARGV.length >= 4)? ARGV[4] : 1;
if(itemID == null) {
    println("Unknown item type specified.");
    os.exit();
}

level_dat = LoadLevelDat(worldNum);
freeslots = GetFreeInvSlots(level_dat);

var inventory = level_dat.get("Data").get("Player").get("Inventory");
//println(inventory);
printf("Inventory items: %@\n", inventory.size());

if(freeslots.length == 0) {
    println("No empty item slots!");
    os.exit();
}

AddInvItem(level_dat, itemID, 0, itemCount, freeslots[0]);

WriteLevelDat(level_dat, worldNum);

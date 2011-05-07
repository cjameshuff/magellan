// Add item(s) to inventory

if(ARGV.length != 3) {
    println("\ngivedyes usage:");
    println("\tmagellan addinv WORLD");
    println("\nGives the user a full stack of each dye item, given sufficient space.");
    os.exit()
}

run_script("mcdefs.js");

var world = ARGV[2];

level_dat = LoadLevelDat(world);
freeslots = GetFreeInvSlots(level_dat);

var inventory = level_dat.get("Data").get("Player").get("Inventory");
//println(inventory);
printf("Inventory items: %@\n", inventory.size());

if(freeslots.length < 16) {
    println("Insufficient empty item slots!");
    os.exit();
}

for(var j = 0; j < 16; ++j)
    AddInvItem(level_dat, ITEM_IDS["Dye"], j, 64, freeslots[j]);

WriteLevelDat(level_dat, world);

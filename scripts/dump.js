
if(ARGV.length < 3) {
    println("\ndump.js");
    println("Usage: magellan dump FILE.nbt");
    exit();
}

//for(var i in ARGV)
//    println("ARGV[i]: " + ARGV[i]);

nbt = NBT.load(ARGV[2]);
print(nbt);

// Compute statistics for world.
// One parameter is required, world name. Dimension number may be specified
// as well, 1 for the nether.

if(ARGV.length < 3) {
    println("\nmapstats usage:");
    println("\tnbtutil mapstats WORLD");
    println("\tnbtutil mapstats WORLD DIMENSION");
    exit()
}

var world = ARGV[2];
var dimNum = (ARGV.length > 3)? ARGV[3] : 0;

load_world(world, dimNum);

print_world_stats({ymin: 1, ymax: 127})

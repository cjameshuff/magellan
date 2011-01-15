// Compute statistics for world.
// One parameter is required, world number. Dimension number may be specified
// as well, 1 for the nether.

if(ARGV.length < 3) {
    println("\nmapstats usage:");
    println("\tnbtutil mapstats WORLDNUM");
    println("\tnbtutil mapstats WORLDNUM DIMENSION");
    exit()
}

var worldNum = ARGV[2];
var dimNum = (ARGV.length > 3)? ARGV[3] : 0;

load_world(worldNum, dimNum);

print_world_stats({ymin: 1, ymax: 127})

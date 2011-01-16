
if(ARGV.length < 3) {
    println("\nmapoverview usage:");
    println("\tnbtutil mapoverview WORLDNUM");
    println("\tnbtutil mapoverview WORLDNUM DIMENSION");
    exit()
}

var worldNum = ARGV[2];
var dimNum = (ARGV.length > 3)? ARGV[3] : 0;
var outputName = null;

if(dimNum == 0)
    outputName = "world" + worldNum + ".png";
else
    outputName = "world" + worldNum + "_nether" + ".png";

load_world(worldNum, dimNum);

render_map({
    output_file: outputName,
    render_mode: "alt",// alt, altgray, day, night, morning, evening
    scale: 1, // tile size, 1, 2, 4, 8, or 16. 1: 1 pixel, 2: 2x2 pixel tiles, 4: 4x4...
    ymin: 1, ymax: 127
})

print_world_stats({ymin: 1, ymax: 127})

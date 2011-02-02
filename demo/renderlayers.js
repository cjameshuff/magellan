println("maprender.js")

//load_world(WORLD_NUM[, DIMENSION])
// dimension, 0 for normal world, 1 for nether
load_world(2, 0)

for(var y = 1; y < 127; ++y) {
    printf("Rendering layer %@\n", y);
    render_map({
        output_file: "world2/layer" + y + ".png",
//        render_mode: "evening",// alt, altgray, day, night, morning, evening
        render_mode: "alt",// alt, altgray, day, night, morning, evening
        scale: 4, // tile size, 1, 2, 4, 8, or 16. 1: 1 pixel, 2: 2x2 pixel tiles, 4: 4x4...
        peel: 0,  // "peel back" blocks to an open area underneath, this many times.
        // Not the number of blocks to peel back.
        // Or in NSEW boundary coordinates (reversed in direction from the chunk coordinates):
    //    west: -15,  east: 50,  // increasing eastward
    //    north: 5, south: -40, // increasing northward
        ymin: y - 8, ymax: y
    })
}

// Some additional notes: keep in mind that memory requirements for the image increase
// with the square of scale. The render area limits can be omitted, rendering the entire
// world into a single map, but this can easily consume available memory.

print_world_stats({ymin: 1, ymax: 127})
// print_world_stats() can take coordinates defining a specific region
//print_world_stats({north: 0, south: -10, etc...})

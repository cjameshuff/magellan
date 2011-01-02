print("maprender.lua\n")

load_world({
    world = 4, -- world number
    dim = 0,   -- dimension, 0 for normal world, 1 for nether
})

render_map({
    output_file = "renderout.png",
    render_mode = "night",-- alt, altgray, day, night, morning, evening
    scale = 2, -- pixels per tile, 1, 2, 4, 8, or 16
    peel = 0,  -- "peel back" blocks to an open area underneath, this many times.
    -- Not the number of blocks to peel back.
    layers = false, -- render all layers in range
    -- Render area can be specified in chunk coordinates:
--    zmin = 5, zmax = 15, -- east-west, increasing westward
--    xmin = 10, xmax = 20, -- north-south, increasing southward
    -- Or in NSEW boundary coordinates (reversed in direction from the chunk coordinates):
--     west = -15,  east = 0,  -- increasing eastward
--    north = 0, south = -15, -- increasing northward
    ymin = 1, ymax = 127
})

print_world_stats({ymin = 1, ymax = 127})
-- print_world_stats() can take coordinates defining a specific region
--print_world_stats({north = 0, south = -10, etc...})

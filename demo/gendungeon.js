

run_script("mcdefs.js");
run_script("maptools.js");

function irand(mn, mx) {return Math.floor(mn + (mx - mn)*Math.random());}



// '#' indicates locations outside the dungeon
// '.' indicates open room space
// ',' indicates unavailable room space (in front of doors, etc)
// 'X' indicates wall
// 'T' indicates wall junctions
// 'F' indicates door frames

function Dungeon(_xSize, _zSize) {
    this.xSize = _xSize;
    this.zSize = _zSize;
    this.floorplan = null;
    this.workbuffer = null;
    this.rooms = [];
};


function CreateMatrix(xMin, xMax, yMin, yMax, fillVal) {
    var matrix = [];
    for(var x = xMin; x <= xMax; ++x) {
        matrix[x] = [];
        for(var y = yMin; y <= yMax; ++y)
            matrix[x][y] = fillVal;
    }
    return matrix;
}
function CreateMatrixWith(xMin, xMax, yMin, yMax, fillFn) {
    var matrix = [];
    for(var x = xMin; x <= xMax; ++x) {
        matrix[x] = [];
        for(var y = yMin; y <= yMax; ++y)
            matrix[x][y] = fillFn(x, y);
    }
    return matrix;
}

function CopyMatrix(xMin, xMax, yMin, yMax, dst, src) {
    for(var x = xMin; x <= xMax; ++x)
        for(var y = yMin; y <= yMax; ++y)
            dst[x][y] = src[x][y];
}

Dungeon.prototype.AddRoomSeeds = function(numRooms)
{
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    var rooms = this.rooms;
    
    while(rooms.length < numRooms) {
        var room = {
            x: irand(2, xSize - 2),
            z: irand(2, zSize - 2)
        };
        printf("Trying room at %@, %@\n", room.x, room.z);
        
        var accept = true;
        for(r in rooms) {
            if(Math.abs(rooms[r].x - room.x) < 3 &&
               Math.abs(rooms[r].z - room.z) < 3)
            {
                accept = false;
                break;
            }
        }
        
        if(accept) {
            printf("Added room at %@, %@\n", room.x, room.z);
            rooms[rooms.length] = room;
            workbuffer[room.x][room.z] = '.';
            room.xmin = room.x;
            room.xmax = room.x;
            room.zmin = room.z;
            room.zmax = room.z;
        }
    }
    CopyMatrix(0, this.xSize, 0, this.zSize, this.floorplan, this.workbuffer);
}

Dungeon.prototype.FindBoundaries = function()
{
    // If a tile is next to an open area, it is marked as a wall
    // Only looks at open areas, and may be called multiple times
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
            if(floorplan[x][z] == '#' &&
               (floorplan[x-1][z] == '.' ||
               floorplan[x+1][z] == '.' ||
               floorplan[x][z-1] == '.' ||
               floorplan[x][z+1] == '.' ||
               floorplan[x-1][z-1] == '.' ||
               floorplan[x+1][z-1] == '.' ||
               floorplan[x+1][z+1] == '.' ||
               floorplan[x-1][z+1] == '.'))
            {
                workbuffer[x][z] = 'X';
            }
        }
    }
    CopyMatrix(0, this.xSize, 0, this.zSize, this.floorplan, this.workbuffer);
}

Dungeon.prototype.FindJoins = function()
{
    // If a tile is a connecting point of 2 or more walls, mark as such
    // Should only be called once, as it "pollutes" the wall data by marking joins.
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
            if(floorplan[x][z] != 'X')
                continue;
            
            // Count walls in each cardinal direction. A wall is considered to exist
            // if two consecutive blocks in a given direction are wall blocks, this
            // prevents two-block-thick walls from being marked entirely as joins.
            // Still fails in some corner cases on 2 block thick walls, but this
            // failure should be harmless.
            var count = 0;
            count += (floorplan[x-1][z] == 'X' && floorplan[x-2][z] == 'X')? 1 : 0;
            count += (floorplan[x+1][z] == 'X' && floorplan[x+2][z] == 'X')? 1 : 0;
            count += (floorplan[x][z-1] == 'X' && floorplan[x][z-2] == 'X')? 1 : 0;
            count += (floorplan[x][z+1] == 'X' && floorplan[x][z+2] == 'X')? 1 : 0;
            if(count > 2)
                workbuffer[x][z] = 'T';
        }
    }
    CopyMatrix(0, this.xSize, 0, this.zSize, this.floorplan, this.workbuffer);
}

Dungeon.prototype.DecorateWalls = function()
{
    // Add things like door frames, corner decorations, etc.
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    
    CopyMatrix(0, this.xSize, 0, this.zSize, this.workbuffer, this.floorplan);
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
            // Mark door frames: wall columns adjacent to doorways.
            if(floorplan[x][z] == 'X' || floorplan[x][z] == 'T')
            {
                var count = 0;
                count += (floorplan[x-1][z] == '_')? 1 : 0;
                count += (floorplan[x+1][z] == '_')? 1 : 0;
                count += (floorplan[x][z-1] == '_')? 1 : 0;
                count += (floorplan[x][z+1] == '_')? 1 : 0;
                if(count > 0)
                    workbuffer[x][z] = 'F';
            }
            
            // Mark space in front of doorways...keep doors from being blocked by items
            if(floorplan[x][z] == '.')
            {
                var count = 0;
                count += (floorplan[x-1][z] == '_')? 1 : 0;
                count += (floorplan[x+1][z] == '_')? 1 : 0;
                count += (floorplan[x][z-1] == '_')? 1 : 0;
                count += (floorplan[x][z+1] == '_')? 1 : 0;
                if(count > 0)
                    workbuffer[x][z] = ',';
            }
        }
    }
    CopyMatrix(0, this.xSize, 0, this.zSize, this.floorplan, this.workbuffer);
}

Dungeon.prototype.GrowRoom = function(room)
{
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    var rooms = this.rooms;
    
    
    var grew = false;
    // TODO: randomize selection of growth axis to remove bias in growth direction
    
    // grow along z axis
    // min and max blocks are of the room interior. +- 1 is in the wall, we want to
    // see if we can push the wall out....check the far side of the wall, then erase wall.
    var clearLow = true;
    var clearHigh = true;
    for(var x = room.xmin-1; x <= room.xmax+1; ++x) {
        if(floorplan[x][room.zmin - 2] == '.')
            clearLow = false;
        if(floorplan[x][room.zmax + 2] == '.')
            clearHigh = false;
    }
    
    if(clearLow && room.zmin > 1) {
        --(room.zmin);
        for(var x = room.xmin; x <= room.xmax; ++x)
            workbuffer[x][room.zmin] = '.';
    }
    
    if(clearHigh && room.zmax < zSize - 2) {
        ++(room.zmax);
        for(var x = room.xmin; x <= room.xmax; ++x)
            workbuffer[x][room.zmax] = '.';
    }
    grew = clearLow || clearHigh;
    
    // Now do x axis
    clearLow = true;
    clearHigh = true;
    for(var z = room.zmin-1; z <= room.zmax+1; ++z) {
        if(floorplan[room.xmin - 2][z] == '.')
            clearLow = false;
        if(floorplan[room.xmax + 2][z] == '.')
            clearHigh = false;
    }
    
    if(clearLow && room.xmin > 1) {
        --(room.xmin);
        for(var z = room.zmin; z <= room.zmax; ++z)
            workbuffer[room.xmin][z] = '.';
    }
    
    if(clearHigh && room.xmax < xSize - 2) {
        ++(room.xmax);
        for(var z = room.zmin; z <= room.zmax; ++z)
            workbuffer[room.xmax][z] = '.';
    }
    grew |= clearLow || clearHigh;
    
    CopyMatrix(0, this.xSize, 0, this.zSize, this.floorplan, this.workbuffer);
    
    return grew;
}

function append(dest, entry) {dest[dest.length] = entry;}

// Inserts entry if doesn't exist. Returns index of item, inserted or existing.
function insert_unique(dest, entry, testFn) {
    var exists = false;
    for(e in dest)
        if(testFn(dest[e], entry))
            return e;
    
    dest[dest.length] = entry;
    return dest.length - 1;
}

Dungeon.prototype.GeneratePassages = function()
{
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    var rooms = this.rooms;
    // Walls: matrix of arrays of room IDs
    // wallLocs: list of wall locations, XZ coordinates in 2 element arrays
    var walls = CreateMatrixWith(0, xSize, 0, zSize, function(x, z) {return new Array();});
    var wallLocs = new Array();
    // Save adjacent rooms in walls matrix
    for(var r = 0; r < rooms.length; ++r)
    {
        var room = rooms[r];
        
        // Add some additional members we'll be needing
        room.neighbors = new Array();
        room.connTo = new Array();
        room.connFrom = new Array();
        
        // Go along the walls of each room, adding that room to the entry in the walls
        // matrix. Don't bother with the corners.
        for(var x = room.xmin; x <= room.xmax; ++x) {
            walls[x][room.zmin-1].push(r);
            walls[x][room.zmax+1].push(r);
        }
        for(var z = room.zmin; z <= room.zmax; ++z) {
            walls[room.xmin-1][z].push(r);
            walls[room.xmax+1][z].push(r);
        }
    }
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
            if(walls[x][z].length >= 2)
                wallLocs.push([x, z]);
            print(' ');
            print(walls[x][z].length);
        }
        print('\n');
    }
    
    // Compute neighboring rooms for each room.
    for(var l = 0; l < wallLocs.length; ++l)
    {
        var loc = wallLocs[l];
        var wallPart = walls[loc[0]][loc[1]];
//        printf("loc: %@\n", loc);
//        printf("wallPart.length: %@\n", wallPart.length);
        
        var roomID0 = wallPart[0];
        var roomID1 = wallPart[1];
        var room0 = rooms[roomID0];
        var room1 = rooms[roomID1];
//        printf("roomID0: %@, roomID1: %@\n", roomID0, roomID1);
        
        var entry = insert_unique(room0.neighbors,
             {id: roomID1, locs: new Array()},
             function(a, b) {return a.id == b.id;})
        room0.neighbors[entry].locs.push(loc);
        
        entry = insert_unique(room1.neighbors,
             {id: roomID0, locs: new Array()},
             function(a, b) {return a.id == b.id;})
        room1.neighbors[entry].locs.push(loc);
    }
    
//    for(var r = 0; r < rooms.length; ++r)
//    {
//        var room = rooms[r];
//        printf("room: %@ has %@ neighbors\n", r, room.neighbors.length);
//    }
    
    // Connect rooms
    // Start at one room (0 is as good as any other) and "flood" map, connecting
    // all unconnected rooms.
    // Store newly connected rooms in active list.
    var activeList = [0];// rooms to connect from
    var connectedList = [];
    var connectedRooms = 0;
    while(connectedRooms < rooms.length && activeList.length > 0)
    {
        // TODO: scramble order of active list to remove bias in connection order
        for(var r in activeList)
        {
            var room = rooms[activeList[r]];
//            printf("room %@ has %@ neighbors\n", activeList[r], room.neighbors.length);
            for(var n in room.neighbors)
            {
                var neighbor = rooms[room.neighbors[n].id];
                if(neighbor.connFrom.length == 0)
                {
                    printf("room %@ connects to %@\n", activeList[r], room.neighbors[n].id);
                    // Make connection
                    append(room.connTo, n);// store neighbor index of connection
                    append(neighbor.connFrom, activeList[r]);
                    
                    append(connectedList, room.neighbors[n].id);
                    ++connectedRooms;
                }
            }
        }
        printf("Connected %@ rooms\n", connectedList.length);
        activeList = connectedList;
        connectedList = [];
    }
    
    printf("Connected %@ rooms in total\n", connectedRooms);
    if(connectedRooms < rooms.length) {
        println("Error, could not connect all rooms!");
    }
    
    
    // Make actual connecting passages
    for(var r = 0; r < rooms.length; ++r)
    {
        var room = rooms[r];
        for(var n in room.connTo) {
            var locs = room.neighbors[room.connTo[n]].locs;
            if(locs.length == 0) {
                println("Error, empty locs entry!");
                continue;
            }
            // TODO: Put door a block away from room corners if possible
            var loc = locs[irand(0, locs.length)];
            floorplan[loc[0]][loc[1]] = '_';
        }
//        printf("room: %@ has %@ neighbors\n", r, room.neighbors.length);
    }
}

Dungeon.prototype.PopulateRooms = function()
{
//    for(var r in rooms) {
//        var room = rooms[r];
//    }
}


Dungeon.prototype.GenerateFloorplan = function(numRooms)
{
    var xSize = this.xSize;
    var zSize = this.zSize;
    var rooms = this.rooms;
    // Initialize floorplan
    // Set available area to free.
    // Initialize boundaries to room interior to keep rooms from growing outside.
    // The fact that JS arrays can have negative indices is rather helpful here.
    this.floorplan = CreateMatrix(-1, xSize + 1, -1, zSize + 1, "#");
    this.workbuffer = CreateMatrix(-1, xSize + 1, -1, zSize + 1, "#");
    
    var floorplan = this.floorplan;
    var workbuffer = this.workbuffer;
    for(var x = -1; x <= xSize; ++x) {
        floorplan[x][-1] = '.';
        floorplan[x][zSize] = '.';
        workbuffer[x][-1] = '.';
        workbuffer[x][zSize] = '.';
    }
    for(var z = -1; z <= zSize; ++z) {
        floorplan[-1][z] = '.';
        floorplan[xSize][z] = '.';
        workbuffer[-1][z] = '.';
        workbuffer[xSize][z] = '.';
    }
    
    this.AddRoomSeeds(numRooms);
    
    // Expand rooms to fill available space
    for(var j = 0; j < 100; ++j) {
        this.FindBoundaries();
        var grew = false;
        for(var r in rooms)
            grew |= this.GrowRoom(rooms[r]);
        
        if(!grew) break;
    }
    
    // Do a final FindBoundaries() step and find the wall joins
    this.FindBoundaries();
    this.FindJoins();
    this.GeneratePassages();
    this.DecorateWalls();
    this.PopulateRooms();
    
    return floorplan;
}


Dungeon.prototype.PrintFloorplan = function() {
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
            print(' ');
            print(floorplan[x][z]);
        }
        print('\n');
    }
}

// Make an axis-aligned rectangular prism
function MakeSlab(xmin, xmax, ymin, ymax, zmin, zmax, type) {
    for(var x = xmin; x <= xmax; ++x) {
        for(var y = ymin; y <= ymax; ++y) {
            for(var z = zmin; z <= zmax; ++z) {
                world.set_block(x + xoff, yoff, z + zoff, type);
            }
        }
    }
}
function MakeSlabFn(xmin, xmax, ymin, ymax, zmin, zmax, typeFn) {
    for(var x = xmin; x <= xmax; ++x) {
        for(var y = ymin; y <= ymax; ++y) {
            for(var z = zmin; z <= zmax; ++z) {
                world.set_block(x + xoff, yoff, z + zoff, typeFn(x, y, z));
            }
        }
    }
}

Dungeon.prototype.PlaceDungeon = function(world, xoff, yoff, zoff)
{
    var xSize = this.xSize;
    var zSize = this.zSize;
    var floorplan = this.floorplan;
    
    var airID = BLOCK_IDS["Air"];
    var glassID = BLOCK_IDS["Air"];
    var logID = BLOCK_IDS["Log"];
    var stoneID = BLOCK_IDS["Stone"];
    var cstoneID = BLOCK_IDS["Cobblestone"];
    var mossStoneID = BLOCK_IDS["MossyCobblestone"];
    
    var bl = 0;
    var sl = 0;
    
//    for(var y = 0; y < 128; ++y) {
//        var block = world.get_block(xoff+3, y, zoff+1);
//        printf("%@] type: %@ bl: %@ sl: %@ data: %@\n",
//          y, block.type, block.blocklight, block.skylight, block.data);
//    }
//    for(var x = 0; x < 64; ++x) {
//        for(var z = 0; z < 64; ++z) {
//        }
//    }
    
    for(var x = 0; x < xSize; ++x) {
        for(var z = 0; z < zSize; ++z) {
//            for(var y = 0; y < 15; ++y)
//                world.set_block(x + xoff, y + yoff + 1, z + zoff, airID, 0, 15, 0);
            
            // floor
            world.set_block(x + xoff, yoff, z + zoff, stoneID, 0, sl, bl);
            
            // ceiling
            world.set_block(x + xoff, yoff + 5, z + zoff, cstoneID, 0, sl, bl);
            
            for(var y = 1; y < 5; ++y) {
                switch(floorplan[x][z]) {
                  case '.':
                  case ',':// empty space
                    world.set_block(x + xoff, y + yoff, z + zoff, airID, 0, sl, bl);
                  break;
                  case '_':// Door
                    if(y < 3)
                        world.set_block(x + xoff, y + yoff, z + zoff, airID, 0, sl, bl);
                    else if(y == 3)
                        world.set_block(x + xoff, y + yoff, z + zoff, logID, 0, sl, bl);
                    else
                        world.set_block(x + xoff, y + yoff, z + zoff, cstoneID, 0, sl, bl);
                  break;
                  case 'F':
                    // doorframe
                    if(y <= 3)
                        world.set_block(x + xoff, y + yoff, z + zoff, logID, 0, sl, bl);
                    else
                        world.set_block(x + xoff, y + yoff, z + zoff, cstoneID, 0, sl, bl);
                  break;
                  case 'X':
                  case '#':
                  case 'T':
                    // Wall
                    world.set_block(x + xoff, y + yoff, z + zoff, cstoneID, 0, sl, bl);
                  break;
                }
            }
            
            // Recompute heightmap
            for(var y = 127; y > 0; --y) {
                var bt = world.get_block(x, y, z).type;
                if(bt != airID && bt != glassID) {
                    world.set_heightmap(x, z, y+1);// y, or y+1?
//                    printf("Computed hm = %@\n", y);
                    break;
                }
            }
        }
    }
}

var dungeon = new Dungeon(64, 64);

dungeon.GenerateFloorplan(50);

dungeon.PrintFloorplan();


MC_World.prototype.add_tile_entity = function(x, y, z, id) {
    var chunk = this.get_block_chunk_nbt(x, z);
    var level = chunk.get("Level");
    var entities = level.get("Entities");
    var tileEntities = level.get("TileEntities");
    var entity = NBT.new_compound();
    entity.set(NBT.new_string("id", id));
    entity.set(NBT.new_integer("x", x));
    entity.set(NBT.new_integer("y", y));
    entity.set(NBT.new_integer("z", z));
    tileEntities.push_back(entity);
    return entity;
}


var WORLDPATH = MCPATH + "/saves/World" + 3;
printf("WORLDPATH = %@\n", WORLDPATH);

world = new MC_World(WORLDPATH);

dungeon.PlaceDungeon(world, 0, 64, 0);

world.add_chest(4, 65, 4);

world.calc_heightmap();
world.write(WORLDPATH);



//var chunk = world.get_chunk_nbt(0,-1);
//var level = chunk.get("Level");
//var entities = level.get("Entities").contents;
//var tileEntities = level.get("TileEntities").contents;
//
//printf("entities:\n");
//for(e in entities)
//    println(entities[e]);
//printf("\ntile entities:\n");
//for(e in tileEntities)
//    println(tileEntities[e]);
//
//var xoff = 0;
//var zoff = -8;
//var yoff = 65;
//for(var x = 0; x < 8; ++x) {
//    for(var z = 0; z < 8; ++z) {
//        var block = world.get_block(x + xoff, yoff, z + zoff);
//        printf("%@,%@,%@: type: %@ data: %@, ",
//            x + xoff, yoff, z + zoff, block.type, block.data);
//        block = world.get_block(x + xoff, yoff+1, z + zoff);
//        printf("%@: type: %@ data: %@\n",
//            yoff + 1, block.type, block.data);
//    }
//}


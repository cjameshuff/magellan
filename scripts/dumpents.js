// Dump inventory. One parameter, world number.

if(ARGV.length < 3) {
    println("\ndumpents usage:");
    println("\tnbtutil dumpents WORLDNUM");
    exit()
}

run_script("mcdefs.js");

// Chunk format:
//    COMPOUND Level {
//        BYTE_ARRAY Blocks(32768)
//        BYTE_ARRAY Data(16384)
//        BYTE_ARRAY SkyLight(16384)
//        BYTE_ARRAY BlockLight(16384)
//        BYTE_ARRAY HeightMap(16384)
//        
//        TAG_LIST<COMPOUND> Entities
//        TAG_LIST<COMPOUND> TileEntities
//        
//        TAG_LONG LastUpdate
//        
//        TAG_INT xPos
//        TAG_INT zPos
//        
//        TAG_BYTE TerrainPopulated
//    }

function DumpChunk(chunk) {
    var level = chunk.get("Level");
    var lastUpdate = level.get("LastUpdate").value;
    var xPos = level.get("xPos").value;
    var zPos = level.get("zPos").value;
    var terrainPopulated = level.get("TerrainPopulated").value;
    
    printf("\nx = %@, z = %@\n", xPos, zPos);
    printf("LastUpdate: %@, terrainPopulated = %@\n", lastUpdate, terrainPopulated);
    
    var entities = level.get("Entities").contents();
    var tileEntities = level.get("TileEntities").contents();
    printf("entities = %@\n", entities);
    printf("tileEntities = %@\n", tileEntities);
}


var WORLDPATH = MCPATH + "/saves/World" + ARGV[2];
printf("WORLDPATH = %@\n", WORLDPATH);

world = new MC_World(WORLDPATH);

chunks = world.get_all_chunk_nbts();

printf("World has %@ chunks\n", chunks.length);

for(var i in chunks)
    DumpChunk(chunks[i])


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

function DumpEntity(entity) {
    var id = entity.get("id").value;
    var pos = entity.get("Pos").contents;
    var x = pos[0].value;
    var y = pos[1].value;
    var z = pos[2].value;
    printf("\t%@ at < %@, %@, %@>\n", id, x, y, z);
}
function DumpTileEntity(entity) {
    var id = entity.get("id").value;
    var x = entity.get("x").value;
    var y = entity.get("y").value;
    var z = entity.get("z").value;
//    var id = level.get("EntityId").value;// for MobSpawner
    printf("\t%@ at < %@, %@, %@>\n", id, x, y, z);
}

function DumpChunk(chunk) {
    var level = chunk.get("Level");
    var lastUpdate = level.get("LastUpdate").value;
    var xPos = level.get("xPos").value;
    var zPos = level.get("zPos").value;
    var terrainPopulated = level.get("TerrainPopulated").value;
    
    var entities = level.get("Entities").contents;
    var tileEntities = level.get("TileEntities").contents;
    
    if(entities.length > 0 || tileEntities.length > 0) {
        printf("\nChunk: x = %@, z = %@\n", xPos, zPos);
        printf("%@ movable entities, %@ tile entities\n", entities.length, tileEntities.length);
        if(entities.length > 0) {
            printf("tileEntities:\n");
            for(e in entities)
                DumpEntity(entities[e])
        }
        if(tileEntities.length > 0) {
            printf("tileEntities:\n");
            for(e in tileEntities)
                DumpTileEntity(tileEntities[e])
        }
    }
}


var WORLDPATH = MCPATH + "/saves/World" + ARGV[2];
printf("WORLDPATH = %@\n", WORLDPATH);

world = new MC_World(WORLDPATH);

chunks = world.get_all_chunk_nbts();

printf("World has %@ chunks\n", chunks.length);

for(var i in chunks)
    DumpChunk(chunks[i])


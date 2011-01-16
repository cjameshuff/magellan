// Saddles all pigs in world

if(ARGV.length < 3) {
    println("\nUsage:");
    println("\tmagellan demo/saddleup.js WORLDNUM");
    exit()
}

run_script("mcdefs.js");

var WORLDPATH = MCPATH + "/saves/World" + ARGV[2];
printf("WORLDPATH = %@\n", WORLDPATH);

world = new MC_World(WORLDPATH);

chunks = world.get_all_chunk_nbts();

printf("World has %@ chunks\n", chunks.length);

for(var i in chunks) {
    var chunk = chunks[i];
    var level = chunk.get("Level");
    var entities = level.get("Entities").contents();
    
    for(e in entities) {
        var entity = entities[e];
        var id = entity.get("id").value;
        var pos = entity.get("Pos").contents();
        var x = pos[0].value;
        var y = pos[1].value;
        var z = pos[2].value;
        if(id == "Pig") {
            printf("\tSaddled %@ at < %@, %@, %@>\n", id, x, y, z);
            entity.get("Saddle").value = 1;
        }
    }
}

world.write(WORLDPATH);

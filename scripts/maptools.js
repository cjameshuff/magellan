// Various tools useful in manipulating Minecraft maps

run_script("mcdefs.js");

MC_World.prototype.for_each_chunk_nbt = function(fn) {
    var chunks = this.get_all_chunk_nbts();
    for(var i in chunks)
        fn(chunks[i]);
}

MC_World.prototype.for_each_entity_nbt = function(fn) {
    var chunks = this.get_all_chunk_nbts();
    for(var i in chunks) {
        var chunk = chunks[i];
        var level = chunk.get("Level");
        var entities = level.get("Entities").contents;
        
        for(e in entities)
            fn(entities[e]);
    }
}

MC_World.prototype.for_each_tile_entity_nbt = function(fn) {
    var chunks = this.get_all_chunk_nbts();
    for(var i in chunks) {
        var chunk = chunks[i];
        var level = chunk.get("Level");
        var entities = level.get("TileEntities").contents;
        
        for(e in entities)
            fn(entities[e]);
    }
}


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

// Creates a chest and returns its NBT entry
MC_World.prototype.add_chest = function(x, y, z) {
    this.set_block(x, y, z, BLOCK_IDS["Chest"], 0, 0, 0);
    var chestNBT = this.add_tile_entity(x, y, z, "Chest");
    chestNBT.set(NBT.new_list("Items", NBT.TAG_Compound));
    return chestNBT;
}

MC_World.prototype.add_music = function(x, y, z, note) {
    this.set_block(x, y, z, BLOCK_IDS["NoteBlock"], 0, 0, 0);
    var noteBlockNBT = this.add_tile_entity(x, y, z, "Music");
    noteBlockNBT.set(NBT.new_byte("note", note));
    return noteBlockNBT;
}

MC_World.prototype.add_mob_spawner = function(x, y, z, mob, delay) {
    this.set_block(x, y, z, BLOCK_IDS["MobSpawner"], 0, 0, 0);
    var mobSpawnerNBT = this.add_tile_entity(x, y, z, "MobSpawner");
    mobSpawnerNBT.set(NBT.new_string("EntityID", mob));
    mobSpawnerNBT.set(NBT.new_short("Delay", delay));
    return mobSpawnerNBT;
}

MC_World.prototype.add_sign = function(x, y, z, lines) {
    this.set_block(x, y, z, BLOCK_IDS["SignPost"], 0, 0, 0);
    var signNBT = this.add_tile_entity(x, y, z, "Sign");
    for(var j = 0; j < 4; ++j) {
        if(lines.length > j)
            signNBT.set(NBT.new_string("Text" + j, lines[0]));
        else
            signNBT.set(NBT.new_string("Text" + j, ""));
    }
    return signNBT;
}

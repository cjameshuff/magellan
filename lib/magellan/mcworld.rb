
require 'magellan'
require 'magellan/mcdefs'
require 'magellan/mcentity'

module Magellan

# The MC_World class manages the various regions and chunks a Minecraft world
# is composed of, and provides facilities for editing and creating such worlds.

# Chunks do not have a full class at this time, they are simple hashes:
# chunk = {
#     region: region,
#     region_coords: [region_chunk_x, region_chunk_z],
#     nbt: chunk_nbt,
#     blocks: chunk_nbt[:Level][:Blocks].value,
#     block_data: chunk_nbt[:Level][:Data].value,
#     dirty: true/false,
#     accessed: incrementing integer
# }


class MC_World
    attr_reader :level_dat, :world_dir, :world_name
    
    CHUNK_COORDS = (0..31).to_a.product((0..31).to_a)
    
    def initialize(opts = {})
        # @gen_chunks = opts.fetch(:gen_chunks, true)
        @all_regions = {}
        if(opts[:world_dir])
            load_world(opts[:world_dir]})
        elsif(opts[:world_name])
            load_world("#{MCPATH}/saves/#{opts[:world_name]}")
        end
    end
    
    # Load world...opens all region files and level.dat for world, does not load any chunks.
    def load_world(world_dir)
        @world_dir = world_dir
        @world_name = world_dir.split('/')[-1]
        regionFiles = Dir.entries(@world_dir + "/region")
        regionFiles.delete('.')
        regionFiles.delete('..')
        
        regionFiles.each {|fin|
            # Extract coordinates from file name. File name format is r.X.Y.mcr
            coords = fin.split('.')[1, 2]
            region = MCRegion.new
            if(region.open("#{@world_dir}/#{fin}") == 0)
                @all_regions[coords] = region
            end
        }
        coords = @all_regions.keys
        @minmax_x = (coords.map {|coord| coord[0]}).minmax
        @minmax_z = (coords.map {|coord| coord[1]}).minmax
        
        @level_dat = MC_LevelDat.new({world_dir: world_dir})
        
        @slock = File.new("#{@world_dir}/session.lock", "wb")
        
        # Update session.lock
        @slock.rewind
        ts = mc_timestamp()
        tsbytes = (0..7).map {|x| (ts >> (8*(7-x))) & 0xFF}
        @slock.write(tsbytes.pack("CCCCCCCC"))
        
        @chunks = {}
    end
    
    def compute_lights()
        compute_lights_intern()
    end
    
    def compute_heights()
        compute_heights_intern()
    end
    
    def mc_timestamp()
        ts = (Time.now.to_r*1000).to_i
    end
    
    # Generate a new chunk for each empty chunk in specified area
    # def fill_empty_chunks(xrange, zrange)
    # end
    
    # Iterate over each non-empty chunk in the world, calling block on each.
    def each_chunk(fn)
        @all_regions.each_key {|rgmcoord|
            CHUNK_COORDS.each {|chunkcoord|
                chunk = get_chunk(rgmcoord[0] + chunkcoord[0], rgmcoord[1] + chunkcoord[1])
                if(chunk)
                    yield(chunk)
                end
            }
        }
    end

    def each_entity_nbt(fn)
        each_chunk_nbt {|chunk|
            level = chunk[:Level]
            entities = level[:Entities].value
            entities.each {|ent| yield(ent)}
        }
    end

    def each_tile_entity_nbt(fn)
        each_chunk_nbt {|chunk|
            level = chunk[:Level]
            entities = level[:TileEntities].value
            entities.each {|ent| yield(ent)}
        }
    end
    
    def get_stats()
        stats = {}
        stats[:world_name] = @world_name
        stats[:world_dir] = @world_dir
        stats[:num_regions] = @all_regions.size
        stats[:range_x] = @minmax_x
        stats[:range_z] = @minmax_z
        stats[:total_chunks] = @all_regions.size*1024
        stats[:empty_chunks] = 0
        @all_regions.each {|rgn|
            CHUNK_COORDS.each {|coord|
                if(!rgn.chunk_exists(coord[0], coord[1]))
                    stats[:empty_chunks] += 1
                end
            }
        }
        stats
    end
    
    # Get NBT for chunk containing XZ block coordinates
    # Parameters are world XZ coordinates for any column of blocks in the desired chunk.
    # Loads NBT for chunk containing coordinates if not already loaded, or creates new one with
    # fill_empty_chunk() if chunk doesn't yet exist.
    # TODO: unload a chunk if more than a maximum number are loaded. Unload clean chunk if any
    # exist unless it was just loaded, otherwise write and unload least recently accessed chunk.
    def load_chunk(x, z)
        chunk_nbt = nil
        region = @all_regions[[x/512, z/512]]
        
        if(region)
            region_chunk_x = (x/16) & 31
            region_chunk_z = (z/16) & 31
            if(region.chunk_exists(region_chunk_x, region_chunk_z))
                chunk_nbt = region.read_chunk_nbt(region_chunk_x, region_chunk_z)
            end
        end
        
        if(chunk_nbt != nil)
            chunk = {
                region: region,
                region_coords: [region_chunk_x, region_chunk_z],
                nbt: chunk_nbt,
                blocks: chunk_nbt[:Level][:Blocks].value,
                block_data: chunk_nbt[:Level][:Data].value,
                dirty: false,
                accessed: 0
            }
            @chunks[[x/16, z/16]] = chunk
            chunk
        else
            nil
        end
    end
    
    def unload_chunk(chunk)
        chunk = @chunks.delete(chunk[:region_coords) {raise "Attempt to unload chunk that isn't loaded"}
        if(chunk[:dirty])
            write_chunk(chunk)
        end
    end
    
    def write_chunk(chunk)
        # TODO: verify this. The value is described in the wiki as
        # "Tick when the chunk was last saved". Unclear whether
        # this is the same quantity written to the session lock file.
        chunk[:nbt][:Level][:LastUpdate].value = mc_timestamp()
        chunk[:region].write_chunk_nbt(x, z, chunk[:nbt])
        chunk[:dirty] = false
    end
    
    # Write all loaded/modified chunks to disk
    def write_chunks()
        @chunks.each {|chunk|
            if(chunk[:dirty])
                write_chunk(chunk)
            end
        }
    end
    
    # Get the chunk containing given coordinates
    # Parameters are world XZ coordinates for any column of blocks in the desired chunk.
    # If chunk not already loaded, attempts to load it.
    # If chunk doesn't exist, yields to block, passing chunk coordinates and returning result.
    # Do something like:
    # 
    # get_chunk(x, z) {|x, z| load_chunk(x, z)}
    # 
    # to automatically load chunks on access.
    def get_chunk(x, z)
        chunk = @chunks.fetch([x/16, z/16]) {load_chunk(x, z)}
        if(chunk == nil)
            chunk = yield(x, z)
        end
        if(chunk != nil)
            chunk[:accessed] = (@access_ctr += 1)
        end
        chunk
    end
    
    # region coordinates of region containing block
    def get_region_coords(block_coords)
        [block_coords[0]/512, block_coords[1]/512]
    end
    
    # chunk coordinates of chunk containing block
    def get_chunk_coords(block_coords)
        [block_coords[0]/16, block_coords[10]/16]
    end
    
    # Sets block type and data.
    def set_block2(x, y, z, bid, data)
        chunk = get_chunk(x, z)
        chunk[:dirty] = true
        
        blocks = chunk[:nbt][:Level][:Blocks].value
        block_data = chunk[:nbt][:Level][:Data].value
        
        # Set block
        bidx = ((x & 15)*16 + (z & 15))*128 + y
        blocks.setbyte(bidx, bid)
        
        # Set data
        halfidx = bidx >> 1;
        data_byte = block_data.getbyte(halfidx)
        if(bidx & 0x01)
            data_byte = (data << 4) | (data_byte & 0x0F);
        else
            data_byte = (data_byte & 0xF0) | (data & 0x0F);
        end
        block_data.setbyte(halfidx, data_byte)
    end
    
    # Get block type and data
    def get_block2(x, y, z)
        chunk = get_chunk(x, z)
        bidx = ((x & 15)*16 + (z & 15))*128 + y
        [chunk[:blocks].getbyte(bidx), chunk[:block_data].getbyte(bidx >> 1)]
    end
    
    # Find first empty block location immediately above a non-empty block,
    # starting from given location.
    def find_drop_pt(x, y, z)
        chunk = get_chunk(x, z)
        
    end
end # class MC_World


class MC_LevelDat
    attr_accessor :player
    
    def initialize(opts = {})
        if(opts[:world_dir] != nil)
            load_world(opts[:world_dir]})
            @level_dat_nbt = NBT.load("#{opts[:world_dir]}/level.dat");
            @player = MC_Player.new({nbt: @level_dat_nbt[:Player]})
        elsif(opts[:world_name] != nil)
            @level_dat_nbt = NBT.load("#{MCPATH}/saves/#{opts[:world_name]}/level.dat");
            @player = MC_Player.new({nbt: @level_dat_nbt[:Player]})
        elsif(opts[:nbt] != nil)
            # initialize from existing NBT
            @level_dat_nbt = opts[:nbt]
            @player = MC_Player.new({nbt: @level_dat_nbt[:Player]})
        else
            # Create new level.dat NBT structure
            @level_dat_nbt = NBT.new_compound("")
            @level_dat_nbt.insert(NBT.new_string("LevelName", "Unnamed World"))
            @level_dat_nbt.insert(NBT.new_long("SizeOnDisk", 0))
            @level_dat_nbt.insert(NBT.new_long("LastPlayed", 0))
            @level_dat_nbt.insert(NBT.new_long("Time", 0))
            @level_dat_nbt.insert(NBT.new_long("RandomSeed", 0))
            @level_dat_nbt.insert(NBT.new_byte("thundering", 0))
            @level_dat_nbt.insert(NBT.new_int("thunderTime", 0))
            @level_dat_nbt.insert(NBT.new_byte("raining", 0))
            @level_dat_nbt.insert(NBT.new_int("rainTime", 0))
            @level_dat_nbt.insert(NBT.new_int("version", 19132))# 1.6.6
            @level_dat_nbt.insert(NBT.new_int("SpawnX", 0))
            @level_dat_nbt.insert(NBT.new_int("SpawnY", 0))
            @level_dat_nbt.insert(NBT.new_int("SpawnZ", 0))
        
            @player = MC_Player.new
            @level_dat_nbt.insert(@player.get_nbt())
        end
    end
    
    def level_name()
        @level_dat_nbt[:LevelName].value
    end
    
    def level_name=(h)
        @level_dat_nbt[:LevelName] = h
    end
    
    def size_on_disk()
        @level_dat_nbt[:SizeOnDisk].value
    end
    
    def size_on_disk=(h)
        @level_dat_nbt[:SizeOnDisk] = h
    end
    
    def last_played()
        @level_dat_nbt[:LastPlayed].value
    end
    
    def last_played=(h)
        @level_dat_nbt[:LastPlayed] = h
    end
    
    def random_seed()
        @level_dat_nbt[:RandomSeed].value
    end
    
    def random_seed=(h)
        @level_dat_nbt[:RandomSeed] = h
    end
    
    def thundering=(h)
        @level_dat_nbt[:thundering] = h
    end
    
    def thundering()
        @level_dat_nbt[:thundering].value
    end
    
    def thunder_time=(h)
        @level_dat_nbt[:thunderTime] = h
    end
    
    def thunder_time()
        @level_dat_nbt[:thunderTime].value
    end
    
    def raining=(h)
        @level_dat_nbt[:raining] = h
    end
    
    def raining()
        @level_dat_nbt[:raining].value
    end
    
    def rain_time=(h)
        @level_dat_nbt[:rainTime] = h
    end
    
    def rain_time()
        @level_dat_nbt[:rainTime].value
    end
    
    def version=(h)
        @level_dat_nbt[:version] = h
    end
    
    def version()
        @level_dat_nbt[:version].value
    end
    
    def spawn=(pt)
        @level_dat_nbt[:SpawnX] = pt[0]
        @level_dat_nbt[:SpawnY] = pt[1]
        @level_dat_nbt[:SpawnZ] = pt[2]
    end
    
    def spawn()
        [@level_dat_nbt[:SpawnX].value, @level_dat_nbt[:SpawnY].value, @level_dat_nbt[:SpawnZ].value]
    end
    
    def get_nbt()
        @player_nbt
    end
end # class MC_LevelDat



class MC_Player
    def initialize(opts = {})
        if(opts[:nbt] != nil)
            @player_nbt = opts[:nbt]
        else
            @player_nbt = NBT.new_compound("Player")
            @player_nbt.insert(NBT.new_int("Score", 0))
            @player_nbt.insert(NBT.new_int("Dimension", opts.fetch(:dim, 0)))
            @player_nbt.insert(NBT.new_byte("OnGround", opts.fetch(:on_ground, 1)))
            @player_nbt.insert(NBT.new_list("Pos", [
                NBT.new_double("", 0.0),
                NBT.new_double("", 0.0),
                NBT.new_double("", 0.0)
            ], NBT::TAG_DOUBLE))
            @player_nbt.insert(NBT.new_list("Motion", [
                NBT.new_double("", 0.0),
                NBT.new_double("", 0.0),
                NBT.new_double("", 0.0)
            ], NBT::TAG_DOUBLE))
            @player_nbt.insert(NBT.new_list("Rotation", [
                NBT.new_float("", 0.0),
                NBT.new_float("", 0.0)
            ], NBT::TAG_FLOAT))
            @player_nbt.insert(NBT.new_int("SpawnX", 0))
            @player_nbt.insert(NBT.new_int("SpawnY", 0))
            @player_nbt.insert(NBT.new_int("SpawnZ", 0))
            @player_nbt.insert(NBT.new_short("AttackTime", 0))
            @player_nbt.insert(NBT.new_short("HurtTime", 0))
            @player_nbt.insert(NBT.new_short("DeathTime", 0))
            @player_nbt.insert(NBT.new_short("Fire", -20))
            @player_nbt.insert(NBT.new_short("Health", 19))
            @player_nbt.insert(NBT.new_short("Air", 300))
            @player_nbt.insert(NBT.new_float("FallDistance", 0.0))
            @player_nbt.insert(NBT.new_byte("Sleeping", 0))
            @player_nbt.insert(NBT.new_short("SleepTimer", 0))
            @player_nbt.insert(NBT.new_list("Inventory", [], NBT::TAG_COMPOUND))
        end
    end # initialize()
    
    def score=(val)
        @player_nbt[:Score] = val
    end
    
    def score()
        @player_nbt[:Score].value
    end
    
    def dimension=(val)
        @player_nbt[:Dimension] = val
    end
    
    def dimension()
        @player_nbt[:Dimension].value
    end
    
    def onground=(val)
        @player_nbt[:OnGround] = val
    end
    
    def onground()
        @player_nbt[:OnGround].value
    end
    
    def pos=(pt)
        @player_nbt[:Pos][0] = pt[0]
        @player_nbt[:Pos][1] = pt[1]
        @player_nbt[:Pos][2] = pt[2]
    end
    
    def pos()
        [@player_nbt[:Pos][0].value, @player_nbt[:Pos][1].value, @player_nbt[:Pos][2].value]
    end
    
    def motion=(vel)
        @player_nbt[:Motion][0] = vel[0]
        @player_nbt[:Motion][1] = vel[1]
        @player_nbt[:Motion][2] = vel[2]
    end
    
    def motion()
        [@player_nbt[:Motion][0].value, @player_nbt[:Motion][1].value, @player_nbt[:Motion][2].value]
    end
    
    def rotation=(pt)
        @player_nbt[:Rotation][0] = pt[0]
        @player_nbt[:Rotation][1] = pt[1]
    end
    
    def rotation()
        [@player_nbt[:Rotation][0].value, @player_nbt[:Rotation][1].value]
    end
    
    def spawn=(pt)
        @player_nbt[:SpawnX] = pt[0]
        @player_nbt[:SpawnY] = pt[1]
        @player_nbt[:SpawnZ] = pt[2]
    end
    
    def spawn()
        [@player_nbt[:SpawnX].value, @player_nbt[:SpawnY].value, @player_nbt[:SpawnZ].value]
    end
            # @player_nbt.insert(NBT.new_short("AttackTime", 0))
            # @player_nbt.insert(NBT.new_short("HurtTime", 0))
            # @player_nbt.insert(NBT.new_short("DeathTime", 0))
            # @player_nbt.insert(NBT.new_short("Fire", -20))
    
    def health=(h)
        @player_nbt[:Health] = h
    end
    
    def health()
        @player_nbt[:Health].value
    end
            # @player_nbt.insert(NBT.new_short("Air", 300))
            # @player_nbt.insert(NBT.new_float("FallDistance", 0.0))
    
    def sleeping=(s)
        @player_nbt[:Sleeping] = s
    end
    
    def sleeping()
        @player_nbt[:Sleeping].value
    end
    
    def sleep_timer=(s)
        @player_nbt[:SleepTimer] = s
    end
    
    def sleep_timer()
        @player_nbt[:SleepTimer].value
    end
    
    def get_nbt()
        @player_nbt
    end
    
    def inventory()
        @player_nbt[:Inventory]
    end
    
    def get_free_inv_slots(level_dat)
        # Empty slots have no entries and entries can have any order, so we start with all
        # slots and then remove the used ones.
        invslots = (0..35).to_a
        inventory.value.each {|item|
            id = item[:id].value
            slot = item[:Slot].value
            count = item[:Count].value
            damage = item[:Damage].value
            #puts "%d of %s in slot %d, damage %d" % [count, ITEM_NAMES[id], slot, damage]
            invslots.delete(slot);
        }
        invslots
    end
    
    def add_inv_item(level_dat, itemID, damage, count, slot)
        puts "Adding %d %s to slot %d\n" % [count, ITEM_NAMES[itemID], slot]
        item = NBT.new_compound("");
        item.insert(NBT.new_short("id", itemID))
        item.insert(NBT.new_short("Damage", damage))
        item.insert(NBT.new_byte("Count", count))
        item.insert(NBT.new_byte("Slot", slot))
        inventory.value.push(item)
    end
end # class MC_Player

end # module Magellan

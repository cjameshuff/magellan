# Various tools useful in manipulating Minecraft maps

require 'magellan'
require 'magellan/mcdefs'

module Magellan

# General workflow:
# If not working on individual chunks or the global level_dat, define
# a workspace: all chunks that will be affected by your work.
# All functions that operate on specific blocks or on entities in the
# world 


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
        
        @chunks = {}
    end
    
    # Generate a new chunk
    # TODO
    def fill_empty_chunk(x, z)
        puts "error, chunk doesn't exist"
        puts "chunk creation not yet implemented"
        exit()
    end
    
    def compute_lights()
        compute_lights_intern()
    end
    
    def compute_heights()
        compute_heights_intern()
    end
    
    # Iterate over the NBT of each non-empty chunk in the world, calling block
    def for_each_chunk_nbt(fn)
        @all_regions.each {|rgn|
            CHUNK_COORDS.each {|coord|
                if(rgn.chunk_exists(coord[0], coord[1]))
                    nbt = rgn.read_chunk_nbt(coord[0], coord[1])
                    yield(nbt)
                end
            }
        }
    end

    def for_each_entity_nbt(fn)
        for_each_chunk_nbt {|chunk|
            level = chunk[:Level]
            entities = level[:Entities].value
            entities.each {|ent| yield(ent)}
        }
    end

    def for_each_tile_entity_nbt(fn)
        for_each_chunk_nbt {|chunk|
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
    # Loads NBT for chunk containing coordinates if not already loaded, or creates new one with
    # fill_empty_chunk() if chunk doesn't yet exist.
    # TODO: unload a chunk if more than a maximum number are loaded. Unload clean chunk if any
    # exist, otherwise write and unload least recently accessed chunk.
    def load_chunk(x, z)
        region_coords = [x/512, z/512]
        
        region = @all_regions.fetch(region_coords) {
            puts "error, region doesn't exist"
            puts "region creation not yet implemented"
            exit()
        }
        
        chunk_nbt = nil
        chunk_coords = [(x/16) & 31, (z/16) & 31]
        if(region.chunk_exists(chunk_coords[0], chunk_coords[1]))
            chunk_nbt = region.read_chunk_nbt(chunk_coords[0], chunk_coords[1])
        else
            chunk_nbt = fill_empty_chunk(x, z)
        end
        
        @chunks[[x/16, z/16]] = {
            nbt: chunk_nbt,
            dirty: false,
            accessed: 0
        }
        
        chunk[]
        chunk
    end
    
    def write_chunk(x, z)
        slockPath = @world_dir + "/session.lock"
        # TODO: update session.lock
    end
    
    def get_chunk(x, z)
        chunk = @chunks.fetch([x/16, z/16]) {load_chunk(x, z)}
        @access_ctr += 1
        chunk[:accessed] = @access_ctr
        chunk
    end
    
    # Write loaded/modified chunks to disk
    def write_chunks()
        # @chunks.each {|chunk|
        #         }
    end
    
    # get list of chunk coordinates in workspace
    def workspace_chunks(min_coords, max_coords)
        region_coords_min = get_region_coords(min_coords)
        region_coords_max = get_region_coords(max_coords)
        chunk_coords_min = get_chunk_coords(min_coords)
        chunk_coords_max = get_chunk_coords(max_coords)
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
    def set_block(x, y, z, bid, data, blocklight, skylight)
        chunk = get_chunk(x, z)
    end
    
    # find first empty block location immediately above a non-empty block, starting from given location
    def drop_pt(x, y, z)
        chunk = get_chunk(x, z)
        
    end
end # class MC_World

# non-block items
# Coal, data/damage is 0 for coal, 1 for charcoal
# Dye: data/damage is dye color

# Tile entities
class MC_TileEntity
    attr_accessor :coords, :id
    
    def initialize(coords, id)
        @coords = coords
        @id = id
    end
    
    def new_nbt()
        entity = NBT.new_compound()
        entity.insert(NBT.new_string("id", @id))
        entity.insert(NBT.new_integer("x", @coords[0]))
        entity.insert(NBT.new_integer("y", @coords[1]))
        entity.insert(NBT.new_integer("z", @coords[2]))
    end
end

# Log
# Data is texture: 0: normal, 1: spruce, 2: birch

# Leaves
# Data is texture: 0: normal, 1: spruce, 2: birch

# Fire
# Data is...duration? 0 is placed fire, 15 is eternal? Wiki is unclear

# Jukebox
# Data is 0: nothing, 1: gold disc, 2: green disc

# Saplings
# Data is 0: plain sapling, 1: spruce, 2: birch
# Top two bits count upward to time growth to full tree

# Cactus
# Data is 0..15 counter to growth

# Sugar cane
# Data is 0..15 counter to growth

# Water, lava: data is 0 for full block, increases with distance.
# Water goes up to 7, lava to 6 in steps of 2. If bit 0x8 is set, fluid is falling and only spreads downward.

# Farmland: data is wetness, 0..8
# Crops: data is growth stage, 0..7

# Tall grass
# 0: dead shrub (identical in appearance to DeadShrub, but really grass)
# 1: tall grass
# 2: fern

# Wool: data/damage is wool color

# Slab
# 0: stone, 1: sandstone, 2: wood, 3: cobblestone

# Cake
# Pieces eaten: 1..5

# Bed
# 0: head is west, 1: north, 2: east, 3: south
# 0x8: set for head, clear for foot

# Lever: data bit 8 is on/off
# rest of data is direction:
# 1: south, 2: north, 3: west, 4: east
# 5: ground, west when off
# 6: ground, south when off (will not power wires around the block it is on)

# Button
# 1: w, 2: e, 3: s, 4: n
# bit 0x8 is pressed state

# Pressure plate
# 0x1: pressed

# Door:
# 0: nw hinge, 1: se hinge, 2: sw hinge, 3: nw hinge
# bit 0x8: top half
# bit 0x4: has swung CCW

# Trapdoor:
# 0: w hinge, 1: e hinge, 2: s hinge, 3: n hinge
# bit 0x4: open

# Redstone repeater
# 0: e, 1: s, 2: w, 3: n
# High two bits are delay, 1..4

# Redstone wire
# data is power strength, 0xF for wire adjacent to power source, 0 for unpowered.

class MC_Torch
    # block data value is direction
    DIRS = {:s: 1, :n: 2, :w: 3, :e: 4, :floor: 5}
end

class MC_Rail
    # block data value is direction
    DIRS = {
        :ew: 0, :ns: 1, # flat
        :as: 2, :an: 3, :ae: 4, :aw: 5, # ascending in a cardinal direction
        :ne: 6, :se: 7, :sw: 8, :nw: 9 # curved
    }
end

class MC_Ladder
    # block data value is direction
    DIRS = {:e: 2, :w: 3, :n: 4, :s: 5}
end

class MC_Stairs
    # block data value is ascending direction
    DIRS = {:s: 0, :n: 1, :w: 2, :e: 3}
end

class MC_Furnace
    # block data value is direction
    # Entity data: "Furnace"
    # TAG_Short("BurnTime")
    # TAG_Short("CookTime")
    # TAG_List("Items")
    DIRS = {:e: 2, :w: 3, :n: 4, :s: 5}
end
class MC_Dispenser
    # block data value is direction
    # Entity data: "Trap"
    # TAG_List("Items")
    DIRS = MC_Furnace::DIRS
end

# MobSpawner
# Entity data: "MonsterSpawner"
# TAG_String("EntityId")
# TAG_Short("Delay")

# Chest block
# Entity data: "Chest"
# TAG_List("Items")

# Note block
# Entity data: "Music"
# TAG_List("Items")

class MC_WallSign
    # block data value is direction
    DIRS = MC_Furnace::DIRS
end

class MC_Pumpkin
    # block data value is direction
    DIRS = {:e: 0, :s: 1, :w: 2, :n: 3}
end

class MC_SignPost
    # block data value is direction
    # tile entity contains Text1..Text4
    DIRS = {
        :w:  0, :wnw:  1, :nw:  2, :nnw: 3,
        :n:  4, :nne:  5, :ne:  6, :ene: 7,
        :e:  8, :ese:  9, :se: 10, :sse: 11,
        :s: 12, :ssw: 13, :sw: 14, :wsw: 15
    }
    def initialize(coords, dir, text1, text2 = "", text3 = "", text4 = "")
        super(coords, "Sign")
        @dir = dir
        @text1 = text1
        @text2 = text2
        @text3 = text3
        @text4 = text4
    end
    
    # Actually insert into world
    def place(world)
        chunk = world.get_chunk_nbt(@coords[0], @coords[2])
        entity = NBT.new_compound()
        entity.insert(NBT.new_string("id", id))
        entity.insert(NBT.new_integer("x", x))
        entity.insert(NBT.new_integer("y", y))
        entity.insert(NBT.new_integer("z", z))
        
        # FIXME: check for existing entity at this location
        chunk[:Level][:TileEntities].value.push(entity)
        
        world.set_block(@coords[0], @coords[1], @coords[2], BLOCK_IDS[:SignPost], @dir, 0, 0)
        signNBT = new_nbt()
        signNBT.insert(NBT.new_string("Text1", @text1))
        signNBT.insert(NBT.new_string("Text2", @text2))
        signNBT.insert(NBT.new_string("Text3", @text3))
        signNBT.insert(NBT.new_string("Text4", @text4))
    end
end

end # module Magellan


require 'magellan'
require 'magellan/mcdefs'
require 'magellan/mcentity'

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
    
    # Find first empty block location immediately above a non-empty block,
    # starting from given location.
    def drop_pt(x, y, z)
        chunk = get_chunk(x, z)
        
    end
end # class MC_World


end # module Magellan

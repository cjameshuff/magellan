
require 'magellan'
require 'magellan/mcdefs'
require 'magellan/mcentity'

module Magellan

class MC_LevelDat
    attr_accessor :player
    
    def initialize(opts = {})
        if(opts[:world_dir] != nil)
            @level_dat_nbt = NBT.load("#{opts[:world_dir]}/level.dat")
            @player = MC_Player.new({nbt: @level_dat_nbt[:Player]})
        elsif(opts[:world_name] != nil)
            @level_dat_nbt = NBT.load("#{MCPATH}/saves/#{opts[:world_name]}/level.dat")
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
            invslots.delete(slot)
        }
        invslots
    end
    
    def add_inv_item(level_dat, itemID, damage, count, slot)
        puts "Adding %d %s to slot %d\n" % [count, ITEM_NAMES[itemID], slot]
        item = NBT.new_compound("")
        item.insert(NBT.new_short("id", itemID))
        item.insert(NBT.new_short("Damage", damage))
        item.insert(NBT.new_byte("Count", count))
        item.insert(NBT.new_byte("Slot", slot))
        inventory.value.push(item)
    end
end # class MC_Player

end # module Magellan

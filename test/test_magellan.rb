require "test/unit"
require "magellan"
require 'magellan/mcdefs'

include Magellan

class TestNBT < Test::Unit::TestCase
  def test_sanity
    #flunk "write tests or I will kneecap you"
    win = true
    win
  end
end

# str = NBT.load("./test/testfiles/level.dat").to_s
# puts str
# pp NBT::load("./test/testfiles/level.dat")

testworld = "TestWorld"

world_dir = "#{MCPATH}/saves/#{testworld}/region"

regionFiles = Dir.entries(world_dir)
if(regionFiles[0] == '.')
    regionFiles.shift
end
if(regionFiles[0] == '..')
    regionFiles.shift
end
puts regionFiles

#regionFiles.each {|fin|
fin = "r.0.0.mcr"
    region = MCRegion.new
    region.open("#{world_dir}/#{fin}")
    puts "#{world_dir}/#{fin}:"
    region.printstats()
    chunks = []
    (0..31).each {|x|
        (0..31).each {|z|
            if(region.chunk_exists(x, z) == true)
                chunks.push([x, z])
            end
        }
    }
    
    puts "chunks.size: #{chunks.size}"
    # chunks.each {|chnk|
    #     puts "Trying to load chunk #{chnk[0]}, #{chnk[1]}"
    #     nbt = region.read_chunk_nbt(chnk[0], chnk[1])
    #     puts nbt.to_s
    #     puts '#####################################################################'
    # }
#    nbt.dump()
#}


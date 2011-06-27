#!/usr/bin/env ruby

require "#{File.dirname($0)}/../../lib/magellan/mcdefs.rb"

blockdefs = File.new("blockdefs.h", "w")

blockdefs.puts '#ifndef BLOCKDEFS_H'
blockdefs.puts '#define BLOCKDEFS_H'
blockdefs.puts ''
blockdefs.puts '#include <string>'
blockdefs.puts ''
blockdefs.puts 'struct BlockInfo {'
blockdefs.puts '    std::string name;'
blockdefs.puts '    uint8_t opacity;'
blockdefs.puts '};'
blockdefs.puts ''
blockdefs.puts 'BlockInfo blockdefs[256] = {'
blockdefs.puts (0..255).map {|bid|
    block = Magellan::BLOCKS_BY_ID[bid]
    if(block)
        "    {\"#{block[:name]}\", #{block[:opacity]}}"
    else
        "    {\"\", 15}"
    end
}.join(",\n")
blockdefs.puts '};'
blockdefs.puts ''
blockdefs.puts '#endif // BLOCKDEFS_H'

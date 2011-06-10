require 'magellan/magellan'

class NBT
    attr_accessor :name, :type, :value, :entry_type
    TAG_END = 0
    TAG_BYTE = 1
    TAG_SHORT = 2
    TAG_INT = 3
    TAG_LONG = 4
    TAG_FLOAT = 5
    TAG_DOUBLE = 6
    TAG_BYTE_ARRAY = 7
    TAG_STRING = 8
    TAG_LIST = 9
    TAG_COMPOUND = 10
    
    TAG_NAMES = %w[TAG_END TAG_BYTE TAG_SHORT TAG_INT TAG_LONG TAG_FLOAT
        TAG_DOUBLE TAG_BYTE_ARRAY TAG_STRING TAG_LIST TAG_COMPOUND]
    
    # NBTs are simple objects with @name, @type, and @value. Lists also have @entry_type.
    # There is currently no actual class heirarchy for the various types.
    def self.new_byte(name, val = 0)
        NBT.new(name, NBT::TAG_BYTE, val)
    end
    def self.new_short(name, val = 0)
        NBT.new(name, NBT::TAG_SHORT, val)
    end
    def self.new_int(name, val = 0)
        NBT.new(name, NBT::TAG_INT, val)
    end
    def self.new_long(name, val = 0)
        NBT.new(name, NBT::TAG_LONG, val)
    end
    def self.new_float(name, val = 0)
        NBT.new(name, NBT::TAG_FLOAT, val)
    end
    def self.new_double(name, val = 0)
        NBT.new(name, NBT::TAG_DOUBLE, val)
    end
    def self.new_byte_array(name, val = '')
        NBT.new(name, NBT::TAG_BYTE_ARRAY, val)
    end
    def self.new_string(name, val = '')
        NBT.new(name, NBT::TAG_STRING, val)
    end
    def self.new_list(name, vals, entry_type)
        NBT.new(name, NBT::TAG_LIST, vals, entry_type)
    end
    def self.new_compound(name, val = {})
        NBT.new(name, NBT::TAG_COMPOUND, val)
    end
    
    @@printindent = 0
    def to_s()
        if(@type == NBT::TAG_BYTE_ARRAY)
            "  "*@@printindent + "#{@name}: <BYTE_ARRAY>"
        elsif(@type == NBT::TAG_COMPOUND)
            @@printindent += 1
            contstr = @value.map {|v| v[1].to_s}.join(",\n")
            @@printindent -= 1
            "  "*@@printindent + "#{@name}: {\n#{contstr}\n" + "  "*@@printindent +  "}"
        elsif(@type == NBT::TAG_LIST)
            @@printindent += 1
            contstr = @value.map {|value| value.to_s}.join(",\n")
            @@printindent -= 1
            "  "*@@printindent + "#{@name}: (#{TAG_NAMES[@entry_type]})[\n#{contstr}\n" + "  "*@@printindent +  "]"
        else
            "  "*@@printindent + "#{@name}: (#{TAG_NAMES[@type]})#{@value.to_s}"
        end
    end
    
    def [](idx)
        if(@type == NBT::TAG_COMPOUND || @type == NBT::TAG_LIST)
            @value[idx]
        else
            puts "attempt to use operator []= on non-composite tag"
            nil
        end
    end
    
    def []=(idx, val)
        if(@type == NBT::TAG_COMPOUND || @type == NBT::TAG_LIST)
            @value[idx] = val
        else
            puts "attempt to use operator []= on non-composite tag"
            nil
        end
    end
    
    def insert(val)
        if(@type == NBT::TAG_COMPOUND)
            @value[val.name] = val
        end
    end
end # class NBT

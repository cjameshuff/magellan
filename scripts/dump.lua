
--print("\ndump.lua");

--print(string.format("#ARGV: %d", #ARGV));

--for k, i in pairs(ARGV) do
--    print(i);
--end

nbt = NBT.load(ARGV[3]);
print(nbt);

// Copies file to output file, and dumps text representation to standard output
// Not useful for much, just a test of NBT loading/writing.
nbt = NBT.load(ARGV[3]);
print(nbt);
nbt.write(ARGV[4]);

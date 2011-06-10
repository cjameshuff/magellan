require 'mkmf'


if(Config::CONFIG['host_os'] =~ /darwin/)
	$CPPFLAGS = "-DMACOSX"
else
	$CPPFLAGS = "-DLINUX"
end

$srcs = []
$srcs.push('blocktypes.cpp')
$srcs.push('mc.cpp')
$srcs.push('nbt.cpp')
$srcs.push('nbtio.cpp')
$srcs.push('nbtrb.cpp')
$srcs.push('magellan.cpp')

#$srcs = $srcs.map {|f| "ext/magellan/" + f}

$objs = $srcs.map {|f| File.basename(f, ".*") + ".o"}
# $objs = $srcs.map {|f| f + ".o"}

have_library("z", "gzopen")
have_library("png", "png_init_io")

create_makefile('magellan/magellan')


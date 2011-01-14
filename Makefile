#******************************************************************************
#    Copyright (c) 2010, Christopher James Huff
#    All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

CXX = g++

#OPT = -O0
OPT = -O3

CFLAGS = -Wall -g ${OPT} -Izlib/include -Ilibpng/include -Ilua/etc -Ilua/src -Iv8/include

INSTALLDIR = $(HOME)/bin
MCIMAGEFILES = terrain.png misc/grasscolor.png misc/foliagecolor.png

ifeq ($(shell uname -s), Darwin)
	CFLAGS += -DMACOSX
	LUABUILD = macosx
	MCDIR = Library/Application\ Support/minecraft
	LIBS = zlib/libz.a libpng/lib/libpng.a lua/src/liblua.a v8/libv8.a
else
	CFLAGS += -DLINUX
	LUABUILD = linux
	MCDIR = .minecraft
	LIBS = -lz -lpng lua/src/liblua.a v8/libv8.a -ldl
endif

# This may be a bad idea...
MGLNDIR = $(MCDIR)/magellan

all: magellan

libs:
	tar -xf zlib-1.2.5.tar.gz; mv zlib-1.2.5 zlib
	cd zlib; ./configure --prefix=`pwd`/../zlib --static; make
	tar -xf libpng-1.4.5.tar.gz
	cd libpng-1.4.5; ./configure --prefix=`pwd`/../libpng --disable-shared; make; make install

#lua:
#	tar -xf lua-5.1.4.tar.gz; mv lua-5.1.4 lua
#	cd lua; make $(LUABUILD)

#magellan: src/magellan.cpp.o src/maglua.cpp.o src/v8base.cpp.o src/nbtv8.cpp.o src/magv8.cpp.o src/nbtlua.cpp.o src/mc.cpp.o src/nbt.cpp.o lua/src/liblua.a
#	$(CXX) src/magellan.cpp.o src/maglua.cpp.o src/v8base.cpp.o src/nbtv8.cpp.o src/magv8.cpp.o src/nbtlua.cpp.o src/mc.cpp.o src/nbt.cpp.o $(LIBS) -o magellan
magellan: src/magellan.cpp.o src/v8base.cpp.o src/nbtv8.cpp.o src/magv8.cpp.o src/mc.cpp.o src/nbt.cpp.o
	$(CXX) src/magellan.cpp.o src/v8base.cpp.o src/nbtv8.cpp.o src/magv8.cpp.o src/mc.cpp.o src/nbt.cpp.o $(LIBS) -o magellan

src/%.cpp.o: src/%.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

install: magellan
	cp magellan $(INSTALLDIR)/magellan
	mkdir -p $(HOME)/$(MGLNDIR)
	mkdir -p $(HOME)/$(MGLNDIR)/scripts
	cp -r scripts/* $(HOME)/$(MGLNDIR)/scripts/
	cd $(HOME)/$(MGLNDIR); jar -xf $(HOME)/$(MCDIR)/bin/minecraft.jar ${MCIMAGEFILES}

clean_all: clean
	rm -rf lua
	rm -rf zlib
	rm -rf libpng-1.4.4
	rm -rf libpng
	rm magellan

clean:
	rm -rf src/*.o


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

CFLAGS = -Wall -g -O0 -Ilibpng/include -Ilua-5.1.4/etc -Ilua-5.1.4/src
#CFLAGS = -Wall -g -O3 -Ilibpng/include -Ilua-5.1.4/etc -Ilua-5.1.4/src
LIBS = zlib-1.2.5/libz.a libpng/lib/libpng.a lua-5.1.4/src/liblua.a

INSTALLDIR = $(HOME)/bin
MCIMAGEFILES = terrain.png misc/grasscolor.png misc/foliagecolor.png

ifeq ($(shell uname -s), Darwin)
	CFLAGS += -DMACOSX
	LUABUILD = macosx
	MCDIR = Library/Application\ Support/minecraft
else
	CFLAGS += -DLINUX
	LUABUILD = linux
	MCDIR = .minecraft
endif

# This may be a bad idea...
MGLNDIR = $(MCDIR)/magellan

all: magellan nbtutil

libs:
	tar -xf zlib-1.2.5.tar.gz
	cd zlib-1.2.5; ./configure --prefix=`pwd`/../zlib --static; make
	tar -xf libpng-1.4.5.tar.gz
	cd libpng-1.4.5; ./configure --prefix=`pwd`/../libpng --disable-shared; make; make install
	tar -xf lua-5.1.4.tar.gz
	cd lua-5.1.4; make $(LUABUILD)

nbtutil: src/nbtutil.cpp.o src/nbt.cpp.o src/nbtlua.cpp.o
	$(CXX) src/nbtutil.cpp.o src/nbt.cpp.o src/nbtlua.cpp.o $(LIBS) -o nbtutil

magellan: src/magellan.cpp.o src/maglua.cpp.o src/nbtlua.cpp.o src/mc.cpp.o src/nbt.cpp.o
	$(CXX) src/magellan.cpp.o src/maglua.cpp.o src/nbtlua.cpp.o src/mc.cpp.o src/nbt.cpp.o $(LIBS) -o magellan

src/%.cpp.o: src/%.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

install: nbtutil magellan
	cp nbtutil $(INSTALLDIR)/nbtutil
	cp magellan $(INSTALLDIR)/magellan
	mkdir -p $(HOME)/$(MGLNDIR)
	mkdir -p $(HOME)/$(MGLNDIR)/scripts
	cp -r scripts/* $(HOME)/$(MGLNDIR)/scripts/
	cd $(HOME)/$(MGLNDIR); jar -xf $(HOME)/$(MCDIR)/bin/minecraft.jar ${MCIMAGEFILES}

clean_all: clean
	rm -rf zlib-1.2.5
	rm -rf libpng-1.4.4
	rm -rf libpng
	rm nbtutil
	rm magellan

clean:
	rm -rf src/*.o


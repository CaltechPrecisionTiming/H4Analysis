CXX = g++
CXXFLAGS = -std=c++11 -fPIC
SOFLAGS = -shared -O3
INCLUDE = -I"./"
LIB = -L"./lib/"
libdir = /usr/lib/root/
incdir = /usr/include/root/

ROOT_LIB := `root-config --libs --glibs`
ROOT_FLAGS := `root-config --cflags --ldflags`

all: bin/DT_SimpleExample lib/LinkDef.cxx lib/DynamicTTreeDict.so lib/libDTT.so

lib/%.o: src/%.cc interface/%.h interface/DynamicTTreeInterface.h
	@echo " CXX $<"
	@$ $(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS)

lib/%.so: lib/DynamicTTreeBase.o
	@echo " CXX $<"
	@$ $(CXX) -shared -o $@ $<		

lib/LinkDef.cxx: test/DT_SimpleExample.cpp
	@$ rootcling -f $@ -c $^
	@$ rootcling -f $@ -c $^

lib/DynamicTTreeDict.so: lib/LinkDef.cxx lib/DynamicTTreeBase.o
	@echo " CXX $<"
	@$ $(CXX) $(CXXFLAGS) $(SOFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) $(LIB)

bin/%: test/%.cpp lib/DynamicTTreeBase.o
	@echo " CXX $<"
	@$ $(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(ROOT_LIB) $(ROOT_FLAGS) 

clean:
	rm -fr bin/*
	rm -fr lib/*

install:
	cp lib/DynamicTTreeDict.so $(libdir)
	cp interface/DynamicTTreeInterface.h $(incdir)
	cp interface/DynamicTTreeBase.h $(incdir)

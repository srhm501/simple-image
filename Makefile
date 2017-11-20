CXXFLAGS=-std=c++11 -Wall -Wextra -Wpedantic -Wno-ignored-attributes -O3 -march=native -I./hdr
LDLIBS=-lOpenCL -lnetpbm

SRCS=$(wildcard src/*.cpp) src/resources.cpp
OBJS=$(addprefix obj/,$(notdir $(addsuffix .o,$(SRCS))))
CLSRCS=$(wildcard cl/*.cl)

all: resources image

resources: make_resource src/resources.cpp $(CLSRCS)

make_resource: util/make_resource.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

src/resources.cpp: make_resource $(CLSRCS)
	rm -f src/resources.cpp
	rm -f src/resources.hpp
	./generate-resources.sh $(CLSRCS)



image: src/resources.cpp $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

obj/%.cpp.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f obj/*
	rm -f make_resource
	rm -f src/resources.hpp
	rm -f src/resources.cpp


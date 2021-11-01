
all: build/planter

run: build/planter FORCE
	docker run \
	  -it \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  build/planter $(ARGS)

FORCE:

CPPFILES = $(shell ls *.cpp)
DEFINES = -DTBB_SUPPRESS_DEPRECATED_MESSAGES=1
INCLUDE = -I/usr/include/lua5.4
CPPFLAGS = $(DEFINES) $(INCLUDE)
LINKFLAGS = -lopenvdb -lHalf -ltbb -llua5.4
OBJECTS = $(CPPFILES:%.cpp=build/%.o)
EXAMPLES_LUA = $(shell ls examples/*.lua)
EXAMPLES = $(EXAMPLES_LUA:examples/%.lua=build/%.test)

build/planter: $(OBJECTS) build/.builder
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  g++ $(OBJECTS) -o build/planter $(LINKFLAGS)

build/%.o: %.cpp build/.builder
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  g++ -c $(CPPFLAGS) $< -o $@

build/.builder: Dockerfile-builder
	mkdir -p build
	docker build -t planter-builder:latest -f Dockerfile-builder .
	touch build/.builder

test: $(EXAMPLES)

build/%.test: examples/%.lua build/planter
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  build/planter $<
	touch $@


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
OBJECTS = $(CPPFILES:%.cpp=build/%.o)

build/planter: $(OBJECTS)
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  g++ $(OBJECTS) -o build/planter -lopenvdb -lHalf -ltbb

build/%.o: %.cpp build/.builder
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  g++ -c $(DEFINES) $< -o $@

build/.builder: Dockerfile-builder
	mkdir -p build
	docker build -t planter-builder:latest -f Dockerfile-builder .
	touch build/.builder

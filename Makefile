
all: build/planter

run: build/planter FORCE
	docker run \
	  -it \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  build/planter $(ARGS)

FORCE:

build/planter: build/.builder *.cpp
	docker run \
	  -v `pwd`:/w \
	  -w /w \
	  planter-builder:latest \
	  g++ *.cpp -o build/planter -lopenvdb -lHalf -ltbb

build/.builder: Dockerfile-builder
	mkdir -p build
	docker build -t planter-builder:latest -f Dockerfile-builder .
	touch build/.builder

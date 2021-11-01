# Planter

Planter is a tool to generate shapes by rendering spheres (particles)
into a signed distance function using OpenVDB.  The original goal of
this project was to generate a 3D printable planter, but it became a
little more general than that.

## Lua Bindings

This repo builds a C++ executable that embeds a Lua interpreter.  The
generation of the shape is defined through a Lua script.  See the
`examples` directory for sample scripts and the file `planterlib.lua`
for the C++ extension functions and Lua infrastructure for defining
shapes.

## Building

To build this repo you need to have GNU make and Docker installed.
Then run:

```bash
  make
```

## Running Examples

To run one of the examples, e.g. helix.lua, run:

```bash
  docker run -it -v `pwd`:/w -w /w planter-builder:latest build/planter examples/helix.lua
```

An OBJ called `output.obj` will be generated.

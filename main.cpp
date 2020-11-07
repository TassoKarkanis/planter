#include <iostream>
#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/ParticlesToLevelSet.h>
#include <openvdb/tools/SignedFloodFill.h>
#include <openvdb/tools/VolumeToMesh.h>

#include "ParticleList.h"
#include "write_obj.h"

void
populate_grid(openvdb::FloatGrid &grid)
{
    ParticleList particles;

    particles.add(openvdb::Vec3R(0, 0, 0), 10);
    particles.add(openvdb::Vec3R(5, 0, 0), 10);

    openvdb::tools::particlesToSdf(particles, grid);
}

int
main()
{
    openvdb::initialize();

    // Create a shared pointer to a newly-allocated grid of a built-in type:
    // in this case, a FloatGrid, which stores one single-precision floating point
    // value per voxel.  Other built-in grid types include BoolGrid, DoubleGrid,
    // Int32Grid and Vec3SGrid (see openvdb.h for the complete list).
    // The grid comprises a sparse tree representation of voxel data,
    // user-supplied metadata and a voxel space to world space transform,
    // which defaults to the identity transform.
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(/*background value=*/2.0);

    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.5 units in world space.
    grid->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.5));

    // Identify the grid as a level set.
    grid->setGridClass(openvdb::GRID_LEVEL_SET);

    populate_grid(*grid);

    // Name the grid "LevelSetSphere".
    grid->setName("LevelSetSphere");

    // get a mesh
    write_obj("build/output.obj", *grid);

    return 0;
}

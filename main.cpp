#include <fstream>
#include <iostream>
#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/ParticlesToLevelSet.h>
#include <openvdb/tools/SignedFloodFill.h>
#include <openvdb/tools/VolumeToMesh.h>

class ParticleList
{
  protected:
    struct Particle
    {
        openvdb::Vec3R p, v;
        openvdb::Real  r;
    };
    openvdb::Real         mRadiusScale;
    openvdb::Real         mVelocityScale;
    std::vector<Particle> mParticleList;

  public:
    typedef openvdb::Vec3R PosType;

    ParticleList(openvdb::Real rScale = 1, openvdb::Real vScale = 1)
        : mRadiusScale(rScale), mVelocityScale(vScale)
    {
    }

    void
    add(const openvdb::Vec3R &p, const openvdb::Real &r, const openvdb::Vec3R &v = openvdb::Vec3R(0, 0, 0))
    {
        Particle pa;
        pa.p = p;
        pa.r = r;
        pa.v = v;
        mParticleList.push_back(pa);
    }

    /// @return coordinate bbox in the space of the specified transfrom
    openvdb::CoordBBox
    getBBox(const openvdb::GridBase &grid)
    {
        openvdb::CoordBBox bbox;
        openvdb::Coord &   min = bbox.min(), &max = bbox.max();
        openvdb::Vec3R     pos;
        openvdb::Real      rad, invDx = 1 / grid.voxelSize()[0];
        for (size_t n = 0, e = this->size(); n < e; ++n)
        {
            this->getPosRad(n, pos, rad);
            const openvdb::Vec3d xyz = grid.worldToIndex(pos);
            const openvdb::Real  r   = rad * invDx;
            for (int i = 0; i < 3; ++i)
            {
                min[i] = openvdb::math::Min(min[i], openvdb::math::Floor(xyz[i] - r));
                max[i] = openvdb::math::Max(max[i], openvdb::math::Ceil(xyz[i] + r));
            }
        }
        return bbox;
    }

    //////////////////////////////////////////////////////////////////////////////
    /// The methods below are the only ones required by tools::ParticleToLevelSet
    /// @note We return by value since the radius and velocities are modified
    /// by the scaling factors! Also these methods are all assumed to
    /// be thread-safe.

    /// Return the total number of particles in list.
    ///  Always required!
    size_t
    size() const
    {
        return mParticleList.size();
    }

    /// Get the world space position of n'th particle.
    /// Required by ParticledToLevelSet::rasterizeSphere(*this,radius).
    void
    getPos(size_t n, openvdb::Vec3R &pos) const
    {
        pos = mParticleList[n].p;
    }

    void
    getPosRad(size_t n, openvdb::Vec3R &pos, openvdb::Real &rad) const
    {
        pos = mParticleList[n].p;
        rad = mRadiusScale * mParticleList[n].r;
    }
    void
    getPosRadVel(size_t n, openvdb::Vec3R &pos, openvdb::Real &rad, openvdb::Vec3R &vel) const
    {
        pos = mParticleList[n].p;
        rad = mRadiusScale * mParticleList[n].r;
        vel = mVelocityScale * mParticleList[n].v;
    }
    // The method below is only required for attribute transfer
    void
    getAtt(size_t n, openvdb::Index32 &att) const
    {
        att = openvdb::Index32(n);
    }
};

void
populate_grid(openvdb::FloatGrid &grid)
{
    ParticleList particles;

    particles.add(openvdb::Vec3R(0, 0, 0), 10);
    particles.add(openvdb::Vec3R(5, 0, 0), 10);

    openvdb::tools::particlesToSdf(particles, grid);
}

// Populate the given grid with a narrow-band level set representation of a sphere.
// The width of the narrow band is determined by the grid's background value.
// (Example code only; use tools::createSphereSDF() in production.)
template <class GridType>
void
makeSphere(GridType &grid, float radius, const openvdb::Vec3f &c)
{
    using ValueT = typename GridType::ValueType;
    // Distance value for the constant region exterior to the narrow band
    const ValueT outside = grid.background();
    // Distance value for the constant region interior to the narrow band
    // (by convention, the signed distance is negative in the interior of
    // a level set)
    const ValueT inside = -outside;
    // Use the background value as the width in voxels of the narrow band.
    // (The narrow band is centered on the surface of the sphere, which
    // has distance 0.)
    int padding = int(openvdb::math::RoundUp(openvdb::math::Abs(outside)));
    // The bounding box of the narrow band is 2*dim voxels on a side.
    int dim = int(radius + padding);
    // Get a voxel accessor.
    typename GridType::Accessor accessor = grid.getAccessor();
    // Compute the signed distance from the surface of the sphere of each
    // voxel within the bounding box and insert the value into the grid
    // if it is smaller in magnitude than the background value.
    openvdb::Coord ijk;
    int &          i = ijk[0], &j = ijk[1], &k = ijk[2];
    for (i = c[0] - dim; i < c[0] + dim; ++i)
    {
        const float x2 = openvdb::math::Pow2(i - c[0]);
        for (j = c[1] - dim; j < c[1] + dim; ++j)
        {
            const float x2y2 = openvdb::math::Pow2(j - c[1]) + x2;
            for (k = c[2] - dim; k < c[2] + dim; ++k)
            {
                // The distance from the sphere surface in voxels
                const float dist = openvdb::math::Sqrt(x2y2 + openvdb::math::Pow2(k - c[2])) - radius;
                // Convert the floating-point distance to the grid's value type.
                ValueT val = ValueT(dist);
                // Only insert distances that are smaller in magnitude than
                // the background value.
                if (val < inside || outside < val)
                    continue;
                // Set the distance for voxel (i,j,k).
                accessor.setValue(ijk, val);
            }
        }
    }
    // Propagate the outside/inside sign information from the narrow band
    // throughout the grid.
    openvdb::tools::signedFloodFill(grid.tree());
}

void
write_obj(const std::string &filename, const openvdb::tools::VolumeToMesh &v2m)
{
    std::ofstream out;
    out.open(filename);

    // write the vertices
    std::cout << "num points: " << v2m.pointListSize() << std::endl;
    size_t                num_points = v2m.pointListSize();
    const openvdb::Vec3s *points     = v2m.pointList().get();
    for (int i = 0; i < int(num_points); ++i)
    {
        const openvdb::Vec3s &p = points[i];
        out << "v " << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }

    // write the faces
    const openvdb::tools::PolygonPool *poly_pools = v2m.polygonPoolList().get();
    size_t                             num_pools  = v2m.polygonPoolListSize();
    std::cout << "num poly pools: " << num_pools << std::endl;
    for (int i = 0; i < int(num_pools); ++i)
    {
        const openvdb::tools::PolygonPool &pool = poly_pools[i];

        // write the quads
        size_t num_quads = pool.numQuads();
        for (size_t j = 0; j < num_quads; ++j)
        {
            const openvdb::Vec4I &q = pool.quad(j);
            out << "f " << q[0] + 1 << ' ' << q[1] + 1 << ' ' << q[2] + 1 << ' ' << q[3] + 1 << std::endl;
        }

        // write the triangles
        size_t num_tris = pool.numTriangles();
        for (size_t j = 0; j < num_tris; ++j)
        {
            const openvdb::Vec3I &q = pool.triangle(j);
            out << "f " << q[0] + 1 << ' ' << q[1] + 1 << ' ' << q[2] + 1 << std::endl;
        }
    }
}

int
main()
{
    std::cout << "hello, world!\n";

    openvdb::initialize();

    // Create a shared pointer to a newly-allocated grid of a built-in type:
    // in this case, a FloatGrid, which stores one single-precision floating point
    // value per voxel.  Other built-in grid types include BoolGrid, DoubleGrid,
    // Int32Grid and Vec3SGrid (see openvdb.h for the complete list).
    // The grid comprises a sparse tree representation of voxel data,
    // user-supplied metadata and a voxel space to world space transform,
    // which defaults to the identity transform.
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(/*background value=*/2.0);

#if 0    
    // Populate the grid with a sparse, narrow-band level set representation
    // of a sphere with radius 50 voxels, located at (1.5, 2, 3) in index space.
    makeSphere(*grid, /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3));
#endif
    populate_grid(*grid);

    // Associate some metadata with the grid.
    grid->insertMeta("radius", openvdb::FloatMetadata(50.0));

    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.5 units in world space.
    grid->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.5));
    // Identify the grid as a level set.
    grid->setGridClass(openvdb::GRID_LEVEL_SET);
    // Name the grid "LevelSetSphere".
    grid->setName("LevelSetSphere");
    // Create a VDB file object.
    openvdb::io::File file("build/mygrids.vdb");
    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(grid);
    // Write out the contents of the container.
    file.write(grids);
    file.close();

    // get a mesh
    {
        openvdb::tools::VolumeToMesh alg;
        alg(*grid);
        write_obj("build/output.obj", alg);
    }

    return 0;
}

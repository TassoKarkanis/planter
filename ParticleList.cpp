#include "ParticleList.h"

// Used to render particles to an SDF

ParticleList::ParticleList(openvdb::Real rScale, openvdb::Real vScale)
    : mRadiusScale(rScale), mVelocityScale(vScale)
{
}

void
ParticleList::add(const openvdb::Vec3R &p, const openvdb::Real &r, const openvdb::Vec3R &v)
{
    Particle pa;
    pa.p = p;
    pa.r = r;
    pa.v = v;
    mParticleList.push_back(pa);
}

openvdb::CoordBBox
ParticleList::getBBox(const openvdb::GridBase &grid)
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

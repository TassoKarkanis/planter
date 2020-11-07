#ifndef PARTICLE_LIST_H
#define PARTICLE_LIST_H

#include <openvdb/openvdb.h>

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

    ParticleList(openvdb::Real rScale = 1, openvdb::Real vScale = 1);

    void add(const openvdb::Vec3R &p, const openvdb::Real &r,
             const openvdb::Vec3R &v = openvdb::Vec3R(0, 0, 0));

    /// @return coordinate bbox in the space of the specified transform
    openvdb::CoordBBox getBBox(const openvdb::GridBase &grid);

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

#endif

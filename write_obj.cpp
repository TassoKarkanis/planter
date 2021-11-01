#include <fstream>
#include <openvdb/tools/VolumeToMesh.h>

#include "write_obj.h"

void
write_obj(const std::string &filename, const openvdb::FloatGrid &grid)
{
    // mesh the grid
    openvdb::tools::VolumeToMesh v2m;
    v2m(grid);

    // check the result
    if (v2m.pointListSize() == 0)
    {
        std::cout << "failed to output OBJ: no vertices in mesh!\n";
        return;
    }

    if (v2m.polygonPoolListSize() == 0)
    {
        std::cout << "failed to output OBJ: no polygons in mesh!\n";
        return;
    }

    // open the file
    std::ofstream out;
    out.open(filename);

    // write out the vertices
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

    out.close();
    if (out.fail())
        std::cout << "failed to write OBJ!\n";
}

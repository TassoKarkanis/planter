#ifndef GRID_USER_DATA_H
#define GRID_USER_DATA_H

#include <lua.h>
#include <openvdb/openvdb.h>

struct GridUserdata
{
    openvdb::FloatGrid::Ptr grid;

    static int create(lua_State *L);
    static int destroy(lua_State *L);

    static std::string s_name;
};

#endif

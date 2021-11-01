#include <iostream>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <string.h>
#include <tuple>

#include <openvdb/tools/ParticlesToLevelSet.h>

#include "GridUserdata.h"
#include "ParticleListUserdata.h"
#include "lua_bindings.h"
#include "write_obj.h"

namespace
{

template <typename UD>
UD *
check(lua_State *L, int index)
{
    void *ud = luaL_checkudata(L, index, UD::s_name.c_str());
    return static_cast<UD *>(ud);
}

openvdb::Vec3d
get_vector(lua_State *L, int index)
{
    if (!lua_istable(L, index))
        luaL_error(L, "expecting vector");

    lua_pushnumber(L, 1);
    lua_gettable(L, index);
    double x = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pushnumber(L, 2);
    lua_gettable(L, index);
    double y = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pushnumber(L, 3);
    lua_gettable(L, index);
    double z = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    return openvdb::Vec3d(x, y, z);
}

int
add_particle(lua_State *L)
{
    // get the particle list
    auto *ud = check<ParticleListUserdata>(L, 1);

    // get the position
    openvdb::Vec3d p = get_vector(L, 2);

    // get the radius
    double r = luaL_checknumber(L, 3);

    // add the particle
    ud->particle_list->add(p, r);

    return 0;
}

int
particles_to_sdf(lua_State *L)
{
    // get the particle list
    auto *ud1 = check<ParticleListUserdata>(L, 1);

    // get the grid
    auto *ud2 = check<GridUserdata>(L, 2);

    openvdb::tools::particlesToSdf(*ud1->particle_list, *ud2->grid);

    return 0;
}

int
write_obj_(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    auto       *ud       = check<GridUserdata>(L, 2);

    write_obj(filename, *ud->grid);

    return 0;
}

template <typename UD>
void
add_userdata(lua_State *L)
{
    // add the metatable
    luaL_newmetatable(L, UD::s_name.c_str());

    // set the destructor
    lua_pushcfunction(L, UD::destroy);
    lua_setfield(L, -2, "__gc");

    // pop the metatable
    lua_pop(L, 1);

    // add the constructor
    std::string constructor_name = "create_" + UD::s_name;
    lua_pushcfunction(L, UD::create);
    lua_setglobal(L, constructor_name.c_str());
}

void
add_extensions(lua_State *L)
{
    add_userdata<GridUserdata>(L);
    add_userdata<ParticleListUserdata>(L);

    std::vector<std::tuple<std::string, lua_CFunction>> functions = {
        {"add_particle", add_particle},
        {"particles_to_sdf", particles_to_sdf},
        {"write_obj", write_obj_},
    };

    for (const auto &item : functions)
    {
        lua_pushcfunction(L, std::get<1>(item));
        lua_setglobal(L, std::get<0>(item).c_str());
    }
}

int
require_planter(lua_State *L)
{
    int ret = luaL_dofile(L, "planterlib.lua");
    if (ret != LUA_OK)
        return ret;

    return LUA_OK;
}

int
run_lua(lua_State *L, int argc, char **argv)
{
    luaL_openlibs(L);

    // add the C extensions
    add_extensions(L);

    // require the planter library
    int ret = require_planter(L);
    if (ret != LUA_OK)
        return ret;

    // run the script
    ret = luaL_dofile(L, argv[0]);
    if (ret != LUA_OK)
        return ret;

    return LUA_OK;
}

} // namespace

int
cmd_lua(int argc, char **argv)
{
    // create the state
    lua_State *L = luaL_newstate();

    // run the script and print any error that occurs
    int ret = run_lua(L, argc, argv);
    if (ret != LUA_OK)
    {
        int count = lua_gettop(L);
        if (count >= 1)
        {
            if (lua_isstring(L, 1))
            {
                const char *str = lua_tostring(L, 1);
                std::cout << str << std::endl;
            }

            // pop the value from the stack
            lua_pop(L, 1);
        }
    }

    lua_close(L);

    return ret == LUA_OK ? 0 : 1;
}

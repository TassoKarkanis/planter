#include "ParticleListUserdata.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

std::string ParticleListUserdata::s_name = "particle_list";

int
ParticleListUserdata::create(lua_State *L)
{
    // create the userdata
    void *ud_         = lua_newuserdata(L, sizeof(ParticleListUserdata));
    auto  ud          = new (ud_) ParticleListUserdata();
    ud->particle_list = std::make_shared<ParticleList>();

    // set the metatable
    luaL_getmetatable(L, s_name.c_str());
    lua_setmetatable(L, -2);

    return 1;
}

int
ParticleListUserdata::destroy(lua_State *L)
{
    void *ud_ = luaL_checkudata(L, 1, s_name.c_str());
    auto *ud  = static_cast<ParticleListUserdata *>(ud_);
    ud->~ParticleListUserdata();
    return 0;
}

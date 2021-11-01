#include "GridUserdata.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

std::string GridUserdata::s_name = "grid";

int
GridUserdata::create(lua_State *L)
{
    // get the background value
    double background = luaL_checknumber(L, 1);

    // create the userdata
    void *ud_ = lua_newuserdata(L, sizeof(GridUserdata));
    auto  ud  = new (ud_) GridUserdata();
    ud->grid  = openvdb::FloatGrid::create(background);
    ud->grid->setGridClass(openvdb::GRID_LEVEL_SET);
    ud->grid->setName("LevelSetSphere");

    // set the metatable
    luaL_getmetatable(L, s_name.c_str());
    lua_setmetatable(L, -2);

    return 1;
}

int
GridUserdata::destroy(lua_State *L)
{
    void *ud_ = luaL_checkudata(L, 1, s_name.c_str());
    auto *ud  = static_cast<GridUserdata *>(ud_);
    ud->~GridUserdata();
    return 0;
}

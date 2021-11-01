#ifndef PARTICLE_LIST_USER_DATA_H
#define PARTICLE_LIST_USER_DATA_H

#include "ParticleList.h"
#include <lua.h>

struct ParticleListUserdata
{
    std::shared_ptr<ParticleList> particle_list;

    static int create(lua_State *L);
    static int destroy(lua_State *L);

    static std::string s_name;
};

#endif

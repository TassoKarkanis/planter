#include "lua_bindings.h"
#include <iostream>

int
usage()
{
    std::cout << "Usage: planter <lua-file> [arg1 arg2 ...]\n";
    std::cout << "Generate an OBJ file using OpenVDB.\n";
    return 1;
}

int
main(int argc, char **argv)
{
    if (argc < 2)
        return usage();

    return cmd_lua(argc - 1, argv + 1);
}

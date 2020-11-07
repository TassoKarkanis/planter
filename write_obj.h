#ifndef WRITE_OBJ_H
#define WRITE_OBJ_H

#include <openvdb/openvdb.h>
#include <string>

void write_obj(const std::string &filename, const openvdb::FloatGrid &grid);

#endif

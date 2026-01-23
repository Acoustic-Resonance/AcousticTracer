/*
 * any types that are forward declared in at.h can be fully defined here, as opposed to its .c file
 * allows for easy includes between any src file
 */

#ifndef AT_INTERNAL_H
#define AT_INTERNAL_H

#include "acoustic/at.h"
#include "acoustic/at_math.h"

struct AT_Scene {
    AT_Source *sources;
    AT_AABB world_AABB;
    uint32_t num_sources;
    AT_Material material;
    const AT_Model *environment;
};

struct AT_Model {
    AT_Vec3 *vertices;
    // TODO: normals
    uint32_t *indices;
    size_t vertex_count;
    size_t index_count;
};

#endif // AT_INTERAL_H

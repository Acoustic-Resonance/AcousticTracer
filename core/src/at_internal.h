/*
 * any types that are forward declared in at.h can be fully defined here, as opposed to its .c file
 * allows for easy includes between any src file
 */

#ifndef AT_INTERNAL_H
#define AT_INTERNAL_H

#include "acoustic/at.h"
#include "acoustic/at_math.h"

// Private Types (typedef + define)
typedef struct AT_Ray AT_Ray;

struct AT_Ray {
    AT_Ray *child;
    AT_Vec3 origin;
    AT_Vec3 direction;
    float energy;
    float total_distance;
    uint32_t ray_id;
    uint32_t bounce_count;
};

// dynamic array structure
// called "items" instead of "bins" since the dynamic array macros are
// to be universal, can use them with any types.
// If we want, we can rewrite them to change items to bins to avoid confusion :|
typedef struct {
    float *items; //bins
    size_t count;
    size_t capacity;
} AT_Voxel;

// API Type definitions (just struct definitions, theyre already typedefed when forward declaring)
struct AT_Scene {
    AT_Source *sources;
    AT_AABB world_AABB;
    uint32_t num_sources;
    AT_Material material;
    const AT_Model *environment;
};

struct AT_Model {
    AT_Vec3 *vertices;
    AT_Vec3 *normals;
    uint32_t *indices;
    size_t vertex_count;
    size_t index_count;
};

struct AT_Simulation {
    //using the scene struct within the simulation struct we can access its members like this:
    // simulation->scene->sources etc..
    const AT_Scene *scene; //borrowed: must remain valid for the lifetime of AT_Simulation
    AT_Voxel *voxel_grid;
    AT_Ray *rays;
    AT_Vec3 origin;
    AT_Vec3 dimensions;
    AT_Vec3 grid_dimensions;
    float voxel_size;
    float bin_width;
    uint32_t num_rays;
    uint8_t fps;
};



#endif // AT_INTERAL_H

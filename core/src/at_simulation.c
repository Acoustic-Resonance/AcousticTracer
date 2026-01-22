#include "acoustic/at_simulation.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_scene.h"
#include "acoustic/at_ray.h"

#include <stdint.h>
#include <stdlib.h>

struct AT_Simulation {
    AT_Voxel *voxels;
    AT_Ray *rays;
    AT_Vec3 origin;
    AT_Vec3 dimensions;
    AT_Vec3 grid_dimensions;
    float voxel_size;
    uint32_t num_rays;
    float bin_width;
    uint8_t fps;
};

AT_Result AT_simulation_create(AT_Simulation **out_simulation, const AT_Scene *scene, const AT_Settings *settings)
{
    if (!scene || !settings) return AT_ERR_INVALID_ARGUMENT;
    if (settings->fps <= 0 || settings->voxel_size <= 0) return AT_ERR_INVALID_ARGUMENT;

    AT_Simulation *simulation = calloc(1, sizeof(AT_Simulation));

    simulation->rays = (AT_Ray*)malloc(sizeof(AT_Ray) * settings->num_rays);
    if (!simulation->rays) {
        free(simulation);
        return AT_ERR_ALLOC_ERROR;
    }

    AT_Vec3 dimensions = AT_vec3_sub(scene->world_AABB.max, scene->world_AABB.min);
    float grid_x = (dimensions.x / settings->voxel_size) + 1;
    float grid_y = (dimensions.y / settings->voxel_size) + 1;
    float grid_z = (dimensions.z / settings->voxel_size) + 1;
    uint32_t num_voxels = (uint32_t){grid_x * grid_y * grid_z};

    simulation->voxels = (AT_Voxel*)malloc(sizeof(AT_Voxel) * num_voxels);
    if (!simulation->voxels) {
        free(simulation->rays);
        free(simulation);
        return AT_ERR_ALLOC_ERROR;
    }

    // Gonna have to initialize a dynamic array for each voxel to store bins dynamically
    for (size_t i = 0; i < num_voxels; i++) {
        AT_voxel_init(&simulation->voxels[i]);
    }

    simulation->origin = scene->world_AABB.min;
    simulation->dimensions = dimensions;
    simulation->fps = settings->fps;
    simulation->num_rays = settings->num_rays;
    simulation->voxel_size = settings->voxel_size;
    simulation->grid_dimensions = (AT_Vec3){grid_x, grid_y, grid_z}; //dimensions in terms of voxels
    simulation->voxel_size = settings->voxel_size;
    simulation->num_rays = settings->num_rays;
    simulation->bin_width = 1.0f / settings->fps;

    // we dont know the length of the simulation at this point, so the bins will have
    // to be dynamic (dynamic array or linked list...)
    // each AT_Voxel will have its own array of "bins"

    *out_simulation = simulation;

    return AT_OK;
}

AT_Result AT_simulation_run(AT_Simulation *simulation) {
    return AT_ERR_INVALID_ARGUMENT; //TODO big boy function
}

void AT_simulation_destroy(AT_Simulation *simulation) {
    if (!simulation) return;
    free(simulation->voxels);
    free(simulation->rays);
    free(simulation);
}

#include "acoustic/at_simulation.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_scene.h"
#include "../src/at_voxel.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

AT_Result AT_simulation_create(AT_Simulation **out_simulation, const AT_Scene *scene, const AT_Settings *settings)
{
    if (!scene || !settings) return AT_ERR_INVALID_ARGUMENT;
    if (settings->fps <= 0 || settings->voxel_size <= 0.0f) return AT_ERR_INVALID_ARGUMENT;

    AT_Simulation *simulation = calloc(1, sizeof(AT_Simulation));
    if (!simulation) return AT_ERR_ALLOC_ERROR;

    simulation->rays = (AT_Ray*)malloc(sizeof(AT_Ray) * settings->num_rays);
    if (!simulation->rays) {
        free(simulation);
        return AT_ERR_ALLOC_ERROR;
    }

    // World dimensions
    AT_Vec3 dimensions = AT_vec3_sub(scene->world_AABB.max, scene->world_AABB.min);

    // Grid dimensions (num voxels in each dimension)
    float grid_x = ceilf(dimensions.x / settings->voxel_size);
    float grid_y = ceilf(dimensions.y / settings->voxel_size);
    float grid_z = ceilf(dimensions.z / settings->voxel_size);
    uint32_t num_voxels = (uint32_t)(grid_x * grid_y * grid_z);

    simulation->voxel_grid = calloc(num_voxels, sizeof(AT_Voxel));
    if (!simulation->voxel_grid) {
        free(simulation->rays);
        free(simulation);
        return AT_ERR_ALLOC_ERROR;
    }

    // Initialize each voxels bin dynamic array (we dont know the num bins yet)
    for (uint32_t i = 0; i < num_voxels; i++) {
        AT_voxel_init(&simulation->voxel_grid[i]);
    }

    simulation->origin = scene->world_AABB.min;
    simulation->dimensions = dimensions;
    simulation->fps = settings->fps;
    simulation->num_rays = settings->num_rays;
    simulation->grid_dimensions = (AT_Vec3){grid_x, grid_y, grid_z}; //dimensions in terms of voxels
    simulation->voxel_size = settings->voxel_size;
    simulation->bin_width = 1.0f / settings->fps;

    *out_simulation = simulation;

    return AT_OK;
}

AT_Result AT_simulation_run(AT_Simulation *simulation) {
    if (!simulation) return AT_ERR_INVALID_ARGUMENT;
    return AT_OK; //TODO big boy function
}

void AT_simulation_destroy(AT_Simulation *simulation) {
    if (!simulation) return;

    uint32_t num_voxels = (uint32_t)(simulation->grid_dimensions.x *
                                     simulation->grid_dimensions.y *
                                     simulation->grid_dimensions.z);

    for (uint32_t i = 0; i < num_voxels; i++) {
        AT_voxel_cleanup(&simulation->voxel_grid[i]);
    }

    free(simulation->voxel_grid);
    free(simulation->rays);
    free(simulation);
}

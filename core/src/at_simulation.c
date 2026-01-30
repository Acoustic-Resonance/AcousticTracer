#include "acoustic/at_simulation.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "acoustic/at_scene.h"
#include "../src/at_voxel.h"
#include "at_internal.h"
#include "at_ray.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

AT_Result AT_simulation_create(AT_Simulation **out_simulation, const AT_Scene *scene, const AT_Settings *settings)
{
    if (!scene || !settings) return AT_ERR_INVALID_ARGUMENT;
    if (settings->fps <= 0 || settings->voxel_size <= 0.0f) return AT_ERR_INVALID_ARGUMENT;

    AT_Simulation *simulation = calloc(1, sizeof(AT_Simulation));
    if (!simulation) return AT_ERR_ALLOC_ERROR;

    //need to store all rays per source
    simulation->rays = (AT_Ray*)calloc(settings->num_rays * scene->num_sources, sizeof(AT_Ray));
    if (!simulation->rays) {
        free(simulation);
        return AT_ERR_ALLOC_ERROR;
    }

    simulation->scene = scene;

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


#define MIN_RAY_ENERGY_THRESHOLD 10.0

AT_Result AT_simulation_run(AT_Simulation *simulation)
{
    if (!simulation) return AT_ERR_INVALID_ARGUMENT;

    uint32_t triangle_count = simulation->scene->environment->index_count / 3;
    AT_Triangle *triangles = NULL;
    if (AT_model_get_triangles(&triangles, simulation->scene->environment) != AT_OK) {
        return AT_ERR_ALLOC_ERROR;
    }

    //initialize and trace rays at every source
    for (uint32_t s = 0; s < simulation->scene->num_sources; s++) {
        //init rays for this source
        for (uint32_t r = 0; r < simulation->num_rays; r++) {
            uint32_t ray_idx = s * simulation->num_rays + r;

            //gpt ahh code
            AT_Vec3 varied_direction = simulation->scene->sources[s].direction;
            varied_direction.x += ((float)rand() / RAND_MAX - 0.5f) * 0.2f;  // ±0.1
            varied_direction.y += ((float)rand() / RAND_MAX - 0.5f) * 0.2f;  // ±0.1
            varied_direction.z += ((float)rand() / RAND_MAX - 0.5f) * 0.2f;  // ±0.1
            varied_direction = AT_vec3_normalize(varied_direction);

            simulation->rays[ray_idx] = AT_ray_init(
                simulation->scene->sources[s].position,
                varied_direction,
                ray_idx //ray index
            );
        }

        //trace rays for this source
        for (uint32_t i = 0; i < simulation->num_rays; i++) {
            uint32_t ray_idx = s * simulation->num_rays + i;
            AT_Ray *ray = &simulation->rays[ray_idx];
            uint32_t bounce_count = 0;
            //while (ray->energy > MIN_RAY_ENERGY_THRESHOLD) {
            while (bounce_count++ < 5) {
                AT_Ray closest = AT_ray_init((AT_Vec3){
                    FLT_MAX, FLT_MAX, FLT_MAX},
                    (AT_Vec3){0},
                    ray_idx);
                bool intersects = false;
                for (uint32_t t = 0; t < triangle_count; t++) {
                    if (AT_ray_triangle_intersect(ray, &triangles[t], &closest)) {
                        intersects = true;
                    }
                }
                if (!intersects) break;

                AT_Ray *child = (AT_Ray*)malloc(sizeof(AT_Ray));
                if (!child) return AT_ERR_ALLOC_ERROR;
                *child = closest;
                child->child = NULL;
                ray->child = child;
                ray = ray->child;
            }
        }
    }
    return AT_OK;
}


void AT_simulation_destroy(AT_Simulation *simulation) {
    if (!simulation) return;

    uint32_t num_voxels = (uint32_t)(simulation->grid_dimensions.x *
                                     simulation->grid_dimensions.y *
                                     simulation->grid_dimensions.z);

    for (uint32_t i = 0; i < num_voxels; i++) {
        AT_voxel_cleanup(&simulation->voxel_grid[i]);
    }

    uint32_t total_rays = simulation->scene->num_sources * simulation->num_rays;
    for (uint32_t i = 0; i < total_rays; i++) {
        if (simulation->rays[i].child) {
            AT_ray_destroy_children(simulation->rays[i].child);
        }
    }

    free(simulation->voxel_grid);
    free(simulation->rays);
    free(simulation);
}

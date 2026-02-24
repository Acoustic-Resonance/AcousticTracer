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
    // printf("grid_x : %f,grid_y: %f,grid_z: %f\n", grid_x, grid_y, grid_z);
    // printf("dim_x : %f,dim_y: %f,dim_z: %f\n", dimensions.x, dimensions.y, dimensions.z);
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
    simulation->num_voxels = num_voxels;
    simulation->grid_dimensions = (AT_Vec3){{grid_x, grid_y, grid_z}}; //dimensions in terms of voxels
    simulation->voxel_size = settings->voxel_size;
    simulation->bin_width = 1.0f / settings->fps;

    *out_simulation = simulation;

    return AT_OK;
}


#define SOURCE_ENERGY 1.0f //this can be the power of the sound source defined by the user

AT_Result AT_simulation_run(AT_Simulation *simulation)
{
    if (!simulation) return AT_ERR_INVALID_ARGUMENT;

    const float MIN_ENERGY_THRESHOLD = 0.8f / simulation->num_rays;
    const float SURFACE_EPSILON = simulation->voxel_size * 1e-4f;

    uint32_t triangle_count = simulation->scene->environment->index_count / 3;
    AT_Triangle *triangles = NULL;
    if (AT_model_get_triangles(&triangles, simulation->scene->environment) != AT_OK) {
        return AT_ERR_ALLOC_ERROR;
    }

    uint32_t num_children = 0;

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
                0.0f,
                SOURCE_ENERGY / simulation->num_rays,
                ray_idx //ray index
            );
        }
    }

    //trace rays for this source
    uint32_t total_rays = simulation->scene->num_sources * simulation->num_rays;
    for (uint32_t i = 0; i < total_rays; i++) {
        AT_Ray *ray = &simulation->rays[i];
        while (ray->energy > MIN_ENERGY_THRESHOLD) {
            AT_Ray closest = AT_ray_init((AT_Vec3){{FLT_MAX, FLT_MAX, FLT_MAX}},
                (AT_Vec3){{0, 0, 0}},
                0.0f,
                0.0f,
                i);
            bool intersects = false;
            uint32_t tri_idx = 0;
            for (uint32_t t = 0; t < triangle_count; t++) {
                if (AT_ray_triangle_intersect(ray, &triangles[t], &closest)) {
                    intersects = true;
                    tri_idx = t;
                }
            }
            if (!intersects) break;
            AT_Ray *child = (AT_Ray*)malloc(sizeof(AT_Ray));
            if (!child) return AT_ERR_ALLOC_ERROR;
            *child = closest;
            child->child = NULL;
            child->ray_id = ray->ray_id + simulation->num_rays;
            //AT_Vec3 hit_point = closest.origin;
            ray->hit_point = closest.origin;

            //slightly offset child origin to avoid percision issues (when hitting corners and such)
            const float SURFACE_EPSILON = 0.001f;
            AT_Vec3 offset = AT_vec3_scale(closest.direction, SURFACE_EPSILON);
            child->origin = AT_vec3_add(ray->hit_point, offset);

            child->total_distance = ray->total_distance + AT_vec3_distance(ray->origin, ray->hit_point);
            child->energy = ray->energy * (1.0f - AT_MATERIAL_TABLE[simulation->scene->environment->triangle_materials[tri_idx]].absorption);
            ray->child = child;
            ray = ray->child;
            num_children++;
        }
        if (ray->energy < MIN_ENERGY_THRESHOLD) ray->has_died = true;
    }

    printf("Number of child rays: %i\n", num_children);

    //DDA

    for (uint32_t i = 0; i < total_rays; i++) {
        AT_Ray *ray = &simulation->rays[i];

        while (ray) {
            AT_Vec3 ray_end;
            //if the ray has an endpoint, set it
            if (ray->child) {
                ray_end = ray->hit_point;
            //if the ray doesnt have an end point but hasnt died, continue it for max_AABB distance
            } else if (!ray->has_died) {
               ray_end = AT_vec3_add(
                         ray->origin,
                         AT_vec3_scale(
                             ray->direction,
                             AT_vec3_distance(
                                 simulation->scene->world_AABB.min,
                                 simulation->scene->world_AABB.max
                             )
                         )
                     );
               //otherwise if it has died just break
            } else {
                break;
            }

            AT_voxel_ray_step(simulation, ray, ray_end);
            ray = ray->child;
        }
    }
    free(triangles);
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

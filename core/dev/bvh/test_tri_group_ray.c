#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_ray.h"
#include "../src/at_trigroup.h"
#include "../src/at_utils.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

bool aabb_intersects(AT_AABB *aabb, AT_Ray *ray)
{
    float t_min = 0.0f, t_max = FLT_MAX;
    AT_Vec3 min = aabb->min, max = aabb->max;
    AT_Vec3 origin = ray->origin, dir = ray->direction;
    bool sign;
    float box_min, box_max, dim_min, dim_max;
    float inv_dir[3] = {1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z};
    float corners[2][3] = {
        {min.x, min.y, min.z},
        {max.x, max.y, max.z}
    };

    for (int d = 0; d < 3; d++) {
        sign = signbit(inv_dir[d]);
        box_min = corners[sign][d];
        box_max = corners[!sign][d];

        dim_min = (box_min - origin.arr[d]) * inv_dir[d];
        dim_max = (box_max - origin.arr[d]) * inv_dir[d];

        t_min = AT_max(dim_min, t_min);
        t_max = AT_min(dim_max, t_max);
    }

    return t_min < t_max;
}

int main()
{
    const char *filepath = "../assets/glb/Sponza.glb";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }

    for (uint32_t i = 0; i < model->vertex_count; i++) {
        model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.01);
    }

    AT_TriangleArrays *triangle_arrs = NULL;
    if (AT_triangle_arrays_create(&triangle_arrs, model) != AT_OK) {
        perror("Failed to create triangle arrays");
        AT_model_destroy(model);
        return 1;
    }
    uint32_t t_count = model->index_count / 3;

    AT_BVHConfig bvh_config = {
        .mini_tree_size = 100,
    };
    AT_TriangleGroups *tri_groups = NULL;
    if (AT_triangle_groups_create(&tri_groups, t_count) != AT_OK) {
        perror("Failed to create the triangle groups holder");
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }
    if (AT_trigroup_split(triangle_arrs, t_count, tri_groups, bvh_config.mini_tree_size) != AT_OK) {
        perror("Failed to split the triangle group");
        AT_triangle_groups_destroy(tri_groups);
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }

    // iterate rays
    for (int iter = 0; iter < 2; iter++) {
        time_t start_time, current_time;
        time(&start_time);
        time(&current_time);
        float elapsed = 0;
        int num_rays = 0;
        int num_bounces = 5;
        while (elapsed < 1) {
            uint32_t count = 0;
            AT_Ray ray = AT_ray_init(
                (AT_Vec3){0},
                (AT_Vec3){{num_rays * elapsed, 0.1f, -1.0f}},
                0.0f,
                0.0f,
                0
            );
            AT_Ray *ray_ptr = &ray;
            while (count++ < num_bounces) {
                AT_Ray closest = AT_ray_init((AT_Vec3){FLT_MAX, FLT_MAX, FLT_MAX}, (AT_Vec3){0}, 0.0f, 0.0f, 0);
                bool intersects = false;
                if (iter == 1) {
                    for (uint32_t j = 0; j < tri_groups->num_groups; j++) {
                        AT_TriGroup *group = tri_groups->groups[j];
                        if (aabb_intersects(&group->aabb, ray_ptr)) {
                            for (uint32_t k = 0; k < group->num_tri; k++) {
                                if (AT_ray_triangle_intersect(ray_ptr, &AT_get_triangle(group, 3, k), &closest)) {
                                    intersects = true;
                                }
                            }
                        }
                    }
                } else {
                    for (uint32_t k = 0; k < t_count; k++) {
                        if (AT_ray_triangle_intersect(ray_ptr, &AT_get_triangle_by_arr(0, 3, k), &closest)) {
                            intersects = true;
                        }
                    }
                }
                if (!intersects) break;
                AT_Ray *child = malloc(sizeof(AT_Ray));
                *child = closest;
                child->child = NULL;
                ray_ptr->child = child;
                ray_ptr = ray_ptr->child;
            }
            AT_ray_destroy(ray_ptr);
            time(&current_time);
            elapsed = current_time - start_time;
            num_rays++;
        }
        printf("Start: %ld, end: %ld, elapsed: %f. ", start_time, current_time, elapsed);
        printf("In one second, %d rays were created and bounced %d times.\n", num_rays, num_bounces);
    }

    AT_triangle_groups_destroy(tri_groups);
    AT_triangle_arrays_destroy(triangle_arrs);
    AT_model_destroy(model);
    return 0;
}

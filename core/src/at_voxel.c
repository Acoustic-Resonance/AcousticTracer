#include "acoustic/at_math.h"
#include "at_voxel.h"
#include "at_internal.h"
#include "../src/at_utils.h"
#include <math.h>
#include <stdint.h>

#define VOXEL_MAX_STEPS 100
#define SPEED_OF_SOUND 343.0f
#define SLOWER_SPEED 10.0f

void AT_voxel_ray_step(AT_Simulation *simulation, AT_Ray *ray, AT_Vec3 ray_end)
{
    //the ray segment spans from p0 (origin) to p1 (end)
    // out current position within the segement is "t"

    float world_ray_length = AT_vec3_distance(ray->origin, ray_end);
    if (world_ray_length <= 0.0f) return;

    //origin in voxel space
    AT_Vec3 p0 = AT_vec3_scale(
        AT_vec3_sub(ray->origin, simulation->origin),
        1.0f / simulation->voxel_size
    );

    //ray end in voxel space
    AT_Vec3 p1 = AT_vec3_scale(
        AT_vec3_sub(ray_end, simulation->origin),
        1.0f / simulation->voxel_size
    );

    const int grid_x = simulation->grid_dimensions.x;
    const int grid_y = simulation->grid_dimensions.y;
    const int grid_z = simulation->grid_dimensions.z;

    if (p0.x < 0.0f || p0.y < 0.0f || p0.z < 0.0f ||
        p0.x >= grid_x || p0.y >= grid_y || p0.z >= grid_z) {
            return;
        }

    //step direction (+1 or -1 per axis)
    //const AT_Vec3 step = AT_get_sign_vec3(ray->direction);
    const AT_Vec3 step = (AT_Vec3) {{
        signbit(ray->direction.x) ? -1 : 1,
        signbit(ray->direction.y) ? -1 : 1,
        signbit(ray->direction.z) ? -1 : 1
    }};

    //distance along "t" to move one voxel
    const AT_Vec3 delta = AT_vec3_delta(ray->direction);

    AT_Vec3i pos = (AT_Vec3i){
        AT_clamp((int)floorf(p0.x), 0, grid_x - 1),
        AT_clamp((int)floorf(p0.y), 0, grid_y - 1),
        AT_clamp((int)floorf(p0.z), 0, grid_z - 1)
    };

    //distance along ray until we cross next voxel boundary each axis
    // aka the max "t" :|
    // since the ray can only leave through one face of the voxel first,
    // the smallest of the three t_max values tells us which face
    //this gif kinda helps visualize it: https://m4xc.dev/anim/articles/amanatides-and-woo/walk-anim.mp4
    AT_Vec3 t_max;

    t_max.x = (step.x > 0) ?
        ((pos.x + 1.0f) - p0.x) * delta.x :
        (p0.x - pos.x) * delta.x;

    t_max.y = (step.y > 0) ?
        ((pos.y + 1.0f) - p0.y) * delta.y :
        (p0.y - pos.y) * delta.y;

    t_max.z = (step.z > 0) ?
        ((pos.z + 1.0f) - p0.z) * delta.z :
        (p0.z - pos.z) * delta.z;

    // printf("Ray: %i: CHILD:%p ", ray->ray_id, ray->child);
    // printf("p0: {%f, %f, %f}, ", p0.x, p0.y, p0.z);
    // printf("p1: {%f, %f, %f}, ", p1.x, p1.y, p1.z);
    // printf("Step: {%.1f, %.1f, %.1f}, ", step.x, step.y, step.z);
    // printf("Delta: {%f, %f, %f}", delta.x, delta.y, delta.z);
    // printf("t_max: {%f, %f, %f}\n", t_max.x, t_max.y, t_max.z);

    //curr pos within ray segment
    float t = 0.0f;
    const float t_end = AT_vec3_length(AT_vec3_sub(p1, p0));
    float t_prev = 0.0f;

    //while we havent yet reached the end of the ray segment
    while (t < t_end) {
        if (pos.x < 0 || pos.x >= grid_x ||
            pos.y < 0 || pos.y >= grid_y ||
            pos.z < 0 || pos.z >= grid_z) break;

        //index into the voxel_grid array
        const uint32_t voxel_idx =
            (uint32_t)pos.z * grid_y * grid_x +
            (uint32_t)pos.y * grid_x +
            (uint32_t)pos.x;

        //printf("VOXEL IDX: %i\n", voxel_idx);
        float t_current = fminf(t_max.x, fminf(t_max.y, t_max.z));
        if (t_current > t_end) break; //if we reached the end of the ray segment

        float t_segment = t_current - t_prev; //how far we moved in voxel space

        if (t_segment > EPSILON) {
            float world_segment = t_segment * simulation->voxel_size; //how far in world space

            float t_midpoint = t_prev + t_segment * 0.5f; //center point of curr voxel
            //total dist from source to this midpoint
            float total_world_dist = ray->total_distance + (t_midpoint * simulation->voxel_size);

            //inverse square law - attenuation
            float dist_from_source = fmaxf(total_world_dist, 0.1f);
            float intensity_factor = 1.0f / (1.0f + dist_from_source * 0.01f);

            float air_coefficient = 0.01f;
            float air_absorbtion = expf(-air_coefficient * total_world_dist);
            float energy_deposit = (ray->energy * world_segment / world_ray_length) * intensity_factor * air_absorbtion;

            float curr_time = total_world_dist / SPEED_OF_SOUND;
            //float curr_time = total_world_dist / SLOWER_SPEED;

            size_t bin_index = (size_t)(curr_time / simulation->bin_width);
            //printf("BIN INDEX: %zu\n", bin_index);

            AT_Voxel *voxel = &simulation->voxel_grid[voxel_idx];

            //grow bin count
            while (voxel->count <= bin_index) {
                AT_voxel_bin_append(voxel, 0.0f);
            }

            if (AT_voxel_add_energy(voxel, energy_deposit, bin_index) != AT_OK) {
                break;
            }
        }
        //printf("ENERGY: %f\n", energy_deposit);

        t_prev = t_current;
        t = t_current;

        //now we look for smallest t_max and advance through it
        //t_max.x smallest
        if (t_max.x < t_max.y && t_max.x < t_max.z) {
            pos.x += step.x;
            t_max.x += delta.x;
        } else if (t_max.y < t_max.z) { //smallest y
            pos.y += step.y;
            t_max.y += delta.y;
        } else { //smallest z
            pos.z += step.z;
            t_max.z += delta.z;
        }
    }
}

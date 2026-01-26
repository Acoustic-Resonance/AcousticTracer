#ifndef AT_RAY_H
#define AT_RAY_H

#include "../src/at_internal.h"
#include "acoustic/at_math.h"
#include "../src/at_utils.h"

#include <stdbool.h>

#define AT_RAY_MAX_ENERGY 100.0f

static inline AT_Ray AT_ray_init(
    const AT_Vec3 origin,
    const AT_Vec3 direction,
    uint32_t ray_id
) {

    AT_Ray ray = {
        .origin = origin,
        .direction = AT_vec3_normalize(direction),
        .energy = AT_RAY_MAX_ENERGY,
        .total_distance = 0.0f,
        .ray_id = ray_id,
        .bounce_count = 0,
    };
    AT_da_init(&ray.hits);

    return ray;
}

//da wrapper
static inline void AT_ray_add_hit(AT_Ray *ray, AT_RayHit hit)
{
    AT_da_append(&ray->hits, hit);
}

static inline AT_Vec3 AT_ray_at(const AT_Ray *ray, float t)
{
    return (AT_vec3_add(ray->origin, AT_vec3_scale(ray->direction, t)));
}

static inline AT_Vec3 AT_ray_reflect(AT_Vec3 incident,
                                     AT_Vec3 normal)
{
    AT_Vec3 u = AT_vec3_scale(
        normal, (AT_vec3_dot(incident, normal) / AT_vec3_dot(normal, normal)));
    AT_Vec3 w = AT_vec3_sub(incident, u);

    return AT_vec3_sub(w, u);
}

static inline void AT_ray_destroy(AT_Ray *ray)
{
    AT_da_free(&ray->hits);
}


bool AT_ray_triangle_intersect(const AT_Ray *ray,
                               const AT_Triangle *triangle,
                               AT_RayHit *out_hit);

#endif // AT_RAY_H

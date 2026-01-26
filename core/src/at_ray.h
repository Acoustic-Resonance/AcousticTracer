#ifndef AT_RAY_H
#define AT_RAY_H

#include "../src/at_internal.h"
#include "acoustic/at_math.h"

#include <stdbool.h>

static inline AT_Ray AT_ray_init(
    const AT_Vec3 *origin,
    const AT_Vec3 *direction,
    float energy,
    uint32_t ray_id
) {
    AT_Ray ray = {
        .origin = *origin,
        .direction = AT_vec3_normalize(*direction),
        .energy = energy,
        .total_distance = 0.0f,
        .ray_id = ray_id,
        .bounce_count = 0,
    };
    return ray;
}

static inline AT_Vec3 AT_ray_at(
    const AT_Ray *ray,
    float t
) {
    return (AT_vec3_add(ray->origin, AT_vec3_scale(ray->direction, t)));
}

static inline AT_Vec3 AT_ray_reflect(AT_Vec3 incident, AT_Vec3 normal) {
    AT_Vec3 u = AT_vec3_scale(
        normal, (AT_vec3_dot(incident, normal) / AT_vec3_dot(normal, normal)));
    AT_Vec3 w = AT_vec3_sub(incident, u);

    return AT_vec3_sub(w, u);
}

bool AT_ray_triangle_intersect(
    const AT_Ray *ray,
    const AT_Triangle *triangle,
    AT_RayHit *out_hit
);



#endif // AT_RAY_H

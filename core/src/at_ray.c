#include "../src/at_ray.h"

#define EPSILON 1e-6f

//Möller–Trumbore intersection alg
bool AT_ray_triangle_intersect(const AT_Ray *ray, const AT_Triangle *triangle, AT_RayHit *out_hit)
{
    AT_Vec3 edge1 = AT_vec3_sub(triangle->v2, triangle->v1);
    AT_Vec3 edge2 = AT_vec3_sub(triangle->v3, triangle->v1);

    AT_Vec3 pvec = AT_vec3_cross(ray->direction, edge2);
    float det  = AT_vec3_dot(edge1, pvec);
    if (fabs(det) < EPSILON) return false;

    float inv_det = 1.0f / det;
    AT_Vec3 tvec = AT_vec3_sub(ray->origin, triangle->v1);

    float u = AT_vec3_dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) return false;

    AT_Vec3 qvec = AT_vec3_cross(tvec, edge1);
    float v = AT_vec3_dot(ray->direction, qvec) * inv_det;
    if (v < 0 || u + v > 1) return false;

    float t = AT_vec3_dot(edge2, qvec) * inv_det;
    if (t < EPSILON) return false;

    out_hit->t = t;
    out_hit->position = (AT_Vec3){
        .x = ray->origin.x + ray->direction.x * t,
        .y = ray->origin.y + ray->direction.y * t,
        .z = ray->origin.z + ray->direction.z * t,
    };
    AT_Vec3 normal = AT_vec3_normalize(AT_vec3_cross(edge1, edge2));
    if (AT_vec3_dot(normal, ray->direction) > 0) normal = AT_vec3_scale(normal, -1);
    out_hit->normal = normal;

    return true;
}

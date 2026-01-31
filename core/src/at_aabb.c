#include "../src/at_aabb.h"

AT_Vec3 AT_AABB_calc_midpoint(AT_AABB *aabb)
{
    float half = 0.5f;
    AT_Vec3 midpoint = AT_vec3_zero();
    midpoint.x = (aabb->min.x + aabb->max.x) * half;
    midpoint.y = (aabb->min.y + aabb->max.y) * half;
    midpoint.z = (aabb->min.z + aabb->max.z) * half;

    return midpoint;
}

AT_AABB AT_AABB_from_triangle(const AT_Triangle *tri)
{
    AT_AABB out_aabb;
    out_aabb.min = (AT_Vec3){
        AT_min(AT_min(tri->v1.x, tri->v2.x), tri->v3.x),
        AT_min(AT_min(tri->v1.y, tri->v2.y), tri->v3.y),
        AT_min(AT_min(tri->v1.z, tri->v2.z), tri->v3.z),
    };
    out_aabb.max = (AT_Vec3){
        AT_max(AT_max(tri->v1.x, tri->v2.x), tri->v3.x),
        AT_max(AT_max(tri->v1.y, tri->v2.y), tri->v3.y),
        AT_max(AT_max(tri->v1.z, tri->v2.z), tri->v3.z),
    };
    out_aabb.midpoint = AT_AABB_calc_midpoint(&out_aabb);

    return out_aabb;
}
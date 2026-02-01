#include "../src/at_aabb.h"
#include "at_utils.h"

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

void AT_AABB_grow(AT_AABB *out_aabb, AT_Vec3 pt)
{
    float half = 0.5f;

    if (AT_min(out_aabb->min.x, pt.x) == pt.x) {
        out_aabb->min.x = pt.x;
        out_aabb->midpoint.x = (pt.x + out_aabb->max.x) * half;
    } else if (AT_max(out_aabb->max.x, pt.x) == pt.x) {
        out_aabb->max.x = pt.x;
        out_aabb->midpoint.x = (out_aabb->min.x + pt.x) * half;
    }
    if (AT_min(out_aabb->min.y, pt.y) == pt.y) {
        out_aabb->min.y = pt.y;
        out_aabb->midpoint.y = (pt.y + out_aabb->max.y) * half;
    } else if (AT_max(out_aabb->max.y, pt.y) == pt.y) {
        out_aabb->max.y = pt.y;
        out_aabb->midpoint.y = (out_aabb->min.y + pt.y) * half;
    }
    if (AT_min(out_aabb->min.z, pt.z) == pt.z) {
        out_aabb->min.z = pt.z;
        out_aabb->midpoint.z = (pt.z + out_aabb->max.z) * half;
    } else if (AT_max(out_aabb->max.z, pt.z) == pt.z) {
        out_aabb->max.z = pt.z;
        out_aabb->midpoint.z = (out_aabb->min.z + pt.z) * half;
    }
}

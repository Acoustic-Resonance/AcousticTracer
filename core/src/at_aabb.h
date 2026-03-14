#ifndef AT_AABB_H
#define AT_AABB_H

#include "../src/at_utils.h"
#include "acoustic/at_math.h"
#include <float.h>

/** \brief Initialises a blank AABB. */
static inline AT_AABB AT_AABB_init()
{
    return (AT_AABB){
        .min = {FLT_MAX, FLT_MAX, FLT_MAX},
        .max = {{-FLT_MAX, -FLT_MAX, -FLT_MAX}},
        .SA = 0,
    };
}

/** \brief Create the AABB for a given AT_Triangle. */
AT_AABB AT_AABB_from_triangle(const AT_Triangle *tri);

/** \brief Calculate the midpoint of an AT_AABB. */
AT_Vec3 AT_AABB_calc_midpoint(AT_AABB *aabb);

/** \brief Grows an AABB to include a given point. */
void AT_AABB_grow(AT_AABB *out_aabb, AT_Vec3 pt);

/** \brief Join two AT_AABBs together. */
static inline AT_AABB AT_AABB_join(AT_AABB a, AT_AABB b)
{
    AT_AABB out_aabb;
    out_aabb.min = (AT_Vec3){
        {AT_min(a.min.x, b.min.y), AT_min(a.min.y, b.min.y), AT_min(a.min.z, b.min.z)}
    };
    out_aabb.max = (AT_Vec3){
        {AT_max(a.max.x, b.max.y), AT_max(a.max.y, b.max.y), AT_max(a.max.z, b.max.z)}
    };
    out_aabb.midpoint = AT_AABB_calc_midpoint(&out_aabb);

    return out_aabb;
}

static inline float AT_AABB_get_SA(AT_AABB aabb)
{
    float diffs[3];
    for (int i = 0; i < 3; i++) {
        diffs[i] = fabsf(aabb.max.arr[i] - aabb.min.arr[i]);
    }

    return 2 * ((diffs[0] * diffs[1]) + (diffs[0] * diffs[2]) + (diffs[1] * diffs[2]));
}

#endif // AT_AABB_H

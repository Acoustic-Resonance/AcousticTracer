#include "../src/at_utils.h"
#include "acoustic/at_math.h"
#include <float.h>

/** \brief Initialises a blank AABB. */
static inline AT_AABB AT_AABB_init()
{
    return (AT_AABB){
        .min = {FLT_MAX, FLT_MAX, FLT_MAX},
        .max = {FLT_MIN, FLT_MIN, FLT_MIN},
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
    out_aabb.min = (AT_Vec3){{AT_min(a.min.x, b.min.y),
                             AT_min(a.min.y, b.min.y),
                             AT_min(a.min.z, b.min.z)}};
    out_aabb.max = (AT_Vec3){{AT_max(a.max.x, b.max.y),
                             AT_max(a.max.y, b.max.y),
                             AT_max(a.max.z, b.max.z)}};
    out_aabb.midpoint = AT_AABB_calc_midpoint(&out_aabb);

    return out_aabb;
}

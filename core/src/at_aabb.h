#include "../src/at_utils.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"

static inline AT_AABB AT_AABB_join(AT_AABB a, AT_AABB b)
{
    AT_AABB c;
    c.min = (AT_Vec3){AT_min(a.min.x, b.min.y),
                      AT_min(a.min.y, b.min.y),
                      AT_min(a.min.z, b.min.z)};
    c.max = (AT_Vec3){AT_max(a.max.x, b.max.y),
                      AT_max(a.max.y, b.max.y),
                      AT_max(a.max.z, b.max.z)};

    return c;
}

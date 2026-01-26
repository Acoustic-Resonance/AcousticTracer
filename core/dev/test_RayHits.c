#include "../src/at_internal.h"
#include "../src/at_ray.h"
#include "acoustic/at_math.h"
#include <stdio.h>


int main()
{
    printf("RayHit!\n");

    AT_Ray r = AT_ray_init((AT_Vec3){0, 0, 0}, (AT_Vec3){1, 0, 0}, 0);

    AT_RayHit h = {
        .normal = (AT_Vec3){5, 4, 6},
        .position = (AT_Vec3){1, 2, 3},
        .t = 0.0f
    };

    printf("Ray Hit List (Before Append): count: %zu - capacity: %zu\n", r.hits.count, r.hits.capacity);
    AT_ray_add_hit(&r, h);

    printf("Ray Hit List (After Append): count: %zu - capacity: %zu\n", r.hits.count, r.hits.capacity);

    for (size_t i = 0; i < r.hits.count; i++) {
        printf("Hit %zu: pos: {%.2f, %.2f, %.2f} - normal: {%.2f, %.2f, %.2f} - t: %.2f\n",
            i,
            r.hits.items[i].position.x, r.hits.items[i].position.y, r.hits.items[i].position.z,
            r.hits.items[i].normal.x, r.hits.items[i].normal.y, r.hits.items[i].normal.z,
            r.hits.items[i].t
            );
    }

    AT_ray_destroy(&r);

    return 0;
}

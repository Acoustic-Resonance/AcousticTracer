#ifndef AT_UTILS_H
#define AT_UTILS_H

#ifndef AT_ASSERT
#include <assert.h>
#define AT_ASSERT assert
#endif //AT_ASSERT

#ifndef AT_FREE
#include <stdlib.h>
#define AT_FREE free
#endif //AT_FREE

#ifndef AT_REALLOC
#include <stdlib.h>
#define AT_REALLOC realloc
#endif //AT_REALLOC

#ifndef AT_MALLOC
#include <stdlib.h>
#define AT_MALLOC malloc
#endif //AT_MALLOC

#ifndef AT_CALLOC
#include <stdlib.h>
#define AT_CALLOC calloc
#endif //AT_CALLOC

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "acoustic/at_math.h"

/* DYNAMIC ARRAYS */

// the DA functions assume the following type definition for any type you want:
//
// typedef struct {
//      YOUR_TYPE *items;
//      size_t     count;
//      size_t     capacity;
// } NameOfYourDA;
//

#define AT_DA_INITIAL_CAPACITY 256

#define AT_da_init(da) \
    do { \
        (da)->items = NULL;\
        (da)->count = 0;\
        (da)->capacity = 0;\
    } while(0)

#define AT_da_clear(da) ((da)->count = 0)
#define AT_da_is_empty(da) ((da)->count == 0)

#define AT_da_reserve(da, expected_capacity) \
    do {\
        if ((expected_capacity) > (da)->capacity) {\
            if ((da)->capacity == 0) {\
                (da)->capacity = AT_DA_INITIAL_CAPACITY;\
            }\
            while ((da)->capacity < expected_capacity) {\
                (da)->capacity *= 2;\
            }\
            (da)->items = AT_REALLOC((da)->items, (da)->capacity * sizeof((da)->items[0]));\
            AT_ASSERT((da)->items != NULL);\
        }\
    } while(0)

#define AT_da_append(da, item) \
    do {\
        AT_da_reserve(da, (da)->count + 1);\
        (da)->items[(da)->count++] = (item);\
    } while(0)

#define AT_da_last(da) \
    ({ \
        AT_ASSERT((da)->count > 0); \
        (da)->items[(da)->count-1]; \
    })

#define AT_da_pop(da) \
    ({ \
        AT_ASSERT((da)->count > 0); \
        (da)->items[--(da)->count]; \
    })

#define AT_da_remove(da, idx) \
    ({ \
        AT_ASSERT(idx < (da)->count); \
        typeof((da)->items[0]) __temp = (da)->items[(idx)]; \
        (da)->items[(idx)] = AT_da_last((da)); \
        (da)->items[(da)->count-1] = __temp; \
        AT_da_pop(da); \
    })

// user provides some label (i) -> macro initializes it as a pointer to (da)->items
// elements can be accessed by dereferencing (i)
// to get index 0, suntract (i) from (da)->items
#define AT_da_foreach(da, i) for (typeof(*(da)->items)* (i) = (da)->items; (i) < (da)->items + (da)->count; (i)++)

#define AT_da_free(da) \
    do { \
        AT_FREE((da)->items); \
        AT_da_init(da); \
    } while(0)


/* MIN / MAX */
#define AT_min(_a, _b) \
    ({ \
        (_a) < (_b) ? (_a) : (_b); \
    })

#define AT_max(_a, _b) \
    ({ \
        (_a) > (_b) ? (_a) : (_b); \
    })

//clamps x between [a, b]
#define AT_clamp(a, x, b) (((x) < (a)) ? (a) : \
		((b) < (x)) ? (b) : (x))

// returns the sign of a float (position +1, negative -1)
static inline int AT_get_sign_float(const float f)
{
    return (f > 0.0f) - (f < 0.0f);
}

static inline AT_Vec3 AT_get_sign_vec3(const AT_Vec3 v)
{
   return (AT_Vec3){{
       AT_get_sign_float(v.x),
       AT_get_sign_float(v.y),
       AT_get_sign_float(v.z)
   }};
}

// Get a triangle from index and axis
#define AT_get_triangle(group, array_idx, idx) group->triangle_arrs->triangles_db[group->triangle_arrs->arrs[array_idx][group->start + idx]]
#define AT_get_triangle_by_arr(start, array_idx, idx) triangle_arrs->triangles_db[triangle_arrs->arrs[array_idx][start + idx]]

static inline float AT_get_random_float()
{
    return (float)rand() / RAND_MAX;
}

#define AT_PI 3.14159265358979323846

//https://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/2D_Sampling_with_Multidimensional_Transformations
static inline AT_Vec3 AT_sample_cosine_hemisphere(AT_Vec3 normal)
{
    // printf("Normal: {%f, %f, %f}\n", normal.x, normal.y, normal.z);
    float u1 = AT_get_random_float();
    float u2 = AT_get_random_float();
    // printf("u1: %.2f, u2: %.2f\n", u1, u2);

    float theta = acos(sqrt(1.0f - u1));
    float phi = 2.0f * AT_PI * u2;
    // printf("theta: %.2f, phi: %.2f\n", theta, phi);

    //dir around z
    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);
    // printf("{%.2f, %.2f, %.2f}\n", x, y, z);

    //orthonormal basis
    AT_Vec3 up = fabsf(normal.z) < 0.999f ? (AT_Vec3){{0, 0, 1}} : (AT_Vec3){{1, 0, 0}};
    AT_Vec3 tangent = AT_vec3_normalize(AT_vec3_cross(up, normal));
    AT_Vec3 bitangent = AT_vec3_cross(normal, tangent);

    AT_Vec3 tangent_x = AT_vec3_scale(tangent, x);
    AT_Vec3 bitangent_y = AT_vec3_scale(bitangent, y);
    AT_Vec3 normal_z = AT_vec3_scale(normal, z);

    return AT_vec3_normalize(
        AT_vec3_add(
            AT_vec3_add(tangent_x, bitangent_y),
            normal_z)
    );
}

#endif //AT_UTILS_H

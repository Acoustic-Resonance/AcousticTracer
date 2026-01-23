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

#endif //AT_UTILS_H

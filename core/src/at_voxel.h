#ifndef AT_VOXEL_H
#define AT_VOXEL_H

#include "acoustic/at.h"
#include "at_internal.h"
#include "../src/at_utils.h"
#include <stddef.h>
#include <stdint.h>

//these are pretty much voxel specific wrappers of the dynamic array
static inline AT_Result AT_voxel_init(AT_Voxel *voxel)
{
    if (!voxel) return AT_ERR_INVALID_ARGUMENT;
    AT_da_init(voxel);
    return AT_OK;
}

static inline AT_Result AT_voxel_bin_append(AT_Voxel *voxel, float bin)
{
    if (!voxel) return AT_ERR_INVALID_ARGUMENT;
    AT_da_append(voxel, bin);
    return AT_OK;
}

static inline AT_Result AT_voxel_add_energy(AT_Voxel *voxel, float energy, size_t bin_index)
{
    if (!voxel) return AT_ERR_INVALID_ARGUMENT;
    if (bin_index >= voxel->count) return AT_ERR_INVALID_ARGUMENT;

    voxel->items[bin_index] += energy;
    return AT_OK;
}

static inline void AT_voxel_print(AT_Voxel *voxel)
{
    printf("[");
    for (size_t i = 0; i < voxel->count; i++) {
        printf("%.3f, ", voxel->items[i]);
    }
    printf("]\n");
}

void AT_voxel_ray_step(AT_Simulation *simulation, AT_Ray *ray, AT_Vec3 ray_end);

static inline uint32_t AT_voxel_get_num_bins(AT_Simulation *simulation)
{
    uint32_t max_count = 0;
    for (uint32_t i = 0; i < simulation->num_voxels; i++) {
        if (simulation->voxel_grid[i].count > max_count) {
            max_count = simulation->voxel_grid[i].count;
        }
    }
    return max_count;
}

static inline void AT_voxel_cleanup(AT_Voxel *voxel)
{
    AT_da_free(voxel);
}

#endif //AT_VOXEL_H

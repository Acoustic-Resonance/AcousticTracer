#ifndef AT_VOXEL_H
#define AT_VOXEL_H

#include "acoustic/at.h"
#include "at_internal.h"

//these are pretty much voxel specific wrappers of the dynamic array
AT_Result AT_voxel_init(AT_Voxel *voxel);
AT_Result AT_voxel_bin_append(AT_Voxel *voxel, float bin);
void AT_voxel_cleanup(AT_Voxel *voxel);

#endif //AT_VOXEL_H

#include "../src/at_utils.h"
#include "../src/at_internal.h"

AT_Result AT_voxel_init(AT_Voxel *voxel)
{
    if (!voxel) return AT_ERR_INVALID_ARGUMENT;
    AT_da_init(voxel);
    return AT_OK;
}

void AT_voxel_cleanup(AT_Voxel *voxel)
{
    AT_da_free(voxel);
}

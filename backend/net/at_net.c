#include "at_net.h"
#include "../../core/src/at_internal.h"
#include "../../core/src/at_voxel.h"

AT_Result AT_simulation_to_json(cJSON **out_json, AT_Simulation *simulation)
{
    if (!out_json || *out_json || !simulation) return AT_ERR_INVALID_ARGUMENT;

    size_t FRAME_NUM_BUFFER_LENGTH = simulation->fps*60*1 + 6;
    cJSON *json = cJSON_CreateObject(); 

    AT_Voxel* voxels = simulation->voxel_grid;
    uint32_t num_voxels = simulation->num_voxels;
    uint32_t num_bins = AT_voxel_get_num_bins(simulation);
   
    for (uint32_t f = 0; f < num_bins; f++) {

        char frame_num[FRAME_NUM_BUFFER_LENGTH];
        sprintf(frame_num, "frame_%d", f);

        cJSON *frame_data = cJSON_CreateArray();

        for (uint32_t v = 0; v < num_voxels; v++) {

            AT_Voxel voxel = voxels[v];

            float energy = (f < voxel.count) ? voxel.items[f] : 0;

            // TODO: IF ENERGY OVER MIN THRESHOLD
            if (energy <= 0) continue;

            char voxel_num[num_voxels];
            sprintf(voxel_num, "%d", v);

            cJSON *voxel_data = cJSON_CreateObject();

            cJSON_AddNumberToObject(voxel_data, voxel_num, energy);
            cJSON_AddItemToArray(frame_data, voxel_data);
        }

        cJSON_AddItemToObject(json, frame_num, frame_data);
    }
    *out_json = json;
    return AT_OK;
}
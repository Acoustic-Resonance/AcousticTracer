#include "acoustic/at.h"
#include "acoustic/at_result.h"
#include <stdio.h>
#include "../src/at_internal.h"

int main()
{
    printf("Hey\n");

    const char *filepath = "../assets/glb/L_room.gltf";

    AT_Model *model = NULL;
    AT_Result res = AT_model_create(&model, filepath);
    AT_handle_result(res, "Error creating model\n");

    AT_Source s1 = {
        .direction = {1, 0, 0},
        .intensity = 50.0,
        .position = {0, 0, 0}
    };

    AT_Source s2 = {
        .direction = {1, 0, 0},
        .intensity = 50.0,
        .position = {0, 0, 0}
    };

    int num_sources = 2;
    AT_Source sources[num_sources];
    sources[0] = s1;
    sources[1] = s2;

    AT_SceneConfig conf = {
        .environment = model,
        .material = AT_MATERIAL_CONCRETE,
        .num_sources = num_sources,
        .sources = sources
    };

    AT_Scene *scene = NULL;
    if (AT_scene_create(&scene, &conf) != AT_OK) {
        fprintf(stderr, "Error creating scene\n");
        return 1;
    }

    printf("Num Sources: %i\n", scene->num_sources);

    AT_Settings settings = {
        .fps = 60,
        .num_rays = 10000,
        .voxel_size = 10
    };

    AT_Simulation *sim = NULL;
    if (AT_simulation_create(&sim, scene, &settings) != AT_OK) {
        //we need some way to print what kind of error it was (alloc, param..)
        fprintf(stderr, "Error creating simulation\n");
        return 1;
    }

   if (AT_simulation_run(sim) != AT_OK) {
       fprintf(stderr, "Error running simulation\n");
       return 1;
   }


   AT_scene_destroy(scene);
   AT_simulation_destroy(sim);

    return 0;
}

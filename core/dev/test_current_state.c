#include "acoustic/at.h"
#include <stdio.h>

int main()
{
    printf("Hey\n");

    const char *filepath = "../assets/glb/L_room.gltf";

    AT_Source src = {
        .direction = {1, 0, 0},
        .intensity = 50.0,
        .position = {0, 0, 0}
    };

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Error creating model\n");
        return 1;
    }

    AT_SceneConfig conf = {
        .environment = model,
        .material = AT_MATERIAL_CONCRETE,
        .num_sources = 1, //can prob just derive this from src?
        .source = &src
    };

    AT_Scene *scene = NULL;
    if (AT_scene_create(&scene, &conf) != AT_OK) {
        fprintf(stderr, "Error creating scene\n");
        return 1;
    }

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

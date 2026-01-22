#ifndef AT_H
#define AT_H

#include "at_math.h"
#include <stdint.h>
#include <stddef.h>

typedef struct AT_Model AT_Model;
typedef struct AT_Scene AT_Scene;
typedef struct AT_Simulation AT_Simulation;

typedef enum {
    AT_OK = 0,
    AT_ERR_INVALID_ARGUMENT,
    AT_ERR_ALLOC_ERROR,
} AT_Result;

typedef enum {
    AT_MATERIAL_CONCRETE,
    AT_MATERIAL_PLASTIC,
    AT_MATERIAL_WOOD
} AT_Material;

typedef struct {
    AT_Vec3 min;
    AT_Vec3 max;
} AT_AABB;

typedef struct {
    AT_Vec3 position;
    AT_Vec3 direction;
    float intensity; // relative to the source intensity
} AT_Source;

typedef struct {
    const AT_Source *source;
    uint32_t num_sources;
    AT_Material material;

    // Borrowed: must remain valid for the entire lifetime of the scene
    const AT_Model *environment;
} AT_SceneConfig;

typedef struct {
    float voxel_size; // Render resolution
    uint32_t num_rays;
    uint8_t fps; // Bin width is always one frame
} AT_Settings;

// Model
AT_Result AT_model_create(
    AT_Model **out_model,
    const char *filepath
);

void AT_model_destroy(
    AT_Model *model
);

void AT_model_to_AABB(
    AT_AABB *out_aabb,
    const AT_Model *model
);

// Scene
AT_Result AT_scene_create(
    AT_Scene **out_scene,
    const AT_SceneConfig* config
);

void AT_scene_destroy(
    AT_Scene *scene
);

// Simulation
// Creates the simulation "object" and allocates voxel memory
AT_Result AT_simulation_create(
    AT_Simulation **out_simulation,
    const AT_Scene *scene,
    const AT_Settings *settings
);

// Runs the actual simulation and updates voxel bins etc..
AT_Result AT_simulation_run(
    AT_Simulation *simulation
);

void AT_simulation_destroy(
    AT_Simulation *simulation
);

#endif // AT_H

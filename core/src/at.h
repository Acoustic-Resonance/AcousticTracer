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
    AT_ERR_NETWORK_ERROR
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
    const char *url;
    uint32_t timeout_ms;
    int *http_status_out;
    char *response_buf;
    size_t response_buf_size;
} AT_NetworkConfig;

typedef struct {
    AT_Vec3 position;
    AT_Vec3 direction;
    float intensity;
} AT_Source;

typedef struct {
    const AT_Source source; // Assuming one source for now
    uint32_t num_rays;
    uint32_t max_bounces;
    AT_Material material;

    // Borrowed: must remain valid for the entire lifetime of the scene
    const AT_Model *environment;
    const AT_AABB *observer_area;
} AT_SceneConfig;

typedef struct {
    uint32_t voxel_size;
    uint32_t replay_length;
    uint8_t fps;
} AT_Settings;

// Model
AT_Result AT_model_create(
    AT_Model **model,
    const char *filepath
);

void AT_model_destroy(
    AT_Model *model
);

void AT_model_to_AABB(
    AT_AABB *aabb,
    const AT_Model *model
);

// Scene
AT_Result AT_scene_create(
    AT_Scene **scene,
    const AT_SceneConfig* config
);

void AT_scene_destroy(
    AT_Scene *scene
);

// Simulation
// Creates the simulation "object" and allocates voxel memory
AT_Result AT_simulation_create(
    AT_Simulation **simulation,
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

AT_Result AT_scene_to_json(
    char *json,
    size_t max_len,
    size_t *bytes_written_out,
    const AT_Scene *scene
);

AT_Result AT_send_json_to_url(
    const char *json,
    const AT_NetworkConfig *config
);

#endif // AT_H

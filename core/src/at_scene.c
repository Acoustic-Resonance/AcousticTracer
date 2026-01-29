#include "acoustic/at_scene.h"
#include "acoustic/at.h"
#include "../src/at_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>



AT_Result AT_scene_create(AT_Scene **out_scene, const AT_SceneConfig* config)
{
    if (!out_scene || !config) return AT_ERR_INVALID_ARGUMENT;
    if (config->num_sources <= 0 || !config->sources) return AT_ERR_INVALID_ARGUMENT;
    if (!config->environment) return AT_ERR_INVALID_ARGUMENT;

    AT_Scene *scene = calloc(1, sizeof(AT_Scene));
    if (!scene) return AT_ERR_ALLOC_ERROR;

    scene->environment = config->environment;
    scene->material = config->material;
    scene->num_sources = config->num_sources;

    AT_model_to_AABB(&scene->world_AABB, config->environment);

    scene->sources = malloc(sizeof(AT_Source) * config->num_sources);
    if (!scene->sources) {
        free(scene);
        return AT_ERR_ALLOC_ERROR;
    }

    memcpy(scene->sources, config->sources, sizeof(AT_Source) * config->num_sources);

    *out_scene = scene;
    return AT_OK;
}

void AT_scene_destroy(AT_Scene *scene)
{
    if (!scene) return;
    free(scene->sources);
    free(scene);
}

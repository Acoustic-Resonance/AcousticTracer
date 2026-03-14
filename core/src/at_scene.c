#include "acoustic/at_scene.h"
#include "../src/at_internal.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "at_trigroup.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

AT_Result AT_scene_create(AT_Scene **out_scene, const AT_SceneConfig *config)
{
    if (!out_scene || !config) return AT_ERR_INVALID_ARGUMENT;
    if (config->num_sources <= 0 || !config->sources) return AT_ERR_INVALID_ARGUMENT;
    if (!config->environment) return AT_ERR_INVALID_ARGUMENT;

    AT_Scene *scene = calloc(1, sizeof(AT_Scene));
    if (!scene) return AT_ERR_ALLOC_ERROR;

    for (uint32_t t = 0; t < config->environment->index_count / 3; t++) {
        config->environment->triangle_materials[t] = config->material;
    }

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
    for (uint32_t i = 0; i < scene->num_sources; i++) {
        scene->sources[i].direction = AT_vec3_normalize(scene->sources[i].direction);
    }

    scene->triangle_arrs = NULL;
    if (AT_triangle_arrays_create(&scene->triangle_arrs, scene->environment) != AT_OK) {
        return AT_ERR_ALLOC_ERROR;
    }

    AT_TriangleGroups *tri_groups = NULL;
    uint32_t num_tri = scene->environment->index_count / 3;
    AT_Result res = AT_triangle_groups_create(&tri_groups, num_tri);
    if (res != AT_OK) {
        return res;
    }

    AT_BVHConfig bvh_config = (AT_BVHConfig){
        .mini_tree_size = 100,
        .intersection_cost = 1,
        .traversal_cost = 0.5f,
    };
    res = AT_trigroup_split(scene->triangle_arrs, num_tri, tri_groups, bvh_config.mini_tree_size);
    if (res != AT_OK) {
        return res;
    }

    scene->mini_trees = calloc(tri_groups->num_groups, sizeof(*scene->mini_trees));
    uint32_t num_minitrees = tri_groups->num_groups;
    for (uint32_t i = 0; i < num_minitrees; i++) {
        res = AT_MiniTree_create(&scene->mini_trees[i], tri_groups->groups[i], &bvh_config);
        if (res != AT_OK) {
            return res;
        }
        scene->num_trees++;
    }

    *out_scene = scene;

    AT_triangle_groups_destroy(tri_groups);
    return AT_OK;
}

void AT_scene_destroy(AT_Scene *scene)
{
    if (!scene) return;

    for (uint32_t i = 0; i < scene->num_trees; i++) {
        AT_MiniTree_destroy(scene->mini_trees[i]);
    }
    AT_triangle_arrays_destroy(scene->triangle_arrs);
    free(scene->sources);
    free(scene);
}

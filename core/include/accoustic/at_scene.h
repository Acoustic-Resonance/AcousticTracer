#ifndef AT_SCENE_H
#define AT_SCENE_H

#include "at.h"

typedef struct AT_Scene AT_Scene;

AT_Result AT_scene_create(AT_Scene **out_scene, const AT_SceneConfig* config);
void AT_scene_destroy(AT_Scene *scene);

#endif

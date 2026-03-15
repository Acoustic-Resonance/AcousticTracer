/** \file
    \brief AT_Scene and related functions
    \ingroup scene
*/

#ifndef AT_SCENE_H
#define AT_SCENE_H

#include "at.h"

/** \defgroup scene Scene */

/** \struct AT_Scene
    \brief Groups the necessary information representing the scene.
    \ingroup scene
 */
typedef struct AT_Scene AT_Scene;

/** \brief AT_Scene constructor for a given AT_SceneConfig.
    \relates AT_Scene
    \ingroup scene

    \param out_scene Pointer to an empty initialised AT_Scene.
    \param config Pointer to the scene's config.

    \retval AT_Result Saves the created scene at the location of the pointer,
   returning a result enum value.
*/
AT_Result AT_scene_create(AT_Scene **out_scene, const AT_SceneConfig *config);

/** \brief Destroys an allocated AT_Scene.
    \relatesalso AT_Scene
    \ingroup scene

    \param scene Pointer to an initialised AT_Scene.
*/
void AT_scene_destroy(AT_Scene *scene);

#endif // AT_SCENE_H

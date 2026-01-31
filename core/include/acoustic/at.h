/** \file
    \brief The libraries public types and functions
*/
#ifndef AT_H
#define AT_H

#include "acoustic/at_math.h"
#include <stdint.h>
#include <stddef.h>

typedef struct AT_Model AT_Model;
typedef struct AT_Scene AT_Scene;
typedef struct AT_Simulation AT_Simulation;

/** \brief Defines possible result types.
 */
typedef enum {
  AT_OK = 0,               /**< Function executed successfully. */
  AT_ERR_INVALID_ARGUMENT, /**< Incorrect arguments were given to the function.
                            */
  AT_ERR_ALLOC_ERROR,      /**< Memory allocation failed. */
} AT_Result;

/** \brief Defines possible material types.
 */
typedef enum {
    AT_MATERIAL_CONCRETE,
    AT_MATERIAL_PLASTIC,
    AT_MATERIAL_WOOD
} AT_Material;

/** \brief Groups the information required for the sound source.
 */
typedef struct {
    AT_Vec3 position;
    AT_Vec3 direction;
    float intensity; // relative to the source intensity
} AT_Source;

/** \brief Groups the scene config settings together.
 */
typedef struct {
  const AT_Source *sources; /**< Dynamic array of AT_Source types. */
  uint32_t num_sources;
  AT_Material material; /**< Material of the room. */

  // Borrowed: must remain valid for the entire lifetime of the scene
  const AT_Model *environment; /**< Pointer to the room object. */
} AT_SceneConfig;

/** \brief The simulation's settings. */
typedef struct {
  float voxel_size; /**< The renderer's resolution. */
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

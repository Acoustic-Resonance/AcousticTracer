/** \file
    \brief AT_Model and related functions
    \ingroup model
*/

#ifndef AT_MODEL_H
#define AT_MODEL_H

#include "at.h"

/** \defgroup model Model */

/** \struct AT_Model
    \brief Groups the necessary information representing the 3D model.
    \ingroup model
 */
typedef struct AT_Model AT_Model;

/** \brief AT_Model constructor for a given `glb` filepath.
    \relatesalso AT_Model
    \ingroup model

    \param out_model Pointer to an empty initialised AT_Model.
    \param filepath String showing the location of the file.

    \retval AT_Result Saves the created model at the location of the pointer, returning a result enum value.
*/
AT_Result AT_model_create(AT_Model **out_model, const char *filepath);

/** \brief Calculates the min and max of a model for AABB collision.
    \relatesalso AT_AABB
    \ingroup model

    \param out_aabb Pointer to an empty initialised AT_AABB.
    \param model Pointer to the model.
*/
void AT_model_to_AABB(AT_AABB *out_aabb, const AT_Model *model);

/** \brief Constructs all triangles in a given Model.
    \relatesalso AT_Model
    \ingroup model

    \param out_triangles Pointer an allocated empty array of triangles.
    \param model Pointer to an initialised AT_Model.

    \retval AT_Result saves the created triangles at the location of the pointer, returning a result enum value.
 */
AT_Result AT_model_get_triangles(AT_Triangle **out_triangles, const AT_Model *model);

/** \brief Destroys an allocated AT_Model.
    \relatesalso AT_Model
    \ingroup model

    \param model Pointer to an initialised AT_Model.
*/
void AT_model_destroy(AT_Model *model);

#endif // AT_MODEL_H

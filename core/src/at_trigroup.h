#ifndef AT_TRIGROUP
#define AT_TRIGROUP

#include "../src/at_bvh.h"
#include "acoustic/at.h"

#include <stdint.h>


/** \brief AT_TriGroup constructor for a given list of triangles.
    \relates AT_TriGroup

    \param out_tree Pointer to an empty initialised AT_TriGroup.
    \param triangles Array of triangles.
    \param n The number of triangles associated with the group.

    \retval AT_Result Saves the created triangle group at the location of the pointer, returning a result enum value.
 */
AT_Result AT_trigroup_create(AT_TriGroup **out_group, AT_Triangle *triangles, uint32_t n);

/** \brief Destroys an allocated AT_TriGroup.
    \relates AT_TriGroup

    \param trigroup Pointer to an initialised triangle group.

    \retval void
 */
void AT_trigroup_destroy(AT_TriGroup *trigroup);

AT_Result AT_triangle_groups_create(AT_TriangleGroups **out_tree, int num_ts);

void AT_triangle_groups_destroy(AT_TriangleGroups *mini_tree);

/** \brief Splits triangle group into smaller groups based on the longest axis.
    \relates AT_TriGroup

    \param org_group Pointer to the original triangle group.
    \param left_group Pointer to the first empty group.
    \param right_group Pointer to the second empty group.

    \retval void
 */
AT_Result AT_trigroup_split(AT_TriGroup *org_group, AT_TriangleGroups *groups, uint32_t N);
#endif // AT_TRIGROUP

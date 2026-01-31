/** \file
    \brief AT_Triangle, AT_Vec3 and related functions
*/

#ifndef AT_MATH_H
#define AT_MATH_H

#include <math.h>

/** \brief Groups three floats to represent a vector of size 3.
 */
typedef struct {
    float x, y, z;
} AT_Vec3;

/** \brief Groups the min and max of a voxel for AABB collision detection.
 */
typedef struct {
    AT_Vec3 min;
    AT_Vec3 max;
    AT_Vec3 midpoint;
} AT_AABB;

/** \brief Groups three AT_Vec3 to represent a triangle.
 */
typedef struct {
    AT_Vec3 v1, v2, v3;
    AT_AABB aabb;
} AT_Triangle;

/** \brief AT_Vec3 constructor for a given point.
    \relates AT_Vec3

    \retval AT_Vec3 Vector with coordinate values of those given in function call.
*/
static inline AT_Vec3 AT_vec3(float x, float y, float z)
{
    return (AT_Vec3){x, y, z};
}

/** \brief AT_Vec3 constructor for a zero initialised vector.
    \relates AT_Vec3

    \retval AT_Vec3 Vector with all values initialised at 0.
*/
static inline AT_Vec3 AT_vec3_zero(void)
{
    return (AT_Vec3){0.0f, 0.0f, 0.0f};
}

/** \brief Adds two AT_Vec3.
    \relates AT_Vec3

    \retval AT_Vec3 The result of the vector addition.
*/
static inline AT_Vec3 AT_vec3_add(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

/** \brief Subtracts two AT_Vec3.
    \relates AT_Vec3

    \retval AT_Vec3 The result of the vector subtraction.
*/
static inline AT_Vec3 AT_vec3_sub(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

/** \brief Scales an AT_Vec3 by a given scalar.
    \relates AT_Vec3

    \param v The AT_Vec3 to be scaled.
    \param s The scalar multiple.

    \retval AT_Vec3 The result of the scaling the vector by \a s.
*/
static inline AT_Vec3 AT_vec3_scale(AT_Vec3 v, float s)
{
    return (AT_Vec3){v.x * s, v.y * s, v.z * s};
}

/** \brief Performs vector dot operation on two given AT_Vec3.
    \relates AT_Vec3

    \retval float The result of the vector dot operation.
*/
static inline float AT_vec3_dot(AT_Vec3 a, AT_Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/** \brief Performs vector cross multiplication on two given AT_Vec3.
    \relates AT_Vec3

    \retval AT_Vec3 The result of the vector cross multiplication.
*/
static inline AT_Vec3 AT_vec3_cross(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

/** \brief Calculate the length of an AT_Vec3.
    \relates AT_Vec3

    \retval float The length of the given AT_Vec3.
*/
static inline float AT_vec3_length(AT_Vec3 v)
{
    return sqrtf(AT_vec3_dot(v, v));
}

/** \brief
    \relates AT_Vec3

    \retval AT_Vec3 A normalized AT_Vec3.
*/
static inline AT_Vec3 AT_vec3_normalize(AT_Vec3 v)
{
    float len = AT_vec3_length(v);
    return (len > 0.0f) ? AT_vec3_scale(v, 1.0f / len) : AT_vec3_zero();
}

/** \brief Calculates the distance between two AT_Vec3's.
    \relates AT_Vec3

    \retval float The distance between two AT_Vec3's.
 */
static inline float AT_vec3_distance(AT_Vec3 a, AT_Vec3 b)
{
    return sqrt((b.x - a.x) * (b.x - a.x) +
                (b.y - a.y) * (b.y - a.y) +
                (b.z - a.z) * (b.z - a.z));
}

/** \brief Calculates the squared distance between two AT_Vec3's.
    \relates AT_Vec3

    \retval float The squared distance between two AT_Vec3's.
 */
static inline float AT_vec3_distance_sq(AT_Vec3 a, AT_Vec3 b)
{
    return (b.x - a.x) * (b.x - a.x) +
           (b.y - a.y) * (b.y - a.y) +
           (b.z - a.z) * (b.z - a.z);
}
#endif // AT_MATH_H

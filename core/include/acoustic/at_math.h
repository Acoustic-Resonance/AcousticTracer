/** \file
    \brief AT_Triangle, AT_Vec3 and related functions
    \ingroup math
*/

#ifndef AT_MATH_H
#define AT_MATH_H

#define EPSILON 1e-6f

#include <math.h>
#include <float.h>
#include <stdbool.h>

/** \defgroup math Maths */
/** \addtogroup vec Vector Math
    \ingroup math
 */
/** \addtogroup tri Triangles
    \ingroup model math
 */
/** \addtogroup aabb AABB
    \ingroup tri
 */

/** \brief Groups three floats to represent a vector of size 3.
    \ingroup vec
 */
typedef union {
    struct {
        float x, y, z;
    }; /**< Struct representation of a 3D vector */
    float arr[3]; /**< Array representation to enable iteration. */
} AT_Vec3;

/** \brief Groups three integers to represent a vector of size 3.
    \ingroup vec
 */
typedef struct {
    int x; /**< Vector's `x` coords. */
    int y; /**< Vector's `y` coords. */
    int z; /**< Vector's `z` coords. */
} AT_Vec3i;

/** \brief Axis-Aligned Bounding Box for triangle(s).
    \ingroup aabb
 */
typedef struct {
    AT_Vec3 min; /**< Minimum `x`, `y`, and `z` in one vector. */
    AT_Vec3 max; /**< Maximum `x`, `y`, and `z` in one vector. */
    AT_Vec3 midpoint;
    float SA;
} AT_AABB;

/** \brief Groups three AT_Vec3 to represent a triangle.
    \ingroup tri
 */
typedef struct {
    AT_Vec3 v1;   /**< Triangle's first vertex. */
    AT_Vec3 v2;   /**< Triangle's second vertex. */
    AT_Vec3 v3;   /**< Triangle's third vertex. */
    AT_AABB aabb; /**< Triangle's Axis-Aligned Bounding Box */
    bool left;
} AT_Triangle;

/** \brief AT_Vec3 constructor for a given point.
    \relates AT_Vec3
    \ingroup vec

    \param x `x` coord in world coordinates.
    \param y `y` coord in world coordinates.
    \param z `z` coord in world coordinates.

    \retval AT_Vec3 Vector with coordinate values of those given in function call.
*/
static inline AT_Vec3 AT_vec3(float x, float y, float z)
{
    return (AT_Vec3){{x, y, z}};
}

/** \brief AT_Vec3 constructor for a zero initialised vector.
    \relates AT_Vec3
    \ingroup vec

    \retval AT_Vec3 Vector with all values initialised at 0.
*/
static inline AT_Vec3 AT_vec3_zero(void)
{
    return (AT_Vec3){{0.0f, 0.0f, 0.0f}};
}

/** \brief Adds two AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param a First vector in the addition.
    \param b Second vector in the addition.

    \retval AT_Vec3 The result of the vector addition.
*/
static inline AT_Vec3 AT_vec3_add(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){{a.x + b.x, a.y + b.y, a.z + b.z}};
}

/** \brief Subtracts two AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param a First vector in the subtraction.
    \param b Second vector in the subtraction.

    \retval AT_Vec3 The result of the vector subtraction.
*/
static inline AT_Vec3 AT_vec3_sub(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){{a.x - b.x, a.y - b.y, a.z - b.z}};
}

/** \brief Product of two AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param a First vector in the multiplication.
    \param b Second vector in the multiplication.

    \retval AT_Vec3 The result of the vector multiplication.
*/
static inline AT_Vec3 AT_vec3_mul(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){{a.x * b.x, a.y * b.y, a.z * b.z}};
}

/** \brief Calculates the inverse of a vector.
    \relates AT_Vec3
    \ingroup vec

    \param v Vector to be inverted.

    \retval AT_Vec3 The inverse of the input vector.
*/
static inline AT_Vec3 AT_vec3_inv(AT_Vec3 v)
{
    return (AT_Vec3){
        {(fabsf(v.x) < EPSILON ? FLT_MAX : (1.0f / fabsf(v.x))),
         (fabsf(v.y) < EPSILON ? FLT_MAX : (1.0f / fabsf(v.y))),
         (fabsf(v.z) < EPSILON ? FLT_MAX : (1.0f / fabsf(v.z)))}
    };
}

/** \brief Scales an AT_Vec3 by a given scalar.
    \relates AT_Vec3
    \ingroup vec

    \param v The AT_Vec3 to be scaled.
    \param s The scalar multiple.

    \retval AT_Vec3 The result of the scaling the vector by \a s.
*/
static inline AT_Vec3 AT_vec3_scale(AT_Vec3 v, float s)
{
    return (AT_Vec3){{v.x * s, v.y * s, v.z * s}};
}

/** \brief Performs vector dot operation on two given AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param a First vector in the dot operation.
    \param b Second vector in the dot operation.

    \retval float The result of the vector dot operation.
*/
static inline float AT_vec3_dot(AT_Vec3 a, AT_Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/** \brief Performs vector cross multiplication on two given AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param a First vector in the cross multiplication.
    \param b Second vector in the cross multiplication.

    \retval AT_Vec3 The result of the vector cross multiplication.
*/
static inline AT_Vec3 AT_vec3_cross(AT_Vec3 a, AT_Vec3 b)
{
    return (AT_Vec3){
        {a.y * b.z - a.z * b.y,
         a.z * b.x - a.x * b.z,
         a.x * b.y - a.y * b.x}
    };
}

/** \brief Calculate the length of an AT_Vec3.
    \relates AT_Vec3
    \ingroup vec

    \param v Vector whose length is to be returned.

    \retval float The length of the given AT_Vec3.
*/
static inline float AT_vec3_length(AT_Vec3 v)
{
    return sqrtf(AT_vec3_dot(v, v));
}

/** \brief Normalise a given vector to be within 0.0 -> 1.0.
    \relates AT_Vec3
    \ingroup vec

    \param v Vector to be normalised.

    \retval AT_Vec3 A normalised AT_Vec3.
*/
static inline AT_Vec3 AT_vec3_normalize(AT_Vec3 v)
{
    float len = AT_vec3_length(v);
    return (len > 0.0f) ? AT_vec3_scale(v, 1.0f / len) : AT_vec3_zero();
}

/** \brief Calculates the distance between two AT_Vec3's.
    \relates AT_Vec3
    \ingroup vec

    \param a First point on the distance line.
    \param b Second point on the distance line.

    \retval float The distance between two AT_Vec3's.
 */
static inline float AT_vec3_distance(AT_Vec3 a, AT_Vec3 b)
{
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z));
}

/** \brief Calculates the squared distance between two AT_Vec3's.
    \relates AT_Vec3
    \ingroup vec

    \param a First point on the distance line.
    \param b Second point on the distance line.

    \retval float The squared distance between two AT_Vec3's.
 */
static inline float AT_vec3_distance_sq(AT_Vec3 a, AT_Vec3 b)
{
    return (b.x - a.x) * (b.x - a.x) +
           (b.y - a.y) * (b.y - a.y) +
           (b.z - a.z) * (b.z - a.z);
}

/* \brief Computes per axis step distance used in DDA.
   \relates AT_Vec3
   \ingroup vec

   \param v Vector being traversed.

  \retval AT_Vec3 Per axis step distance.
 */
static inline AT_Vec3 AT_vec3_delta(AT_Vec3 v)
{
    return (AT_Vec3){
        {v.x != 0.0f ? fabsf(1.0f / v.x) : FLT_MAX,
         v.y != 0.0f ? fabsf(1.0f / v.y) : FLT_MAX,
         v.z != 0.0f ? fabsf(1.0f / v.z) : FLT_MAX}
    };
}

#endif // AT_MATH_H

#ifndef AT_MATH_H
#define AT_MATH_H

#include <math.h>

typedef struct {
    float x, y, z;
} AT_Vec3;

typedef struct {
    AT_Vec3 v1, v2, v3;
} AT_Triangle;

static inline AT_Vec3 AT_vec3(float x, float y, float z) {
    return (AT_Vec3){ x, y, z };
}

static inline AT_Vec3 AT_vec3_zero(void) {
    return (AT_Vec3){ 0.0f, 0.0f, 0.0f };
}

static inline AT_Vec3 AT_vec3_add(AT_Vec3 a, AT_Vec3 b) {
    return (AT_Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline AT_Vec3 AT_vec3_sub(AT_Vec3 a, AT_Vec3 b) {
    return (AT_Vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

static inline AT_Vec3 AT_vec3_scale(AT_Vec3 v, float s) {
    return (AT_Vec3){ v.x * s, v.y * s, v.z * s };
}

static inline float AT_vec3_dot(AT_Vec3 a, AT_Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline AT_Vec3 AT_vec3_cross(AT_Vec3 a, AT_Vec3 b) {
    return (AT_Vec3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

static inline float AT_vec3_length(AT_Vec3 v) {
    return sqrtf(AT_vec3_dot(v, v));
}

static inline AT_Vec3 AT_vec3_normalize(AT_Vec3 v) {
    float len = AT_vec3_length(v);
    return (len > 0.0f) ? AT_vec3_scale(v, 1.0f / len) : AT_vec3_zero();
}

#endif // AT_MATH_H
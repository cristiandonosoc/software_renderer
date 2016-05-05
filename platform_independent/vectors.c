#ifndef PI_VECTORS_C
#define PI_VECTORS_C

#include "vectors.h"

vec3i CrossProducti(vec3i a, vec3i b)
{
    vec3i result = { (a.y * b.z) - (a.z * b.y),
                     (a.z * b.x) - (a.x * b.z),
                     (a.x * b.y) - (a.y * b.x) };
    return result;
}

vec3d CrossProductd(vec3d a, vec3d b)
{
    vec3d result = { (a.y * b.z) - (a.z * b.y),
                     (a.z * b.x) - (a.x * b.z),
                     (a.x * b.y) - (a.y * b.x) };
    return result;
}

#endif

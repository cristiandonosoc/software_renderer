#ifndef PI_VECTORS_C
#define PI_VECTORS_C

#include "vectors.h"

#include <math.h>

int DotProducti(vec3i a, vec3i b)
{
    int dot = ((a.x * b.x) +
               (a.y * b.y) +
               (a.z * b.z));
    return dot;
}

double DotProductd(vec3d a, vec3d b)
{
    double dot = ((a.x * b.x) +
                  (a.y * b.y) +
                  (a.z * b.z));
    return dot;
}

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

void NormalizeInPlace(vec3d *v)
{
    double length = sqrt((v->x * v->x) + 
                         (v->y * v->y) +
                         (v->z * v->z));
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

vec3d Normalize(vec3d v)
{
    vec3d copy = v;
    NormalizeInPlace(&copy);
    return copy;
}

vec3d Vec3dSubstract(vec3d a, vec3d b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

#endif

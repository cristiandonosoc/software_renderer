#ifndef PI_OBJ_H
#define PI_OBJ_H

#include "graphics.h"

typedef struct _vertex3d
{
    vec3d position;
    vec3d normal;
    vec3d textureCoord;
} vertex3d;

typedef struct _face
{
    int i1;
    int i2;
    int i3;
    vertex3d *v1;
    vertex3d *v2;
    vertex3d *v3;
} face;

typedef struct _obj_model
{
    vertex3d *vertices;
    int vertexCount;
    face *faces;
    int faceCount;
} obj_model;

#endif

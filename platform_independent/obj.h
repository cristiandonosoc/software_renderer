#ifndef PI_OBJ_H
#define PI_OBJ_H

typedef struct _vertex
{
    double x;
    double y;
    double z;
} vertex;

typedef struct _face
{
    int i1;
    int i2;
    int i3;
    vertex *v1;
    vertex *v2;
    vertex *v3;
} face;

typedef struct _obj_model
{
    vertex *vertices;
    int vertexCount;
    face *faces;
    int facesCount;
} obj_model;

#endif

#ifndef PI_OBJ_C
#define PI_OBJ_C

#include "macros.h"
#include "obj.h"
#include "obj/parsing.c"

void AllocObj(obj_model *model)
{
    // We allocate the vertex/faces
    // TODO: Only allocate the amount needed
    model->vertices = (vertex3d *)malloc(MAX_VERTEX * sizeof(vertex3d));
    model->faces = (face *)malloc(MAX_FACES * sizeof(face));
    model->vertexCount = 0;
    model->faceCount = 0;
}

void FreeObj(obj_model *model)
{
    free(model->vertices);
    free(model->faces);
}

int LoadObj(char *filename, obj_model *model)
{
    AllocObj(model);

    if (!ParseObj(filename, model))
    {
        FreeObj(model);
        return 0;
    }

    return 1;
}

#endif

#include "platform_independent/colors.h"

#include "platform_independent/graphics.c"
#include "platform_independent/obj.c"
#include "platform_independent/utilities.c"

/**
 * Task 1: 2D rendering of a model
 */
void DrawObj(graphics_buffer *buffer, char *modelPath)
{
    obj_model model;
    if(!LoadObj(modelPath, &model))
    {
        return;
    }

    // We draw the model
    for (int i = 0; i < model.facesCount; ++i)
    {
        // We draw the vertices
        face f = model.faces[i];

        DrawVertices(f.v1, f.v2, buffer);
        DrawVertices(f.v2, f.v3, buffer);
        DrawVertices(f.v3, f.v1, buffer);

        usleep(10 * 1000);
    }
}

/**
 * Task 2: Drawing (filled) triangles
 */
void Triangles(graphics_buffer *buffer)
{
    vec2i t1[3] = { {10, 70}, {50, 160}, {70, 80} };
    vec2i t2[3] = { {180, 50}, {150, 1}, {70, 180} };
    vec2i t3[3] = { {180, 150}, {120, 160}, {130, 180} };
    DrawTriangle(t1, buffer, RED);
    DrawTriangle(t2, buffer, WHITE);
    DrawTriangle(t3, buffer, GREEN);
    return;
}

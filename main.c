#include "platform_independent/graphics.c"
#include "platform_independent/obj.c"
#include "platform_independent/utilities.c"

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

void Triangles(graphics_buffer *buffer)
{
    vec2i v0 = { 100, 100 };
    vec2i v1 = { 100, 300 };
    vec2i v2 = { 300, 300 };
    DrawTriangle(v0, v1, v2, buffer, 0xFFFFFFFF);
    return;
}

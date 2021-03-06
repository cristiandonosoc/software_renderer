#include "platform_independent/colors.h"

#include "platform_independent/graphics.c"
#include "platform_independent/obj.c"
#include "platform_independent/utilities.c"
#include "platform_independent/texture.c"

#include <stdlib.h>

#define DELAY 10

/**
 * Task 1: 2D rendering of a model
 */
void DrawObj(graphics_buffer *buffer, char *modelPath, draw_control *drawControl)
{
    obj_model model;
    if(!LoadObj(modelPath, &model))
    {
        return;
    }

    int index = 0;
    while (1)
    {
        if (drawControl->drawNext)
        {
            drawControl->drawNext = 0;

            if (index >= model.faceCount) { continue; }

            // We draw the vertices
            face f = model.faces[index];

            DrawVertices(f.v1, f.v2, buffer);
            DrawVertices(f.v2, f.v3, buffer);
            DrawVertices(f.v3, f.v1, buffer);

            ++index;
        }

        usleep(DELAY * 1000);
    }

    FreeObj(&model);
}

/**
 * Task 2: Drawing (filled) triangles
 */
void Triangles(graphics_buffer *buffer, char *modelPath)
{
    obj_model model;
    if(!LoadObj(modelPath, &model))
    {
        return;
    }

    vec3d lightDir = { 0.0, 0.0, -1.0 };
    NormalizeInPlace(&lightDir);

    // We draw the model
    for (int i = 0; i < model.faceCount; ++i)
    {
        // We draw the vertices
        face f = model.faces[i];

        vec3d cross = CrossProductd(Vec3dSubstract(f.v3->position, f.v1->position),
                                    Vec3dSubstract(f.v2->position, f.v1->position));
        NormalizeInPlace(&cross);

        double intensity = DotProductd(cross, lightDir);
        if (intensity > 0.0)
        {
            int color = 0xFF000000 |
                        ((int)(intensity * 0xFF) << 16) |
                        ((int)(intensity * 0xFF) << 8) |
                        ((int)(intensity * 0xFF));
            vec2i vertices[3] = { GetVec2iFromVertex3d(f.v1, buffer),
                                  GetVec2iFromVertex3d(f.v2, buffer),
                                  GetVec2iFromVertex3d(f.v3, buffer) };
            DrawTriangle(vertices, buffer, color);

            usleep(DELAY * 1000);
        }
    }

    FreeObj(&model);
}

/**
 * Task 3: Drawing with Z buffer
 */
void TrianglesWithZBuffer(graphics_buffer *buffer, char *modelPath)
{
    obj_model model;
    if(!LoadObj(modelPath, &model)) { return; }

    vec3d lightDir = { 0.0, 0.0, -1.0 };
    NormalizeInPlace(&lightDir);

    // We draw the model
    for (int i = 0; i < model.faceCount; ++i)
    {
        // We draw the vertices
        face f = model.faces[i];

        vec3d cross = CrossProductd(Vec3dSubstract(f.v3->position, f.v1->position),
                                    Vec3dSubstract(f.v2->position, f.v1->position));
        NormalizeInPlace(&cross);

        double intensity = DotProductd(cross, lightDir);
        if (intensity > 0.0)
        {
            int color = 0xFF000000 |
                        ((int)(intensity * 0xFF) << 16) |
                        ((int)(intensity * 0xFF) << 8) |
                        ((int)(intensity * 0xFF));
            DrawTriangleFromFace(f, buffer, color);

            usleep(DELAY * 1000);
        }
    }

    FreeObj(&model);
}

/**
 * Task 4: Drawing with texture
 */
void TrianglesWithTexture(graphics_buffer *buffer, char *modelPath)
{
    obj_model model;
    if (!LoadObj(modelPath, &model)) { return; }
    
    // We load the texture
    int length = strlen(modelPath) + 1;
    char *texPath = (char *)malloc(length);
    char *c = modelPath;
    for(int i = 0; i < length; ++i)
    {
        texPath[i] = *c++; 
    }
    texPath[length - 4] = 't';
    texPath[length - 3] = 'g';
    texPath[length - 2] = 'a';

    texture tex;
    if (!LoadTexture(&tex, texPath))
    {
        FreeObj(&model);
        return;
    }

    vec3d lightDir = { 0.0, 0.0, -1.0 };
    NormalizeInPlace(&lightDir);

    // We draw the model
    for (int i = 0; i < model.faceCount; ++i)
    {
        // We draw the vertices
        face f = model.faces[i];

        vec3d cross = CrossProductd(Vec3dSubstract(f.v3->position, f.v1->position),
                                    Vec3dSubstract(f.v2->position, f.v1->position));
        NormalizeInPlace(&cross);

        float intensity = DotProductd(cross, lightDir);
        if (intensity > 0.0)
        {
            DrawTriangleFromFaceWithTexture(f, buffer, &tex, intensity);

            usleep(DELAY * 1000);
        }
    }


    FreeTexture(&tex);
    FreeObj(&model);
}

void TrianglesWithProjection(graphics_buffer *buffer, char *modelPath)
{
    obj_model model;
    if (!LoadObj(modelPath, &model)) { return; }

    // We load the texture
    int length = strlen(modelPath) + 1;
    char *texPath = (char *)malloc(length);
    char *c = modelPath;
    for(int i = 0; i < length; ++i)
    {
        texPath[i] = *c++; 
    }
    texPath[length - 4] = 't';
    texPath[length - 3] = 'g';
    texPath[length - 2] = 'a';

    texture tex;
    if (!LoadTexture(&tex, texPath))
    {
        FreeObj(&model);
        return;
    }

    vec3d lightDir = { 0.0, 0.0, -1.0 };
    NormalizeInPlace(&lightDir);

    // We draw the model
    for (int i = 0; i < model.faceCount; ++i)
    {
        // We draw the vertices
        face f = model.faces[i];

        vec3d cross = CrossProductd(Vec3dSubstract(f.v3->position, f.v1->position),
                                    Vec3dSubstract(f.v2->position, f.v1->position));
        NormalizeInPlace(&cross);

        float intensity = DotProductd(cross, lightDir);
        if (intensity > 0.0)
        {
            DrawTriangleFromFaceWithTexture(f, buffer, &tex, intensity);

            usleep(DELAY * 1000);
        }
    }


    FreeTexture(&tex);
    FreeObj(&model);
}

void SelectTask(program_info *programInfo)
{
    switch (programInfo->task)
    {
        case 1:
            DrawObj(programInfo->buffer, programInfo->modelPath, &programInfo->drawControl);
            break;
        case 2:
            Triangles(programInfo->buffer, programInfo->modelPath);
            break;
        case 3:
            TrianglesWithZBuffer(programInfo->buffer, programInfo->modelPath);
            break;
        case 4:
            TrianglesWithTexture(programInfo->buffer, programInfo->modelPath);
            break;
        case 5:
            TrianglesWithProjection(programInfo->buffer, programInfo->modelPath);
            break;
        default:
            fprintf(stderr, "Unknown task\n");
            PrintTasks(stderr);
            programInfo->running = 0;
    }
}



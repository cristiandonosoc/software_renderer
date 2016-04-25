#include "macros.h"
#include "obj.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define REMOVE_SPACES(c, stream) while((c = fgetc(stream)) == ' ') {};
#define GET_WORD(c, stream, buffer, index, delim) REMOVE_SPACES(c, stream) \
                                                  ungetc(c, stream); \
                                                  index = 0; \
                                                  do \
                                                  { \
                                                      c = fgetc(stream); \
                                                      if ((c == delim) || (c == '\n') || (c == EOF)) { break; }\
                                                      buffer[index++] = (char)c; \
                                                  } \
                                                  while (1); \
                                                  buffer[index++] = '\0';
#define SEEK_CHAR(c, stream, seekc) while((c != seekc) && (c != EOF)) { c = fgetc(stream); }

// For now we allocate a shitload of vertex
#define MAX_VERTEX 10000
static vertex vertices[MAX_VERTEX];
#define MAX_FACES 10000
static face faces[MAX_FACES];

obj_model LoadObj(char *filename)
{
    FILE *objFile;
    int error = fopen_s(&objFile, filename, "r");
    if (error != 0)
    {
        char string [256];
        strerror_s(string, sizeof(string), errno);
        OutputDebugString(string);
    }

    // We allocate the vertex/faces
    vertex *vertices = (vertex *)malloc(MAX_VERTEX * sizeof(vertex));
    face *faces = (face *)malloc(MAX_FACES * sizeof(face));
    int vertexCount = 0;
    int faceCount = 0;

    char buffer[256];
    int index = 0;
    char c = ' ';
    while (c != EOF)
    {
        // We get the first character of the line
        GET_WORD(c, objFile, buffer, index, ' ')

        // We get the first character
        if (strcmp(buffer, "v") == 0)
        {
            GET_WORD(c, objFile, buffer, index, ' ');
            vertices[vertexCount].x = atof(buffer);
            GET_WORD(c, objFile, buffer, index, ' ');
            vertices[vertexCount].y = atof(buffer);
            GET_WORD(c, objFile, buffer, index, ' ');
            vertices[vertexCount].z = atof(buffer);
            ++vertexCount;
        }
        // For now the format requires the whole f <int>/<int>/<int> ... format
        else if(strcmp(buffer, "f") == 0)
        {
            // absolutely no error checking :D
            int index;
            GET_WORD(c, objFile, buffer, index, '/')
            faces[faceCount].v1 = &vertices[atoi(buffer)];
            SEEK_CHAR(c, objFile, ' ')

            GET_WORD(c, objFile, buffer, index, '/')
            faces[faceCount].v2 = &vertices[atoi(buffer)];
            SEEK_CHAR(c, objFile, ' ')

            GET_WORD(c, objFile, buffer, index, '/')
            faces[faceCount].v3 = &vertices[atoi(buffer)];
            SEEK_CHAR(c, objFile, ' ')

            ++faceCount;
        }

        SEEK_CHAR(c, objFile, '\n');
    }

    fclose(objFile);

    obj_model model;
    model.vertices = vertices;
    model.vertexCount = vertexCount;
    model.faces = faces;
    model.facesCount = faceCount;

    return model;
}


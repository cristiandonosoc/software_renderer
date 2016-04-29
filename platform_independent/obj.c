#include "macros.h"
#include "obj.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int LoadObj(char *filename, obj_model *model)
{
    FILE *objFile = fopen(filename, "r");
    if (objFile == 0) 
    {
        fprintf(stderr, "Model %s not found in file %s line %d\n", filename, __FILE__, __LINE__ - 3);
        return 0;
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
            int temp, index;
            GET_WORD(c, objFile, buffer, temp, '/')
            index = atoi(buffer) - 1;
            faces[faceCount].i1 = index;
            faces[faceCount].v1 = &vertices[index];
            SEEK_CHAR(c, objFile, ' ')

            GET_WORD(c, objFile, buffer, temp, '/')
            index = atoi(buffer) - 1;
            faces[faceCount].i2 = index;
            faces[faceCount].v2 = &vertices[index];
            SEEK_CHAR(c, objFile, ' ')

            GET_WORD(c, objFile, buffer, temp, '/')
            index = atoi(buffer) - 1;
            faces[faceCount].i3 = index;
            faces[faceCount].v3 = &vertices[index];

            ++faceCount;
        }

        SEEK_CHAR(c, objFile, '\n');
    }

    fclose(objFile);

    model->vertices = vertices;
    model->vertexCount = vertexCount;
    model->faces = faces;
    model->facesCount = faceCount;

    return 1;
}



#ifndef PI_OBJ_C
#define PI_OBJ_C

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
#define MAX_FACES 10000

typedef enum _token_kind 
{
    TOKEN_VERTEX,
    TOKEN_TEXTURE_COORD,
    TOKEN_NORMAL,
    TOKEN_FACE,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_SLASH,
    TOKEN_NEW_LINE,
    TOKEN_END_OF_FILE,
    TOKEN_UNSUPPORTED
} token_kind;

typedef enum _parse_state
{
    PARSE_STATE_NEW_STMT,
    PARSE_STATE_VERTEX_DEF,
    PARSE_STATE_TEX_COORD_DEF,
    PARSE_STATE_NORMAL_DEF,
    PARSE_STATE_FACE_DEF
} parse_state;

typedef struct _token
{
    token_kind kind;
    union {
        int int_value;
        float float_value;
    };
} token;

typedef struct _parsing_status
{
    parse_state state;
    int slashCount;
    int numCount;
    int parsing;
} parsing_status;

char *GetNextToken(char *parsePoint, token *t)
{
    // BAD PRACTICE: Not checking overflow :)
    static char localBuffer[100];
    char c;
    // We ignore whitespace
    while ((c = *parsePoint) == ' ') { ++parsePoint; }

    // We ignore comments
    if (c == '#')
    {
        ++parsePoint;
        while(((c = *parsePoint) != '\n') ||
              ((c = *parsePoint) != EOF))
        {
            ++parsePoint;
        }

        if (c == '\n') { ++parsePoint; }
    }

    if (c == '\n') { t->kind = TOKEN_NEW_LINE; ++parsePoint; }
    else if (c == EOF) { t->kind = TOKEN_END_OF_FILE; }
    else if (c == '/') { t->kind = TOKEN_SLASH; ++parsePoint; }
    else if (c == 'f') { t->kind = TOKEN_FACE; ++parsePoint; }
    else if (c == 'v')
    {
        char next = *(parsePoint + 1);
        if (next == ' ') { t->kind = TOKEN_VERTEX; ++parsePoint;}
        else if (next == 't') { t->kind = TOKEN_TEXTURE_COORD; parsePoint += 2; }
        else if (next == 'n') { t->kind = TOKEN_NORMAL; parsePoint += 2; }
        else { t->kind = TOKEN_UNSUPPORTED; }
    }
    else if(('0' <= c) && (c <= '9'))
    {
        int dot = 0;
        int index = 0;

        // We start accumulating buffer
        while((('0' <= c) && (c <= '9')) || (c == '.'))
        {
            if (c == '.') { dot = 1; }
            localBuffer[index++] = c;
            c = *++parsePoint;
        }
        localBuffer[index++] = 0;

        // We parse the value
        if (dot == 0)
        {
            t->int_value = atoi(localBuffer);
            t->kind = TOKEN_INT;
        }
        else
        {
            t->float_value = atof(localBuffer);
            t->kind = TOKEN_FLOAT;
        }

        ++parsePoint;
    }
    else { t->kind = TOKEN_UNSUPPORTED; }

    return parsePoint;
}

int LoadObj(char *filename, obj_model *model)
{
    FILE *objFile = fopen(filename, "r");
    if (objFile == 0) 
    {
        fprintf(stderr, "Model %s not found in file %s line %d\n", filename, __FILE__, __LINE__ - 3);
        return 0;
    }

    // We allocate the vertex/faces
    vertex3d *vertices = (vertex3d *)malloc(MAX_VERTEX * sizeof(vertex3d));
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
            vertices[vertexCount].color = 0xFFFFFFFF;
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
    model->faceCount = faceCount;

    return 1;
}

void ParseVector(obj_model *model, parsing_status *parseStatus, token *t)
{
    switch (t->kind)
    {
        case TOKEN_FLOAT:
            if (parseStatus->numCount == 0) { model->vertices[model->vertexCount].x = t->float_value; }
            else if (parseStatus->numCount == 1) { model->vertices[model->vertexCount].y = t->float_value; }
            else if (parseStatus->numCount == 2) { model->vertices[model->vertexCount].z = t->float_value; }
            else
            {
                fprintf(stderr, "ERROR PARSING");
                parseStatus->parsing = 0;
                break;
            }
            ++parseStatus->numCount;
            break;
        case TOKEN_INT:
            if (parseStatus->numCount == 0) { model->vertices[model->vertexCount].x = t->int_value; }
            else if (parseStatus->numCount == 1) { model->vertices[model->vertexCount].y = t->int_value; }
            else if (parseStatus->numCount == 2) { model->vertices[model->vertexCount].z = t->int_value; }
            else
            {
                fprintf(stderr, "ERROR PARSING");
                parseStatus->parsing = 0;
                break;
            }
            ++(parseStatus->numCount);
            break;
        case TOKEN_NEW_LINE:
            if (parseStatus->numCount != 3)
            {
                fprintf(stderr, "ERROR PARSING");
                parseStatus->parsing = 0;
                return;
            }
            ++(model->vertexCount);
            parseStatus->state = PARSE_STATE_NEW_STMT;
            parseStatus->numCount = 0;
            break;
        default:
            fprintf(stderr, "ERROR PARSING");
            parseStatus->parsing = 0;
            return;
    }
}


int ParseObj(char *filename, obj_model *model)
{
    FILE *objFile = fopen(filename, "r");
    if (objFile == 0) 
    {
        fprintf(stderr, "Model %s not found in file %s line %d\n", filename, __FILE__, __LINE__ - 3);
        return 0;
    }

    token t;

    // We read the file into memory
    int bufferSize = 1 << 20; // 1 MB
    char *text = (char *)malloc(bufferSize);
    int len = fread(text, 1, bufferSize, objFile);
    char *limit;
    if (len == bufferSize)
    {
        // We need more file
        limit = text + bufferSize;
        ASSERT(0);
    }

    parsing_status parseStatus = { PARSE_STATE_NEW_STMT, 0, 0, 1 };


    // We allocate the vertex/faces
    // TODO: Only allocate the amount needed
    model->vertices = (vertex3d *)malloc(MAX_VERTEX * sizeof(vertex3d));
    model->faces = (face *)malloc(MAX_FACES * sizeof(face));
    model->vertexCount = 0;
    model->faceCount = 0;


    //char *parsePoint = GetNextToken(text, &t);
    while ((parseStatus.parsing != 0) &&
           (t.kind != TOKEN_END_OF_FILE))
    {
        switch (parseStatus.state)
        {
            case PARSE_STATE_NEW_STMT:
                switch (t.kind)
                {
                    case TOKEN_VERTEX:
                        parseStatus.state = PARSE_STATE_VERTEX_DEF;
                        break;
                    case TOKEN_TEXTURE_COORD:
                        parseStatus.state = PARSE_STATE_TEX_COORD_DEF;
                        break;
                    case TOKEN_NORMAL:
                        parseStatus.state = PARSE_STATE_NORMAL_DEF;
                        break;
                    case TOKEN_FACE:
                        parseStatus.state = PARSE_STATE_FACE_DEF;
                        break;
                    case TOKEN_NEW_LINE: break;
                    default:
                        fprintf(stderr, "ERROR PARSING");
                        parseStatus.parsing = 0;
                        break;
                }
                break;
            case PARSE_STATE_VERTEX_DEF: ParseVector(model, &parseStatus, &t); break;
            case PARSE_STATE_TEX_COORD_DEF: ParseVector(model, &parseStatus, &t); break;
            case PARSE_STATE_NORMAL_DEF: ParseVector(model, &parseStatus, &t); break;
            default:
                break;

        }

    }
    



    return 1;
}

#endif

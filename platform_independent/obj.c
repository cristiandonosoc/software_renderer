#ifndef PI_OBJ_C
#define PI_OBJ_C

#include "macros.h"
#include "obj.h"
#include "obj/parsing.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// For now we allocate a shitload of vertex
#define MAX_VERTEX 5000
#define MAX_FACES 5000

void GetNextToken(parsing_status *parseStatus)
{
    token *t = &parseStatus->currentToken;
    char *parsePoint = parseStatus->parsePoint;

    // BAD PRACTICE: Not checking overflow :)
    static char localBuffer[100];
    char c;
    // We ignore whitespace
    while ((c = *parsePoint) == ' ') { ++parsePoint; }

    // We ignore comments
    if (c == '#')
    {
        c = *(++parsePoint);
        // We pass this line
        while((c != '\n') && (c != EOF))
        {
            c = *(++parsePoint);
        }
        if (c == '\n')
        { 
            ++(parseStatus->lineNumber); 
            parseStatus->lineStart = parsePoint + 1;
        }
        c = *(++parsePoint);
    }
    // For now we ignore g/s
    else if ((c == 'g') || (c == 's'))
    {
        c = *(++parsePoint);
        // We pass this line
        while((c != '\n') && (c != EOF))
        {
            c = *(++parsePoint);
        }
        if (c == '\n')
        { 
            ++(parseStatus->lineNumber); 
            parseStatus->lineStart = parsePoint + 1;
        }
        c = *(++parsePoint);
    }
    else if (c == '\n') 
    { 
        t->kind = TOKEN_NEW_LINE; 
        ++parsePoint; 
        parseStatus->lineStart = parsePoint;
        ++(parseStatus->lineNumber);
    }
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
    else if((c == '-') || (c == '.') || (('0' <= c) && (c <= '9')))
    {
        int floatString = 0;
        int index = 0;

        localBuffer[index++] = c;
        c = *++parsePoint;

        // We start accumulating buffer
        while((c == '-') || (c == '.') || (c == 'e') ||
              (('0' <= c) && (c <= '9')))
        {
            if (c == '.') { floatString = 1; }
            localBuffer[index++] = c;
            c = *++parsePoint;
        }
        localBuffer[index++] = 0;

        // We parse the value
        if (floatString == 0)
        {
            t->int_value = atoi(localBuffer);
            t->kind = TOKEN_INT;
        }
        else
        {
            t->float_value = atof(localBuffer);
            t->kind = TOKEN_FLOAT;
        }
    }
    else { t->kind = TOKEN_UNSUPPORTED; }

    parseStatus->parsePoint = parsePoint;
}

void PrintParsingError(parsing_status *parseStatus, char *errorMessage)
{
    static char lineBuffer[100];
    // We copy the line
    char *cPtr = parseStatus->lineStart;
    char c = *cPtr++;
    int index = 0;
    while ((c != '\n') && (c != EOF))
    {
        lineBuffer[index++] = c;
        c = *cPtr++;
    }
    lineBuffer[index++] = '\0';

    int charNumber = (int)((parseStatus->parsePoint - parseStatus->lineStart) + 1);
    fprintf(stderr, "%s in Line %d, Char %d:\n", errorMessage,
                                                parseStatus->lineNumber,
                                                charNumber);
    fprintf(stderr, "%s\n", lineBuffer);
    for (int i = 0; i < charNumber; ++i)
    {
        lineBuffer[i] = ' ';
    }
    lineBuffer[charNumber - 1] = '^';
    lineBuffer[charNumber] = '\0';
    fprintf(stderr, "%s\n", lineBuffer);
}

int ParseVertex(parsing_status *parseStatus, obj_model *model)
{
    // for convenience
    token *t = &(parseStatus->currentToken);

    // X coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto vertex_error; }
    if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].position.x = t->float_value; }
    else { model->vertices[model->vertexCount].position.x = t->int_value; }

    // Y coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto vertex_error; }
    if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].position.y = t->float_value; }
    else { model->vertices[model->vertexCount].position.y = t->int_value; }

    // Z coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto vertex_error; }
    if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].position.z = t->float_value; }
    else { model->vertices[model->vertexCount].position.z = t->int_value; }

    // The line stops
    GetNextToken(parseStatus);
    if (t->kind != TOKEN_NEW_LINE) { goto vertex_error; }

    // We completed a vertex
    ++(model->vertexCount);
    return 1;

vertex_error:
    PrintParsingError(parseStatus, "Error Processing Vertex");
    return 0;
}

int ParseTextureCoordinates(parsing_status *parseStatus, obj_model *model)
{
    // for convenience
    token *t = &(parseStatus->currentToken);

    // X coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto tex_coord_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].textureCoord.x = t->float_value; } */
    /* else { model->vertices[model->vertexCount].textureCoord.x = t->int_value; } */

    // Y coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto tex_coord_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].textureCoord.y = t->float_value; } */
    /* else { model->vertices[model->vertexCount].textureCoord.y = t->int_value; } */

    // (OPTIONAL) coordinate
    GetNextToken(parseStatus);
    if (t->kind == TOKEN_NEW_LINE) 
    { 
        model->vertices[model->vertexCount].textureCoord.z = 0.0;  // Default
        return 1; 
    }
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto tex_coord_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].textureCoord.z = t->float_value; } */
    /* else { model->vertices[model->vertexCount].textureCoord.z = t->int_value; } */

    // The line stops
    GetNextToken(parseStatus);
    if (t->kind != TOKEN_NEW_LINE) { goto tex_coord_error; }

    return 1;

tex_coord_error:
    PrintParsingError(parseStatus, "Error Processing Texture Coordinate");
    return 0;
}

int ParseNormal(parsing_status *parseStatus, obj_model *model)
{
    // TODO:REMOVE
    model = 0;
    // for convenience
    token *t = &(parseStatus->currentToken);

    // X coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto normal_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].normal.x = t->float_value; } */
    /* else { model->vertices[model->vertexCount].normal.x = t->int_value; } */

    // Y coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto normal_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].normal.y = t->float_value; } */
    /* else { model->vertices[model->vertexCount].normal.y = t->int_value; } */

    // Z coordinate
    GetNextToken(parseStatus);
    if ((t->kind != TOKEN_FLOAT) && (t->kind != TOKEN_INT)) { goto normal_error; }
    /* if (t->kind == TOKEN_FLOAT) { model->vertices[model->vertexCount].normal.z = t->float_value; } */
    /* else { model->vertices[model->vertexCount].normal.z = t->int_value; } */

    // The line stops
    GetNextToken(parseStatus);
    if (t->kind != TOKEN_NEW_LINE) { goto normal_error; }

    return 1;

normal_error:
    PrintParsingError(parseStatus, "Error Processing Normal");
    return 0;
}


int ParseFaceStmt(parsing_status *parseStatus, obj_model *model)
{
    // TODO: REMOVE
    model = 0;

    // for convenience
    token *t = &(parseStatus->currentToken);

    // We see if we get texture coords
    GetNextToken(parseStatus);
    if (t->kind == TOKEN_SLASH)
    {
        GetNextToken(parseStatus);
        if (t->kind == TOKEN_INT)
        {
            // TODO: PROCESS TEXTURE COORDINATE
            
            // We see if we also get normals
            GetNextToken(parseStatus);
            if (t->kind == TOKEN_SLASH)
            {
                GetNextToken(parseStatus);
                if (t->kind != TOKEN_INT) { return 0; }

                // TODO: PROCESS NORMAL
                
                // The token is taken after
            }

            // We need to take the next token
            GetNextToken(parseStatus);
        }
        else if (t->kind == TOKEN_SLASH)
        {
            GetNextToken(parseStatus);
            if (t->kind != TOKEN_INT) { return 0; }
            
            // TODO: PROCESS NORMAL
            
            // We need to take the next token
            GetNextToken(parseStatus);
        }
    
    }
    else if (t->kind != TOKEN_INT) { return 0; }

    return 1;
}

int ParseFace(parsing_status *parseStatus, obj_model *model)
{
    // NOTE: ParseFaceStmt takes the next token after the value stmt
    // for convenience
    token *t = &(parseStatus->currentToken);
    int index;

    GetNextToken(parseStatus);
    if (parseStatus->currentToken.kind != TOKEN_INT) { goto face_error; }
    index = t->int_value - 1;
    model->faces[model->faceCount].i1 = index;
    model->faces[model->faceCount].v1 = &(model->vertices[index]);

    if(!ParseFaceStmt(parseStatus, model)) { goto face_error; }

    // We process the next batch
    index = t->int_value - 1;
    model->faces[model->faceCount].i2 = index;
    model->faces[model->faceCount].v2 = &(model->vertices[index]);

    if(!ParseFaceStmt(parseStatus, model)) { goto face_error; }

    // We process the next batch
    index = t->int_value - 1;
    model->faces[model->faceCount].i3 = index;
    model->faces[model->faceCount].v3 = &(model->vertices[index]);

    if(!ParseFaceStmt(parseStatus, model)) { goto face_error; }

    if (parseStatus->currentToken.kind != TOKEN_NEW_LINE) { goto face_error; }

    // We completed a face
    ++(model->faceCount);
    return 1;

face_error:
    PrintParsingError(parseStatus, "Error Processing Face");
    return 0;
}

int ParseObj(char *filename, obj_model *model)
{
    FILE *objFile = fopen(filename, "r");
    if (objFile == 0) 
    {
        fprintf(stderr, "Model %s not found in file %s line %d\n", filename, __FILE__, __LINE__ - 3);
        return 0;
    }

    // We read the file into memory
    int bufferSize = 1 << 20; // 1 MB
    char *text = (char *)malloc(bufferSize);
    int len = fread(text, 1, bufferSize, objFile);
    char *limit;
    if (len == bufferSize)
    {
        // We need more file
        limit = text + bufferSize;
        fclose(objFile);
        ASSERT(0);
    }
    else
    {
        text[len] = EOF;
        fclose(objFile);
    }

    // We allocate the vertex/faces
    // TODO: Only allocate the amount needed
    model->vertices = (vertex3d *)malloc(MAX_VERTEX * sizeof(vertex3d));
    model->faces = (face *)malloc(MAX_FACES * sizeof(face));
    model->vertexCount = 0;
    model->faceCount = 0;

    parsing_status parseStatus = { .currentToken = { TOKEN_UNSUPPORTED, { 0 } },
                                   .parsePoint = text,
                                   .lineStart = text,
                                   .lineNumber = 1,
                                   .charNumber = 1,
                                   .parsing = 1 };

    // We get the first token
    GetNextToken(&parseStatus);
    while ((parseStatus.parsing != 0) &&
           (parseStatus.currentToken.kind != TOKEN_END_OF_FILE))
    {
        switch (parseStatus.currentToken.kind)
        {
            case TOKEN_VERTEX:
                parseStatus.parsing = ParseVertex(&parseStatus, model);
                break;
            case TOKEN_TEXTURE_COORD:
                parseStatus.parsing = ParseTextureCoordinates(&parseStatus, model);
                break;
            case TOKEN_NORMAL:
                parseStatus.parsing = ParseNormal(&parseStatus, model);
                break;
            case TOKEN_FACE:
                parseStatus.parsing = ParseFace(&parseStatus, model);
                break;
            case TOKEN_NEW_LINE: break;
            default:
                PrintParsingError(&parseStatus, "Unsupported obj command");
                parseStatus.parsing = 0;
                break;
        }

        GetNextToken(&parseStatus);
    }

    return 1;
}

#endif

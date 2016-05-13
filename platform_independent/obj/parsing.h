#ifndef PI_OBJ_PARSING_H
#define PI_OBJ_PARSING_H

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
    token currentToken;
    char *parsePoint;
    char *lineStart;
    int lineNumber;
    int charNumber;
    int parsing;

} parsing_status;

#endif

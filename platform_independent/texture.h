#ifndef PI_TEXTURE_H
#define PI_TEXTURE_H

typedef struct _texture
{
    char *filename;
    int width;
    int height;
    int bytesPerPixel;
    int *data;
} texture;

#endif

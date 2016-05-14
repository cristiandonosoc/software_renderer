#ifndef PI_TEXTURE_C
#define PI_TEXTURE_C

#ifndef STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_TGA
#define STB_IMAGE_IMPLEMENTATION
#include "../external/std_image.h"
#endif

#include "texture.h"
#include "graphics.h"

int LoadTexture(texture *tex, char *filename)
{
    tex->filename = filename;
    int defaultBytesPerPixel;
    tex->data = (int *)stbi_load(filename, 
                                 &tex->width, &tex->height,
                                 &defaultBytesPerPixel,
                                 4);
    if (tex->data == NULL)
    {
        fprintf(stderr, "Texture %s couldn't be loaded", filename);
        return 0;
    }

    return 1;
}

void FreeTexture(texture *tex)
{
    stbi_image_free(tex->data);
}

int ObtainARGBFromABGR(int color)
{
    int result = ((color & 0xFF00FF00) |
                  ((color << 16) & 0x00FF0000) |
                  ((color >> 16) & 0x000000FF));
    return result;
}

void DrawTexture(graphics_buffer *buffer, texture *tex)
{
    // We draw the texture
    int *pixel = (int *)buffer->data;
    int *texPixel = (int *)tex->data;
    for (int y = 0; y < tex->height; ++y)
    {
        int tY = tex->height - y - 1;
        if (y >= buffer->height) { continue; }
        for (int x = 0; x < tex->width; ++x)
        {
            if (x >= buffer->width) { break; }
            // format is ABGR
            int color = texPixel[tex->width * tY + x];
            color = ObtainARGBFromABGR(color);
            // We need ARGB
            pixel[buffer->width * y + x] = color;
        }
    }
}


#endif

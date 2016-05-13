#ifndef PI_GRAPHICS_H
#define PI_GRAPHICS_H

#include "vectors.h"

typedef struct _graphics_buffer 
{
  int width;
  int height;
  int bytesPerPixel;
  int pitch;

  void *data;
  void *zBuffer;
} graphics_buffer;


typedef struct _box2i
{
    union
    {
        vec2i min;
        struct 
        {
            int minX;
            int minY;
        };
    };
    union
    {
        vec2i max;
        struct 
        {
            int maxX;
            int maxY;
        };
    };
} box2i;

#endif

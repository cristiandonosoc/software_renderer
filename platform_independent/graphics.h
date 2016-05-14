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
    vec2i min;
    vec2i max;
} box2i;

#endif

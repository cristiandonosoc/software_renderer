#ifndef PI_GRAPHICS_H
#define PI_GRAPHICS_H

typedef struct _graphics_buffer 
{
  int width;
  int height;
  int bytesPerPixel;
  int pitch;

  void *data;
} graphics_buffer;

#endif

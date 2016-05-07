#ifndef PI_GRAPHICS_UTILS_C
#define PI_GRAPHICS_UTILS_C

#include "../graphics.h"
#include "../vectors.h"

void ClearBuffer(graphics_buffer *buffer, int color)
{
  int *pixel = (int *)buffer->data;
  for (int y = 0; y < buffer->height; ++y)
  {
    for (int x = 0; x < buffer->width; ++x)
    {
      *pixel++ = color;
    }
  }
}

// Borders are INCLUSIVE
box2i GetTriangleBoundingBox2i(vec2i vertices[3], graphics_buffer *buffer)
{
    // Max impossible bounding box at start
    box2i box = { .minX = buffer->width - 1, 
                  .minY = buffer->height - 1,
                  .maxX = 0,
                  .maxY = 0 };

    for(int i = 0; i < 3; ++i)
    {
        int x = vertices[i].x;
        int y = vertices[i].y;
        if ((x < box.minX) && (x >= 0)) { box.minX = x; }
        if ((y < box.minY) && (y >= 0)) { box.minY = y; }
        if ((x > box.maxX) && (x < buffer->width))  { box.maxX = x; }
        if ((y > box.maxY) && (y < buffer->height)) { box.maxY = y; }
    }
    return box;
}



#endif

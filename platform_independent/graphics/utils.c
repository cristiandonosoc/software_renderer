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
    box2i box = { .minX = buffer->width - 1,
                  .minY = buffer->height - 1,
                  .maxX = 0,
                  .maxY = 0 };

    for(int i = 0; i < 3; ++i)
    {
        if (vertices[i].x < box.minX) { box.minX = vertices[i].x; }
        if (vertices[i].x > box.maxX) { box.maxX = vertices[i].x; }
        if (vertices[i].y < box.minY) { box.minY = vertices[i].y; }
        if (vertices[i].y > box.maxY) { box.maxY = vertices[i].y; }

    }
    return box;
}



#endif

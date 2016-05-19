#ifndef PI_GRAPHICS_UTILS_C
#define PI_GRAPHICS_UTILS_C

#include "../graphics.h"
#include "../vectors.c"
#include "../macros.h"

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
    box2i box = { .min = { buffer->width - 1, buffer->height - 1 },
                  .max = { 0, 0 } };

    for(int i = 0; i < 3; ++i)
    {
        int x = vertices[i].x;
        int y = vertices[i].y;
        if ((x < box.min.x) && (x >= 0)) { box.min.x = x; }
        if ((y < box.min.y) && (y >= 0)) { box.min.y = y; }
        if ((x > box.max.x) && (x < buffer->width))  { box.max.x = x; }
        if ((y > box.max.y) && (y < buffer->height)) { box.max.y = y; }
    }
    return box;
}

vec3d ObtainBaricentricCoordenate(int x, int y, vec2i vertices[3])
{
    vec3i xVec = { vertices[1].x - vertices[0].x,
                   vertices[2].x - vertices[0].x,
                               x - vertices[0].x };
    vec3i yVec = { vertices[1].y - vertices[0].y,
                   vertices[2].y - vertices[0].y,
                               y - vertices[0].y };
    vec3i cross = CrossProducti(xVec, yVec);
    if (ABS(cross.z) < 1.0) 
    { 
        vec3d a = { -1, 1, 1 }; 
        return a;
    }
    vec3d baricentric = { -1.0,
                          (double)cross.x / -(double)cross.z,
                          (double)cross.y / -(double)cross.z };
    baricentric.x = 1.0 - (baricentric.y + baricentric.z);
    return baricentric;
}

float InterpolateBaricentric(vec3d baricentric, float a, float b, float c)
{
    float value = ((baricentric.x * a) +
                   (baricentric.y * b) +
                   (baricentric.z * c));
    return value;
}



int UpdateZBuffer(int x, int y, float z, graphics_buffer *buffer)
{
    float *zBuffer = (float *)buffer->zBuffer;

    if (zBuffer[buffer->width * y + x] >= z) { return 0; }
    zBuffer[buffer->width * y + x] = z;
    return 1;
}

int Clampi(int value,  int min, int max)
{
    if (value < min) { value = min; }
    else if (value > max) { value = max; }
    return value;
}

float Clampd(float value, float min, float max)
{
    if (value < min) { value = min; }
    else if (value > max) { value = max; }
    return value;
}





#endif

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

typedef struct _vertex2i
{
    union
    {
        vec2i v;
        struct 
        {
            int x;
            int y;
        };
    };
    int color;
} vertex2;

typedef struct _vertex2d
{
    union
    {
        vec2d v;
        struct
        {
            double x;
            double y;
        };
    };
    int color;
} vertex2d;

typedef struct _vertex3i
{
    union
    {
        vec3i v;
        struct 
        {
            int x;
            int y;
            int z;
        };
    };
    int color;
} vertex3;

typedef struct _vertex3d
{
    union
    {
        vec3d v;
        struct
        {
            double x;
            double y;
            double z;
        };
    };
    int color;
} vertex3d;



#endif

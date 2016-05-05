#include "graphics.h"
#include "macros.h"
#include "vectors.h"
#include "obj.h"
#include "colors.h"

#include "vectors.c"

void ClearBuffer(graphics_buffer *buffer, int clearColor)
{
  int *pixel = (int *)buffer->data;
  for (int y = 0; y < buffer->height; ++y)
  {
    for (int x = 0; x < buffer->width; ++x)
    {
      *pixel++ = clearColor;
    }
  }
}

void DrawLine(int x0, int y0, int x1, int y1, graphics_buffer *buffer, int color)
{
    // Vertical line
    if (x0 == x1)
    {
        int x = x0;
        if ((x < 0) || (x >= buffer->width)) { return;}

        if (y0 > y1) { SWAP(y0, y1, t1, int) }
        if (y0 < 0) { y0 = 0; }
        if (y1 >= buffer->height) { y1 = buffer->height - 1; }

        int *pixel = (int *)buffer->data;
        for (int y = y0; y != y1; ++y)
        {
            
            pixel[buffer->width * y + x] = color;
        }
        return;
    }

    float m = (float)(y1 - y0) / (float)(x1 - x0); // x0 != x1

    int steep = 0;
    // If the gradient is steep, we traspose the lines (we treat y as x)
    // So only we have one loop.
    // I prefer two loops, is cleaner
    if (ABS(m) > 1.0f)
    {
        steep = 1;
        m = 1 / m;
        SWAP(x0, y0, t1, int)
        SWAP(x1, y1, t2, int)
    }

    // We iterate from left to right
    if (x1 < x0) 
    { 
        SWAP(x0, x1, xTemp, int) 
        SWAP(y0, y1, yTemp, int)
    }


    int y = y0;
    int yDir = 1;
    if (m < 0.0f) { yDir = -1; }

    int *pixel = (int *)buffer->data;
    float error = 0;
    float errorPerX = ABS(m);
    for (int x = x0; x <= x1; ++x)
    {
        // If within the bounds, we draw
        if ((x >= 0) && (x < buffer->width) &&
            (y >= 0) && (y < buffer->height))
        {
            // If the gradient if steep, we traspose the line
            if (!steep)
            {
                pixel[buffer->width * y + x] = color;
            }
            else
            {
                pixel[buffer->width * x + y] = color;
            }
        }

        // We correct for error
        error += errorPerX;
        if (error >= 1.0f)
        {
            y += yDir;
            error -= 1.0f;
        }
    }
}

void DrawVertices(vertex3d *v0, vertex3d *v1, graphics_buffer *buffer)
{
    int x0 = (int)((v0->x + 1.0) * (double)buffer->width / (double)2);
    int y0 = (int)((v0->y + 1.0) * (double)buffer->height / (double)2);

    int x1 = (int)((v1->x + 1.0) * (double)buffer->width / (double)2);
    int y1 = (int)((v1->y + 1.0) * (double)buffer->height / (double)2);

    DrawLine(x0, y0, x1, y1, buffer, 0xFFFFFFFF);
}

// TODO: Check buffer boundaries
void DrawTriangleScan(vec2i v0, vec2i v1, vec2i v2, graphics_buffer *buffer, int color)
{
    if (v1.y < v0.y) { SWAP(v0, v1, temp, vec2i) }
    if (v2.y < v0.y) { SWAP(v0, v2, temp, vec2i) }
    if (v2.y < v1.y) { SWAP(v1, v2, temp, vec2i) }

    int *pixel = (int *)buffer->data;

    // We draw the top half of the borders
    double t;
    int v0v1 = (v1.y - v0.y);
    double v0v1Diff = (double)(v0v1 + 1);
    int v0v2 = (v2.y - v0.y);
    double v0v2Diff = (double)(v0v2 + 1);

    // First half of the triangle
    for (int y = v0.y; y <= v1.y; ++y)
    {
        if ((y < 0) || (y >= buffer->height)) { continue; }

        int x0 = v0.x;
        if (v0v1 > 0)
        {
            t = (double)(y - v0.y)/v0v1Diff;                // v0.y == v1.y ???
            x0 = v0.x + (int)(t * (v1.x - v0.x));     
            if ((x0 >= 0) && (x0 < buffer->width))
            {
                pixel[buffer->width * y + x0] = color;
            }
        }

        int x1 = v1.x;
        if (v0v2 > 0)
        {
            t = (double)(y - v0.y)/v0v2Diff;
            x1 = v0.x + (int)(t * (v2.x - v0.x));    
            if ((x1 >= 0) && (x1 < buffer->width))
            {
                pixel[buffer->width * y + x1] = color;
            }
        }

        // We fill the half
        if (x0 < 0) { x0 = 0; }
        if (x0 >= buffer->width) { x0 = buffer->width; }

        if (x1 < 0) { x1 = 0; }
        if (x1 >= buffer->width) { x1 = buffer->width; }

        if (x1 < x0) { SWAP(x0, x1, t, int) }
        for (int x = x0 + 1; x < x1; ++x)
        {
            pixel[buffer->width * y + x] = color;
        }
    }

    // Second half of the triangle
    double v1v2Diff = (double)(v2.y - v1.y);
    for (int y = v1.y + 1; y <= v2.y; ++y)
    {
        t = (double)(y - v1.y)/v1v2Diff;                
        int x1 = v1.x + (int)(t * (v2.x - v1.x));
        pixel[buffer->width * y + x1] = color;

        t = (double)(y - v0.y)/v0v2Diff;
        int x0 = v0.x + (int)(t * (v2.x - v0.x));    
        pixel[buffer->width * y + x0] = color;

        // We fill the half
        if (x1 < x0) { SWAP(x0, x1, t, int) }
        for (int x = x0 + 1; x < x1; ++x)
        {
            pixel[buffer->width * y + x] = color;
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

void DrawTriangle(vec2i vertices[3], graphics_buffer *buffer, int color)
{
    int *pixel = (int *)buffer->data;
    box2i boundingBox = GetTriangleBoundingBox2i(vertices, buffer);
    for (int y = boundingBox.minY; y <= boundingBox.maxY; ++y)
    {
        for (int x = boundingBox.minX; x <= boundingBox.maxX; ++x)
        {
            vec3i xVec = { vertices[1].x - vertices[0].x,
                           vertices[2].x - vertices[0].x,
                                       x - vertices[0].x };

            vec3i yVec = { vertices[1].y - vertices[0].y,
                           vertices[2].y - vertices[0].y,
                                       y - vertices[0].y };

            vec3i cross = CrossProducti(xVec, yVec);

            vec3d baricentric = { -1.0,
                                  (double)cross.x / -(double)cross.z,
                                  (double)cross.y / -(double)cross.z };
            baricentric.x = 1.0 - baricentric.y - baricentric.z;
            if ((baricentric.x < 0) || (baricentric.x > 1) ||
                (baricentric.y < 0) || (baricentric.y > 1) ||
                (baricentric.z < 0) || (baricentric.z > 1))
            {
                continue;
            }

            pixel[buffer->width * y + x] = color;
        }
    }
}

void DrawTriangleSeparate(vec2i v0, vec2i v1, vec2i v2, graphics_buffer *buffer, int color)
{
    vec2i vecs[3] = { v0, v1, v2 };
    DrawTriangle(vecs, buffer, color);
}

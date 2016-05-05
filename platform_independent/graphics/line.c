#ifndef PI_GRAPHICS_LINE_C
#define PI_GRAPHICS_LINE_C

#include "..\macros.h"
#include "..\graphics.h"

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

#endif

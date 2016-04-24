#include "macros.h"
#include <windows.h>

static int pixelSleepMs = 5;

typedef struct _graphics_buffer 
{
  int width;
  int height;
  int bytesPerPixel;
  int pitch;

  void *data;
} graphics_buffer;

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

void Line(int x0, int y0, int x1, int y1, graphics_buffer *buffer, int color)
{

  // Vertical special case
  if (x0 == x1)
  {
    int yDir = 1;
    if (y0 > y1) { yDir = -1; }
    int *pixel = (int *)buffer->data;
    for (int y = y0; y != y1; y += yDir)
    {
      pixel[buffer->width * y + x0] = color;
    }

    return;
  }

  // NOTE(Cristian): x1 != x0
  float m = (float)(y1 - y0) / (float)(x1 - x0);
  float n = y0 - x0 * m;
  if ((-1.0 < m) && (m < 1.0))
  {
    // Iteration on x
    int y = 0;
    int *pixel = (int *)buffer->data;

    int xDir = 1;
    if (x0 > x1) { xDir = -1; }

    for (int x = x0; x != x1; x += xDir)
    {
      y = (int)(x * m + n);
      pixel[buffer->width * y + x] = color;
    }
  }
  else
  {
    // Iteration on y
    int x = 0;
    int *pixel = (int *)buffer->data;

    int yDir = 1;
    if (y0 > y1) { yDir = -1; }

    for (int y = y0; y != y1; y += yDir)
    {
      // m != 0 because that would be the m < 1.0 case
      x = (y - n) / m;
      pixel[buffer->width * y + x] = color;
    }
  }
}

void Line2(int x0, int y0, int x1, int y1, graphics_buffer *buffer, int color)
{
    // Vertical line
    if (x0 == x1)
    {
        int yDir = 1;
        if (y0 > y1) { yDir = -1; }
        int *pixel = (int *)buffer->data;
        for (int y = y0; y != y1; y += yDir)
        {
          pixel[buffer->width * y + x0] = color;
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
            Sleep(pixelSleepMs);
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

void CreateImage(graphics_buffer *buffer)
{
    // COLOR IS 0xXXRRGGBB
    Line2(-10, -10, 500, 500, buffer, 0xFFFFFF00);
    Line2(-100, 500, 100, -40, buffer, 0xFFFFFFFF);
    Line2(400, 390, 10, 0, buffer, 0xFFFFFFFF);
    Line2(100, 380, 300, 20, buffer, 0xFF0000FF);
    Line2(0, 0, 150, 200, buffer, 0xFF00FF00);
}

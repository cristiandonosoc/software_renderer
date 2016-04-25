#include <X11/Xlib.h>

#include <stdio.h>

#include <malloc.h>

#include "platform_independent/graphics.h"

static graphics_buffer gBuffer;

void DrawRectangle(int x0, int y0, int x1, int y1, graphics_buffer *buffer, int color)
{
  int *pixel = (int *)buffer->data;
  for (int y = y0; y <= y1; ++y)
  {
    for (int x = x0; x <= x1; ++x)
    {
      pixel[buffer->width * y + x] = color;
    }
  }
}

int main(int argc, char *argv[])
{
  Display *display = XOpenDisplay(0);


  int winWidth = 500;
  int winHeight = 500;
  int bytesPerPixel = 4;
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                      0, 0, winWidth, winHeight, 0,
                                      0xFFFFFFFF, 0);
  GC gc = XCreateGC(display, window, 0, 0);


  XMapWindow(display, window);


  gBuffer.width = winWidth;
  gBuffer.height = winHeight;
  gBuffer.data = malloc(winWidth * winHeight * bytesPerPixel);
  int *pixel = (int *)gBuffer.data;

  DrawRectangle(0, 0, 50, 50, &gBuffer, 0xFFFFFF00);
  DrawRectangle(40, 40, 130, 300, &gBuffer, 0xFF0F0FFF);

  int screen = DefaultScreen(display);
  Visual *visual = DefaultVisual(display, screen);
  int depth = DefaultDepth(display, screen);
  XImage *image = XCreateImage(display, 
                               visual,
                               depth,
                               ZPixmap, 
                               0,
                               (char *)gBuffer.data,
                               winWidth,
                               winHeight,
                               32,
                               0);
  XInitImage(image);
  XSelectInput(display, window, ExposureMask);

  while(1)
  {
    XEvent event;
    XNextEvent(display, &event);

    switch (event.type)
    {
      case Expose:
        XPutImage(display, window, gc, image,
                  0, 0, 0, 0,
                  winWidth, winHeight);
        XFlush(display);
    }
  }
}

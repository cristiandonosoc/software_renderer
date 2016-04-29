#include <X11/Xlib.h>

#include <stdio.h>

#include <malloc.h>
#include <unistd.h>
#include <pthread.h>

#include "platform_independent/graphics.c"

static graphics_buffer gBuffer;

typedef struct _image_thread_holder
{
  graphics_buffer *buffer;
  char *modelPath;
} image_thread_holder;

void *ImageThreadFunction(void *input)
{
  image_thread_holder *holder = (image_thread_holder *)input;
  graphics_buffer *buffer = (graphics_buffer *)input;
  CreateImage(holder->buffer, holder->modelPath);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage is: renderer <model_path>");
    return EXIT_FAILURE;
  }

  char *modelPath = argv[1];

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

  // We start the thread
  pthread_t thread;
  int result = pthread_create(&thread, 0, ImageThreadFunction, (void *)(&gBuffer));
  if (result) 
  {
    fprintf(stderr, "Error with pthread_create. Code: %d\n", result);
    exit(EXIT_FAILURE);
  }

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


  int xMasks = ExposureMask | KeyPressMask;
  XSelectInput(display, window, xMasks);


  // Nasty hack
  /* Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False); */
  /* XSetWMProtocols(display, window, &wmDeleteMessage, 1); */

  int loopRunning = 1;
  while(loopRunning)
  {
    XEvent event;
    /* XNextEvent(display, &event); */

    // We flush the events queue
    while (XCheckMaskEvent(display, xMasks, &event))
    {
      switch (event.type)
      {
        case Expose:
          XPutImage(display, window, gc, image,
                    0, 0, 0, 0,
                    winWidth, winHeight);
          XFlush(display);
          break;
        // Hack for the window close eventExposureMask
        // TODO(Cristian): See how to do this correctly
        case ClientMessage:
          if (event.xclient.data.l[0] == wmDeleteMessage)
          {
            loopRunning = 0;
          }
          break;
      }
    }

    // We sleep for a while
    usleep(100 * 1000);

    XPutImage(display, window, gc, image,
              0, 0, 0, 0,
              winWidth, winHeight);
    XFlush(display);

  }
}

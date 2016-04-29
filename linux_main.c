#include <X11/Xlib.h>

#include <stdio.h>

#include <malloc.h>
#include <unistd.h>
#include <pthread.h>

#include "platform_independent/graphics.c"

static graphics_buffer gBuffer;
// Buffer to which the information is flipped
static graphics_buffer xBuffer;

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

// Both buffers must be the same widthXheight
void TransferBuffer(graphics_buffer *from, graphics_buffer *to)
{
  // We flip on y
  int *fromPixel = (int *)(from->data);
  int *toPixel = (int *)(to->data);
  for (int y = 0; y < from->height; ++y)
  {
    for (int x = 0; x < from->width; ++x) 
    {
      toPixel[to->width * y + x] = fromPixel[(to->width) * (to->width - 1 - y) + x];
    }
  }
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
  
  // We allocate the image buffer
  gBuffer.width = winWidth;
  gBuffer.height = winHeight;
  gBuffer.data = malloc(winWidth * winHeight * bytesPerPixel);

  xBuffer.width = winWidth;
  xBuffer.height = winHeight;
  xBuffer.data = malloc(winWidth * winHeight * bytesPerPixel);

  // We start the image thread
  pthread_t thread;
  image_thread_holder holder;
  holder.buffer = &gBuffer;
  holder.modelPath = modelPath;
  int result = pthread_create(&thread, 0, ImageThreadFunction, (void *)(&holder));
  if (result) 
  {
    fprintf(stderr, "Error with pthread_create. Code: %d\n", result);
    exit(EXIT_FAILURE);
  }


  //
  Display *display = XOpenDisplay(0);

  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                      0, 0, winWidth, winHeight, 0,
                                      0xFFFFFFFF, 0);
  GC gc = XCreateGC(display, window, 0, 0);


  XMapWindow(display, window);


  int screen = DefaultScreen(display);
  Visual *visual = DefaultVisual(display, screen);
  int depth = DefaultDepth(display, screen);
  XImage *image = XCreateImage(display, 
                               visual,
                               depth,
                               ZPixmap, 
                               0,
                               (char *)xBuffer.data,
                               winWidth,
                               winHeight,
                               32,
                               0);
  XInitImage(image);


  int xMasks = ExposureMask | KeyPressMask;
  XSelectInput(display, window, xMasks);


  // Nasty hack
  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
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
          TransferBuffer(&gBuffer, &xBuffer);
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

    TransferBuffer(&gBuffer, &xBuffer);
    XPutImage(display, window, gc, image,
              0, 0, 0, 0,
              winWidth, winHeight);
    XFlush(display);

  }
}

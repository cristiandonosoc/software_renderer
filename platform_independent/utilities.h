#ifndef PI_UTILITIES_H
#define PI_UTILITIES_H

#include "graphics.h"

typedef struct _program_info
{
    graphics_buffer *buffer;
    int task;
    char *modelPath;
    int winWidth;
    int winHeight;
} program_info;

#endif

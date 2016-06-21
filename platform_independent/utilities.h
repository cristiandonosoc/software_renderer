#ifndef PI_UTILITIES_H
#define PI_UTILITIES_H

#include "graphics.h"

typedef struct _draw_control
{
    int drawPrev;
    int drawNext;
} draw_control;

typedef struct _program_info
{
    graphics_buffer *buffer;
    int task;
    char *modelPath;
    int winWidth;
    int winHeight;
    draw_control drawControl;
    int running;
} program_info;

#endif

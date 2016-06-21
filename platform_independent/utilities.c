#ifndef PI_UTILITIES_C
#define PI_UTILITIES_C

#include "utilities.h"

#ifdef _WIN32
#include "../platform/win32/win32_utilities.c"
#include "../platform/win32/win32_getopt.c"
#elif __linux__

#endif

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

program_info GetProgramInfoFromArgs(int argc, char *argv[])
{
    program_info result;
    // window is 700x700 by default
    result.winWidth = 700;
    result.winHeight = 700;
    int c;
    while((c = getopt(argc, argv, "t:m:s:")) != -1)
    {
        switch (c)
        {
            case 't':
                result.task = atoi(optarg);
                break;
            case 'm':
                result.modelPath = optarg;
                break;
            case 's':
            {
                // We search for the first number
                int index = 0;
                while (('0' <= optarg[index]) && (optarg[index] <= '9')) { ++index; }
                if (index == 0) { goto wrong_size; }
                // We search for the x
                if (optarg[index] != 'x') { goto wrong_size; }
                int xIndex = index;
                ++index;
                while (('0' <= optarg[index]) && (optarg[index] <= '9')) { ++index; }
                if (index == (xIndex + 1)) { goto wrong_size; }
                if (optarg[index] != '\0') { goto wrong_size; }

                // We mark the x as '\0' and we can do atoi on both numbers
                optarg[xIndex] = '\0';
                result.winWidth = atoi(optarg);
                result.winHeight = atoi(optarg + xIndex + 1);
                break;
wrong_size:
                fprintf(stderr, "Please specify a correct size: \"<width>x<height>\"\n" //str concat
                                "Using default 700x700\n");
                break;
            }
        }
    }

    return result;
}

void PrintTasks(FILE *stream)
{
    fprintf(stream, "Usage is: renderer -m <model_path> -t <task_number> -s <widthxheight> (700x700 default)\n"); 
    fprintf(stream, "Tasks are:\n");
    fprintf(stream, "1. Draw Wireframe model without projection\n");
    fprintf(stream, "2. Draw model with fill triangle without projection\n");
    fprintf(stream, "3. Draw model with z-buffer without projection\n");
    fprintf(stream, "4. Draw model with z-buffer, texture mapping, without projection\n");
    fprintf(stream, "5. Draw model with z-buffer, texture mapping, with projection\n");
}

void SetupGraphicsBuffer(graphics_buffer *buffer, int width, int height, int bytesPerPixel)
{
    // We create out render buffer
    buffer->width = width;
    buffer->height = height;
    buffer->bytesPerPixel = bytesPerPixel;
    buffer->pitch = bytesPerPixel * width;
    buffer->data = malloc(width * height * bytesPerPixel);
    buffer->zBuffer = malloc(width * height * bytesPerPixel);

    float *pixel = (float *)buffer->zBuffer;
    for (int y = 0; y < buffer->height; ++y)
    {
        for (int x = 0; x < buffer->width; ++x)
        {
            pixel[buffer->width * y + x] = -100.0;
        }
    }
}

#endif

#ifndef PI_UNDO_C
#define PI_UNDO_C

#include "macros.h"
#include "graphics.h"

typedef struct _undo_footer
{
    int width;
    int height;
    void *data;
} undo_footer;


/**
 * The undo works like this:
 * Everytime there is a draw:
 * 1. Identify the change bounding box
 * 3. Store the previous information
 * 2. Store a footer describing that change
 *
 * */

#define PIXEL_WIDTH    4
#define AVG_WIDTH    100
#define AVG_HEIGHT   100
#define MAX_CHANGES 1000
// 4 * 100 * 100 * 1000 = ~38.2 MB 
#define TOTAL_BUFFER (PIXEL_WIDTH * AVG_WIDTH * AVG_HEIGHT * MAX_CHANGES)

char undoBlock[TOTAL_BUFFER];
char *currentIndex = undoBlock;

undo_footer footers[MAX_CHANGES];
int footer_count = 0;

void AddChange(graphics_buffer *buffer, box2i *boundingBox, void *data)
{
    ASSERT(footer_count < MAX_CHANGES)

    int width = boundingBox->max.x - boundingBox->min.x;
    int height = boundingBox->max.y - boundingBox->min.y;

    ASSERT(((currentIndex - undoBlock) + buffer->bytesPerPixel * width * height) < TOTAL_BUFFER)

    // We store the header
    footers[footer_count].width = width;
    footers[footer_count].height = height;
    footers[footer_count].data = (void *)currentIndex;
    ++footer_count;

    // We store the data
    int *pixel = (int *)data;
    int *undoPixel = (int *)currentIndex;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            undoPixel[width * y + x] = pixel[buffer->width * y + x];
            currentIndex += buffer->bytesPerPixel;
        }
    }
}

void RemoveChange(graphics_buffer *buffer, void *data)
{
    ASSERT(footer_count >= 0)

    int *pixel = (int *)data;
    int *bufferData = (int *)footers[footer_count].data;
    for (int y = 0; y < footers[footer_count].height; ++y)
    {
        for (int x = 0; x < footers[footer_count].width; ++x)
        {
            pixel[buffer->width * y + x] = *bufferData++;
        }
    }

    // We revert the buffer data
    --footer_count;
}


#endif

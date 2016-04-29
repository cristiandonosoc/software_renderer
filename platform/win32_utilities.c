#include <windows.h>

void usleep(int usec)
{
    // We transform to millisenc
    int msec = usec / 1000;
    Sleep(msec);
}

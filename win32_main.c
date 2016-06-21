#include <windows.h>
#include <stdlib.h>
#include <malloc.h>

#include "platform_independent/graphics.h"
#include "platform_independent/utilities.c"

#include "main.c"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct _window_dimension
{
    int width;
    int height;
} window_dimension;

window_dimension GetWindowDimensions(HWND handle)
{
    RECT clientRect;
    GetClientRect(handle, &clientRect);
    window_dimension result = { clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top };
    return result;
}

BITMAPINFO bitmapInfo;
static graphics_buffer gBuffer;

static program_info gProgramInfo;

DWORD WINAPI ImageThreadFunction(LPVOID input)
{
    program_info *programInfo = (program_info*)input;
    SelectTask(programInfo);
    return 0;
}

int main(int argc, char *argv[])
{
    gProgramInfo = GetProgramInfoFromArgs(argc, argv);
    gProgramInfo.buffer = &gBuffer;

    static TCHAR szAppName [] = TEXT("BitBlt") ;

    HWND         handle;
    WNDCLASS     wndclass;

    HINSTANCE hInstance = GetModuleHandle(0);

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_INFORMATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                szAppName, MB_ICONERROR) ;
        return 0;
    }

    int bytesPerPixel = 4; // RGBA

    handle = CreateWindowEx(0,szAppName, TEXT ("BitBlt Demo"), 
                          WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT,         // x, y position
                          gProgramInfo.winWidth, gProgramInfo.winHeight,                  // width, height
                          NULL, NULL, hInstance, NULL) ;

    window_dimension dim = GetWindowDimensions(handle);

    // We set the BITMAP INFO HEADER
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = dim.width;
    bitmapInfo.bmiHeader.biHeight = dim.height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = bytesPerPixel * 8;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    SetupGraphicsBuffer(&gBuffer, dim.width, dim.height, bytesPerPixel);
    ClearBuffer(&gBuffer, 0x00000000);

    ShowWindow(handle, SW_SHOWNORMAL);
    UpdateWindow(handle);

    // We create the thread that creates the image
    unsigned long threadId;
    HANDLE imageThreadHandle = CreateThread(0,                      // No security measures
                                            0,                      // default stack size
                                            ImageThreadFunction,    // function to be called
                                            &gProgramInfo,           // pointer to the data
                                            0,                      // 
                                            &threadId);             // thread id
    
    gProgramInfo.running = 1;
    while (gProgramInfo.running)
    {
        MSG message;
        while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            int processed = 0;
            switch(message.message)
            {
                // We peak at keyboard events
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    processed = 1;
                    uint32 keyCode = (uint32)message.wParam;

                    // Magic
                    int keyWasDown = (message.lParam & (1 << 30)) ? 1 : 0;
                    int keyIsDown = (message.lParam & (1 << 31)) ? 0 : 1;

                    // We only take care of key events
                    if (keyWasDown != keyIsDown)
                    {
                        switch (keyCode)
                        {
                            case VK_ESCAPE: { gProgramInfo.running = 0; }
                            case VK_LEFT:
                            {
                                if (!keyIsDown)
                                {
                                    fprintf(stdout, "PREV\n");
                                    if (!gProgramInfo.drawControl.drawNext)
                                    {
                                        gProgramInfo.drawControl.drawPrev = 1;
                                    }
                                }
                                break;
                            }
                            case VK_RIGHT:
                            {
                                if (!keyIsDown)
                                {
                                    gProgramInfo.drawControl.drawNext = 1;
                                    fprintf(stdout, "NEXT\n");
                                }
                                break;
                            }
                        }
                    }
                }
            }

            if (!processed) 
            {
                DefWindowProc(handle, message.message, message.wParam, message.lParam);
            }
        }

        // After we saw the image, we want to send the buffer to the window
        Sleep(33);   // ~30 FPS

        HDC context = GetDC(handle);

        StretchDIBits(context,
                      0, 0, gBuffer.width, gBuffer.height,
                      0, 0, gBuffer.width, gBuffer.height,
                      gBuffer.data,
                      &bitmapInfo,
                      DIB_RGB_COLORS,
                      SRCCOPY);

        ReleaseDC(handle, context);

    }

    // We finished the application,
    // we don't care for the thread any more
    TerminateThread(imageThreadHandle, 1);
    return 0;
}

LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int  cxClient, cyClient, cxSource, cySource;
    /* HDC         hdcClient, hdcWindow; */
    /* int         x, y; */
    /* PAINTSTRUCT ps; */

    switch (message)
    {
        case WM_CREATE:
            cxSource = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXSIZE);
            cySource = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
            return 0 ;
        case WM_SIZE:
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            return 0 ;
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
            {
                gProgramInfo.running = 0;
                break;              // We still need the windows processing
            }
        case WM_PAINT:
            {

                PAINTSTRUCT paint;
                HDC context = BeginPaint(handle, &paint);

                StretchDIBits(context,
                              0, 0, gBuffer.width, gBuffer.height,
                              0, 0, gBuffer.width, gBuffer.height,
                              gBuffer.data,
                              &bitmapInfo,
                              DIB_RGB_COLORS,
                              SRCCOPY);

                //ReleaseDC(handle, hdcWindow);
                EndPaint(handle, &paint);

                return 0;
            }
        case WM_ERASEBKGND:
            // We stop erasing the background
            return 1;
    }

    // All other cases are handled by the default procedure
    return DefWindowProc(handle, message, wParam, lParam) ;
}

#include <windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>

#include <stdio.h>

#include "platform_independent/graphics.c"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

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

typedef struct _back_buffer
{
    BITMAPINFO info;
    graphics_buffer buffer;
} back_buffer;
static back_buffer gBuffer;
static int gLoopRunning = 1;


void SetupGraphicsBuffer(int width, int height, int bytesPerPixel)
{
    // We create out render buffer
    gBuffer.buffer.width = width;
    gBuffer.buffer.height = height;
    gBuffer.buffer.bytesPerPixel = bytesPerPixel;
    gBuffer.buffer.pitch = bytesPerPixel * width;
    gBuffer.buffer.data = malloc(width * height * bytesPerPixel);

    // We set the BITMAP INFO HEADER
    gBuffer.info.bmiHeader.biSize = sizeof(gBuffer.info.bmiHeader);
    gBuffer.info.bmiHeader.biWidth = width;
    gBuffer.info.bmiHeader.biHeight = height;
    gBuffer.info.bmiHeader.biPlanes = 1;
    gBuffer.info.bmiHeader.biBitCount = bytesPerPixel * 8;
    gBuffer.info.bmiHeader.biCompression = BI_RGB;

    ClearBuffer(&gBuffer.buffer, 0x00000000);
}

DWORD WINAPI ImageThreadFunction(LPVOID input)
{
    // We cast the input
    graphics_buffer *buffer = (graphics_buffer *)input;
    CreateImage(buffer);
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, 
        HINSTANCE hPrevInstance,
        PSTR szCmdLine, 
        int iCmdShow)
{
    static TCHAR szAppName [] = TEXT("BitBlt") ;
    HWND         handle;
    WNDCLASS     wndclass ;

    hPrevInstance = 0;
    szCmdLine = 0;

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

    int winWidth = 1000;
    int winHeight = 1000;
    int bytesPerPixel = 4; // RGBA

    handle = CreateWindowEx(0,szAppName, TEXT ("BitBlt Demo"), 
                          WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT,         // x, y position
                          winWidth, winHeight,                  // width, height
                          NULL, NULL, hInstance, NULL) ;

    window_dimension dim = GetWindowDimensions(handle);

    char string[256];
    sprintf_s(string, sizeof(string), "WIDTH: %d, HEIGHT: %d", dim.width, dim.height);
    OutputDebugString(string);

    SetupGraphicsBuffer(dim.width, dim.height, bytesPerPixel);

    ShowWindow(handle, iCmdShow);
    UpdateWindow(handle);

    // We create the thread that creates the image
    unsigned long threadId;
    HANDLE imageThreadHandle = CreateThread(0,                   // No security measures
                                            0,                   // default stack size
                                            ImageThreadFunction, // function to be called
                                            &gBuffer.buffer,     // pointer to the data
                                            0,                   // 
                                            &threadId);          // thread id
    
    gLoopRunning = 1;
    while (gLoopRunning)
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
                        if (keyCode == VK_ESCAPE)
                        {
                            gLoopRunning = 0;
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
                0, 0, gBuffer.buffer.width, gBuffer.buffer.height,
                0, 0, gBuffer.buffer.width, gBuffer.buffer.height,
                gBuffer.buffer.data,
                &gBuffer.info,
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
                gLoopRunning = 0;
                break;              // We still need the windows processing
            }
        case WM_PAINT:
            {

                PAINTSTRUCT paint;
                HDC context = BeginPaint(handle, &paint);

                StretchDIBits(context,
                        0, 0, gBuffer.buffer.width, gBuffer.buffer.height,
                        0, 0, gBuffer.buffer.width, gBuffer.buffer.height,
                        gBuffer.buffer.data,
                        &gBuffer.info,
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

/*---------------------------------------
  BITBLT.C -- BitBlt Demonstration
  (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>

#include "platform_independent/graphics.c"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) ;

struct window_dimension
{
    int width;
    int height;
};


struct window_dimension GetWindowDimensions(HWND handle)
{

    RECT clientRect;
    GetClientRect(handle, &clientRect);
    struct window_dimension result = { clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top };
    return result;
}

struct back_buffer
{
    BITMAPINFO info;
    struct graphics_buffer buffer;
};
static struct back_buffer gBuffer;

int WINAPI WinMain(HINSTANCE hInstance, 
        HINSTANCE hPrevInstance,
        PSTR szCmdLine, 
        int iCmdShow)
{


    static TCHAR szAppName [] = TEXT("BitBlt") ;
    HWND         hwnd ;
    MSG          msg ;
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

    int width = 400;
    int height = 400;
    int bytesPerPixel = 4; // RGBA

    hwnd = CreateWindowEx(0,szAppName, TEXT ("BitBlt Demo"), 
            WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT,    // x, y position
            width + 50, height + 50,                   // width, height
            NULL, NULL, hInstance, NULL) ;

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


    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

        case WM_PAINT:
            {

                //hdcWindow = GetWindowDC(hwnd);
                PAINTSTRUCT paint;
                HDC context = BeginPaint(hwnd, &paint);

                // COLOR IS 0xXXRRGGBB

                /* Line2(113, 120, 180, 140, &gBuffer.buffer, 0xFF00FF00); */ 
                /* Line2(20, 13, 40, 80, &gBuffer.buffer, 0xFF0000FF); */ 
                /* Line2(280, 240, 213, 220, &gBuffer.buffer, 0xFFFF0000); */
                /* Line2(380, 320, 390, 10, &gBuffer.buffer, 0xFFFFFF00); */
                /* Line2(40, 380, 200, 390, &gBuffer.buffer, 0xFF0000FF); */
                /* Line2(10, 10, 390, 390, &gBuffer.buffer, 0xFFFF0000); */

                Line2(400, 390, 10, 0, &gBuffer.buffer, 0xFFFFFFFF);

                Line2(100, 380, 300, 20, &gBuffer.buffer, 0xFF0000FF);

                //window_dimension dim = GetWindowDimensions(hwnd);

                StretchDIBits(context,
                        10, 10, gBuffer.buffer.width, gBuffer.buffer.height,
                        0, 0, gBuffer.buffer.width, gBuffer.buffer.height,
                        gBuffer.buffer.data,
                        &gBuffer.info,
                        DIB_RGB_COLORS,
                        SRCCOPY);

                //ReleaseDC(hwnd, hdcWindow);
                EndPaint(hwnd, &paint);

                /* hdcClient = BeginPaint(hwnd, &ps); */
                /* hdcWindow = GetWindowDC(hwnd); */

                /* for (y = 0 ; y < cyClient ; y += cySource) */
                /* { */
                /*   for (x = 0 ; x < cxClient ; x += cxSource) */
                /*   { */
                /*     BitBlt(hdcClient, */ 
                /*         x, y, cxSource, cySource, */
                /*         hdcWindow, 0, 0, */ 
                /*         SRCCOPY); */
                /*   } */
                /* } */

                /* ReleaseDC(hwnd, hdcWindow); */
                /* EndPaint(hwnd, &ps); */
                return 0;
            }
        case WM_ERASEBKGND:
            // We stop erasing the background
            return 1;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam) ;
}

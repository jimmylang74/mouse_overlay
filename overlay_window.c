#include <windows.h>

#include "globals.h"
#include "overlay_window.h"
#include "window_finder.h"

//
// mouse_hook.c 中定义
//
extern POINT g_lastMousePoint;

static RECT g_drawRect = {0};

static LRESULT CALLBACK OverlayProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_NCHITTEST:
        //
        // 鼠标完全穿透
        //
        return HTTRANSPARENT;

    case WM_TIMER:

        if (wParam == 1)
        {
            KillTimer(hwnd, 1);

            //
            // 鼠标停止后再查找窗口
            //
            WindowFinder_Update(g_lastMousePoint);
        }

        return 0;

    case WM_ERASEBKGND:

        //
        // 不需要系统擦背景
        //
        return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hwnd, &ps);

        //
        // 清除上一帧
        //
        RECT rc;
        GetClientRect(hwnd, &rc);

        HBRUSH bg = CreateSolidBrush(RGB(0,0,0));

        FillRect(hdc, &rc, bg);

        DeleteObject(bg);

        //
        // 画新的红框
        //
        if (!IsRectEmpty(&g_drawRect))
        {
            HPEN pen =
                CreatePen(
                    PS_INSIDEFRAME,
                    5,
                    RGB(255,0,0));

            HPEN oldPen =
                (HPEN)SelectObject(hdc, pen);

            HBRUSH oldBrush =
                (HBRUSH)SelectObject(
                    hdc,
                    GetStockObject(HOLLOW_BRUSH));

            Rectangle(
                hdc,
                g_drawRect.left,
                g_drawRect.top,
                g_drawRect.right,
                g_drawRect.bottom);

            SelectObject(
                hdc,
                oldBrush);

            SelectObject(
                hdc,
                oldPen);

            DeleteObject(pen);
        }

        EndPaint(hwnd, &ps);

        return 0;
    }

    case WM_DESTROY:

        PostQuitMessage(0);

        return 0;
    }

    return DefWindowProc(
        hwnd,
        msg,
        wParam,
        lParam);
}

HWND CreateOverlayWindow(
    HINSTANCE hInstance)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MouseOverlayWindow";

    RegisterClass(&wc);

    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd =
        CreateWindowEx(
            WS_EX_LAYERED
            | WS_EX_TRANSPARENT
            | WS_EX_TOPMOST
            | WS_EX_TOOLWINDOW,

            wc.lpszClassName,

            "",

            WS_POPUP,

            0,
            0,
            cx,
            cy,

            NULL,
            NULL,
            hInstance,
            NULL);

    if (hwnd == NULL)
        return NULL;

    //
    // 黑色全部透明
    //
    SetLayeredWindowAttributes(
        hwnd,
        RGB(0,0,0),
        0,
        LWA_COLORKEY);

    ShowWindow(hwnd, SW_SHOW);

    UpdateWindow(hwnd);

    return hwnd;
}

void OverlayUpdateRect(
    const RECT *rc)
{
    if (rc)
    {
        g_drawRect = *rc;
    }
    else
    {
        SetRectEmpty(&g_drawRect);
    }

    //
    // 请求重绘
    //
    InvalidateRect(
        g_overlay,
        NULL,
        TRUE);

    UpdateWindow(g_overlay);
}

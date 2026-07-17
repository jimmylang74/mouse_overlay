#include <windows.h>

#include "globals.h"
#include "mouse_hook.h"
#include "overlay_window.h"
#include "window_finder.h"

//
// mouse_hook.c 中定义
//
extern POINT g_lastMousePoint;

static RECT g_drawRect = {0};

//
// 涟漪动画状态
//
#define RIPPLE_DURATION_MS  400
#define RIPPLE_MAX_RADIUS   50
#define RIPPLE_MAX_PENWIDTH 3
#define RIPPLE_TIMER_ID     2
#define RIPPLE_TIMER_MS     20

static BOOL  g_rippleActive  = FALSE;
static POINT g_rippleCenter  = {0};
static DWORD g_rippleStart   = 0;

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
        else if (wParam == RIPPLE_TIMER_ID)
        {
            DWORD elapsed = GetTickCount() - g_rippleStart;

            if (elapsed >= RIPPLE_DURATION_MS)
            {
                KillTimer(hwnd, RIPPLE_TIMER_ID);
                g_rippleActive = FALSE;
                PrintPendingClick();
            }

            InvalidateRect(hwnd, NULL, TRUE);
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

        if (g_rippleActive)
        {
            DWORD elapsed = GetTickCount() - g_rippleStart;
            float t = (float)elapsed / (float)RIPPLE_DURATION_MS;

            if (t > 1.0f)
                t = 1.0f;

            int radius =
                (int)(t * RIPPLE_MAX_RADIUS);

            int penWidth =
                RIPPLE_MAX_PENWIDTH
                - (int)(t * (RIPPLE_MAX_PENWIDTH - 1));

            if (penWidth < 1)
                penWidth = 1;

            HPEN rPen =
                CreatePen(
                    PS_SOLID,
                    penWidth,
                    RGB(255, 0, 0));

            HPEN oldRPen =
                (HPEN)SelectObject(hdc, rPen);

            HBRUSH oldRBrush =
                (HBRUSH)SelectObject(
                    hdc,
                    GetStockObject(HOLLOW_BRUSH));

            Ellipse(
                hdc,
                g_rippleCenter.x - radius,
                g_rippleCenter.y - radius,
                g_rippleCenter.x + radius,
                g_rippleCenter.y + radius);

            SelectObject(hdc, oldRBrush);
            SelectObject(hdc, oldRPen);
            DeleteObject(rPen);
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

void OverlayTriggerRipple(POINT center)
{
    if (g_rippleActive)
        KillTimer(g_overlay, RIPPLE_TIMER_ID);

    g_rippleCenter = center;
    g_rippleStart  = GetTickCount();
    g_rippleActive = TRUE;

    SetTimer(
        g_overlay,
        RIPPLE_TIMER_ID,
        RIPPLE_TIMER_MS,
        NULL);

    InvalidateRect(g_overlay, NULL, TRUE);
}

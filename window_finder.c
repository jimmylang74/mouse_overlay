#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "window_finder.h"
#include "overlay_window.h"

static HWND s_lastHwnd = NULL;

HWND FindTargetWindow(POINT pt)
{
    HWND hwnd = WindowFromPoint(pt);

    if (!hwnd)
        return NULL;

    if (hwnd == g_overlay)
        return NULL;

    hwnd = GetAncestor(hwnd, GA_ROOTOWNER);

    if (!hwnd)
        return NULL;

    if (!IsWindowVisible(hwnd))
        return NULL;

    if (IsIconic(hwnd))
        return NULL;

    return hwnd;
}

void WindowFinder_Update(POINT pt)
{
    HWND hwnd = FindTargetWindow(pt);

    if (hwnd == NULL)
        return;

    //
    // 不要选中 Overlay 自己
    //
    if (hwnd == g_overlay)
        return;

    //
    // 如果窗口没变，只更新鼠标坐标即可
    //
    if (hwnd == s_lastHwnd)
    {
        g_target.mouse = pt;
        return;
    }

    s_lastHwnd = hwnd;

    RECT rc;

    if (!GetWindowRect(hwnd, &rc))
        return;

    char title[256] = {0};

    GetWindowTextA(
        hwnd,
        title,
        sizeof(title));

    g_target.hwnd = hwnd;
    g_target.rect = rc;
    g_target.mouse = pt;

    strcpy(g_target.title, title);

    printf("\n");
    printf("----------------------------------------\n");
    printf("HWND : %p\n", hwnd);
    printf("Title: %s\n", title);

    printf(
        "RECT : (%ld,%ld)-(%ld,%ld)\n",
        rc.left,
        rc.top,
        rc.right,
        rc.bottom);

    fflush(stdout);

    OverlayUpdateRect(&rc);
}

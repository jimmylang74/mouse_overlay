#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "window_finder.h"
#include "overlay_window.h"

static HWND s_lastHwnd = NULL;

void WindowFinder_Update(POINT pt)
{
    HWND hwnd = WindowFromPoint(pt);

    if (hwnd == NULL)
        return;

    //
    // 不要选中 Overlay 自己
    //
    if (hwnd == g_overlay)
        return;

    //
    // 获取真正的顶层窗口
    //
    HWND root = GetAncestor(hwnd, GA_ROOT);

    if (root == NULL)
        return;

    //
    // 如果窗口没变，只更新鼠标坐标即可
    //
    if (root == s_lastHwnd)
    {
        g_target.mouse = pt;
        return;
    }

    s_lastHwnd = root;

    RECT rc;

    if (!GetWindowRect(root, &rc))
        return;

    char title[256] = {0};

    GetWindowTextA(
        root,
        title,
        sizeof(title));

    g_target.hwnd = root;
    g_target.rect = rc;
    g_target.mouse = pt;

    strcpy(g_target.title, title);

    printf("\n");
    printf("----------------------------------------\n");
    printf("HWND : %p\n", root);
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

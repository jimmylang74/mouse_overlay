#include <windows.h>
#include <stdio.h>

#include "globals.h"
#include "mouse_hook.h"

//
// overlay_window.c 会使用它
//
POINT g_lastMousePoint = {0};

static HHOOK g_hook = NULL;

static LRESULT CALLBACK MouseProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT *ms = (MSLLHOOKSTRUCT *)lParam;

        switch (wParam)
        {
        case WM_MOUSEMOVE:

            //
            // 只保存鼠标位置
            //
            g_lastMousePoint = ms->pt;

            //
            // 防抖：
            // 鼠标持续移动时不断重启Timer
            //
            KillTimer(g_overlay, 1);

            SetTimer(
                g_overlay,
                1,
                50,
                NULL);

            break;

        case WM_LBUTTONDOWN:

            printf(
                "Left Down (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;

        case WM_LBUTTONUP:

            printf(
                "Left Up (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;

        case WM_RBUTTONDOWN:

            printf(
                "Right Down (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;

        case WM_RBUTTONUP:

            printf(
                "Right Up (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;

        case WM_MBUTTONDOWN:

            printf(
                "Middle Down (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;

        case WM_MBUTTONUP:

            printf(
                "Middle Up (%ld,%ld)\n",
                ms->pt.x,
                ms->pt.y);

            fflush(stdout);

            break;
        }
    }

    return CallNextHookEx(
        g_hook,
        nCode,
        wParam,
        lParam);
}

BOOL InstallMouseHook(void)
{
    g_hook = SetWindowsHookEx(
        WH_MOUSE_LL,
        MouseProc,
        GetModuleHandle(NULL),
        0);

    if (g_hook == NULL)
    {
        printf(
            "SetWindowsHookEx failed : %lu\n",
            GetLastError());

        return FALSE;
    }

    printf("Mouse Hook Installed.\n");

    return TRUE;
}

void RemoveMouseHook(void)
{
    if (g_hook)
    {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
}

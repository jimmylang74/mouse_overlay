#include <windows.h>
#include <stdio.h>

#include "globals.h"
#include "mouse_hook.h"
#include "overlay_window.h"
#include "window_finder.h"

//
// overlay_window.c 会使用它
//
POINT g_lastMousePoint = {0};

PENDING_CLICK g_pendingClick = {0};

static HHOOK g_hook = NULL;

static BOOL g_consumed = FALSE;

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

            if(g_debug_verbose)
            {
                printf(
                    "Left Down (%ld,%ld)\n",
                    ms->pt.x,
                    ms->pt.y);

                fflush(stdout);
            }

            break;

        case WM_LBUTTONUP:

            if(g_debug_verbose)
            {
                printf(
                    "Left Up (%ld,%ld)\n",
                    ms->pt.x,
                    ms->pt.y);

                fflush(stdout);
            }

            break;

        case WM_RBUTTONDOWN:

            g_consumed = TRUE;
            OverlayTriggerRipple(ms->pt);

            if(g_debug_verbose)
            {
                printf(
                    "Right Down (%ld,%ld)\n",
                    ms->pt.x,
                    ms->pt.y);

                fflush(stdout);
            }

            break;

        case WM_RBUTTONUP:
        {
            g_consumed = TRUE;

            HWND hwnd = FindTargetWindow(ms->pt);

            if(hwnd && hwnd != g_overlay)
            {
                RECT rc;
                if(GetWindowRect(hwnd, &rc))
                {
                    char title[256] = {0};
                    GetWindowTextA(hwnd, title, sizeof(title));

                    g_pendingClick.hwnd   = hwnd;
                    g_pendingClick.rect   = rc;
                    g_pendingClick.mouse  = ms->pt;
                    g_pendingClick.hasPending = TRUE;

                    strcpy(g_pendingClick.title, title);
                }
            }

            break;
        }

        case WM_MBUTTONDOWN:

            if(g_debug_verbose)
            {
                printf(
                    "Middle Down (%ld,%ld)\n",
                    ms->pt.x,
                    ms->pt.y);

                fflush(stdout);
            }

            break;

        case WM_MBUTTONUP:

            if(g_debug_verbose)
            {
                printf(
                    "Middle Up (%ld,%ld)\n",
                    ms->pt.x,
                    ms->pt.y);

                fflush(stdout);
            }

            break;
        }
    }

    if (g_consumed && nCode >= 0)
    {
        g_consumed = FALSE;
        return 1;
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
        if(g_debug_verbose)
        {
            printf(
                "SetWindowsHookEx failed : %lu\n",
                GetLastError());
        }

        return FALSE;
    }

    if(g_debug_verbose)
    {
        printf("Mouse Hook Installed.\n");
    }

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

void PrintPendingClick(void)
{
    if (!g_pendingClick.hasPending)
        return;

    printf(
        "{\"handle\":\"%p\",\"title\":\"%s\",\"rect\":{\"left\":%ld,\"top\":%ld,\"right\":%ld,\"bottom\":%ld},\"mouse\":{\"x\":%ld,\"y\":%ld}}\n",
        g_pendingClick.hwnd,
        g_pendingClick.title,
        g_pendingClick.rect.left,
        g_pendingClick.rect.top,
        g_pendingClick.rect.right,
        g_pendingClick.rect.bottom,
        g_pendingClick.mouse.x,
        g_pendingClick.mouse.y);

    fflush(stdout);

    g_pendingClick.hasPending = FALSE;
}

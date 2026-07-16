#include <windows.h>
#include <stdio.h>

HHOOK g_hHook = NULL;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT *ms = (MSLLHOOKSTRUCT *)lParam;

        switch (wParam)
        {
        case WM_MOUSEMOVE:
            printf("MOVE  (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_LBUTTONDOWN:
            printf("LBUTTON DOWN (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_LBUTTONUP:
            printf("LBUTTON UP   (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_RBUTTONDOWN:
            printf("RBUTTON DOWN (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_RBUTTONUP:
            printf("RBUTTON UP   (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_MBUTTONDOWN:
            printf("MBUTTON DOWN (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;

        case WM_MBUTTONUP:
            printf("MBUTTON UP   (%ld, %ld)\n", ms->pt.x, ms->pt.y);
            break;
        }

        fflush(stdout);
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

int main(void)
{
    printf("Installing mouse hook...\n");

    g_hHook = SetWindowsHookEx(
        WH_MOUSE_LL,
        MouseProc,
        GetModuleHandle(NULL),
        0);

    if (!g_hHook)
    {
        printf("SetWindowsHookEx failed: %lu\n", GetLastError());
        getchar();
        return 1;
    }

    printf("Mouse hook installed.\n");
    printf("Move the mouse or click buttons...\n\n");
    fflush(stdout);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hHook);

    return 0;
}

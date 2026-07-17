#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "mouse_hook.h"
#include "overlay_window.h"

TARGET_WINDOW g_target;

HWND g_overlay=NULL;

BOOL g_debug_verbose=FALSE;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrev,
    LPSTR lpCmd,
    int nShow)
{
    if(lpCmd && strstr(lpCmd,"--debug-verbose"))
    {
        g_debug_verbose=TRUE;
    }

    SetProcessDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if(g_debug_verbose)
    {
        AllocConsole();
        freopen("CONOUT$","w",stdout);

        printf("DPI = %u\n",GetDpiForSystem());
        UINT dpi=GetDpiForWindow(GetDesktopWindow());
        printf("Desktop DPI = %u\n",dpi);
        printf("Mouse Overlay\n");
    }

    g_overlay=CreateOverlayWindow(hInstance);

    if(g_overlay==NULL)
    {
        if(g_debug_verbose)
        {
            printf("CreateOverlayWindow failed\n");
        }
        return 1;
    }

    if(!InstallMouseHook())
    {
        if(g_debug_verbose)
        {
            printf("InstallMouseHook failed\n");
        }
        return 1;
    }

    MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveMouseHook();

    return 0;
}

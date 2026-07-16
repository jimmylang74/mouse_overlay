#include <windows.h>
#include <stdio.h>

#include "globals.h"
#include "mouse_hook.h"
#include "overlay_window.h"

TARGET_WINDOW g_target;

HWND g_overlay=NULL;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrev,
    LPSTR lpCmd,
    int nShow)
{
    AllocConsole();

    freopen("CONOUT$","w",stdout);

    printf("Mouse Overlay\n");

    g_overlay=CreateOverlayWindow(hInstance);

    if(g_overlay==NULL)
    {
        printf("CreateOverlayWindow failed\n");
        return 1;
    }

    if(!InstallMouseHook())
    {
        printf("InstallMouseHook failed\n");
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

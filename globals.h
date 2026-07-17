#ifndef GLOBALS_H
#define GLOBALS_H

#include <windows.h>

typedef struct
{
    HWND hwnd;
    RECT rect;
    POINT mouse;
    char title[256];
} TARGET_WINDOW;

extern TARGET_WINDOW g_target;

extern HWND g_overlay;

extern BOOL g_debug_verbose;

typedef struct
{
    BOOL  hasPending;
    HWND  hwnd;
    RECT  rect;
    POINT mouse;
    char  title[256];
} PENDING_CLICK;

extern PENDING_CLICK g_pendingClick;

#endif

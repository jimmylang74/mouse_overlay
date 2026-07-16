#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <windows.h>

HWND CreateOverlayWindow(HINSTANCE hInstance);

void OverlayUpdateRect(const RECT *rc);

#endif


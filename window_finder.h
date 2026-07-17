#ifndef WINDOW_FINDER_H
#define WINDOW_FINDER_H

#include <windows.h>

HWND FindTargetWindow(POINT pt);

void WindowFinder_Update(POINT pt);

#endif

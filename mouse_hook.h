#ifndef MOUSE_HOOK_H
#define MOUSE_HOOK_H

#include <windows.h>

BOOL InstallMouseHook(void);

void RemoveMouseHook(void);

void PrintPendingClick(void);

#endif

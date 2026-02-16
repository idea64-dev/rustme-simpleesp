#ifndef HOOK_H_
#define HOOK_H_

#include <Windows.h>

void hooks_install();
void hooks_remove();

LRESULT CALLBACK hook_wndproc(
    HWND, UINT, WPARAM, LPARAM
);

BOOL WINAPI hook_swapbuffers(HDC);

#endif // !HOOK_H_

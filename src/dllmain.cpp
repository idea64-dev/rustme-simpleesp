#include <iostream>
#include <Windows.h>
#include "hook.h"
#include "runtime.h"
#include <thread>
#include <chrono>
DWORD __stdcall main_thread(void*)
{
    if (!runtime_init())
    {
        FreeLibraryAndExitThread(
            GetModuleHandle(nullptr),
            0
        );
    }

    while (!GetAsyncKeyState(VK_END))
        Sleep(25);

    runtime_shutdown();

    FreeLibraryAndExitThread(
        GetModuleHandle(nullptr),
        0
    );
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hInstance);

        AllocConsole();
        FILE *pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        SetConsoleTitle(L"Console");

        CreateThread(
            0, 0,
            main_thread,
            0, 0, 0
        );
    }
    return TRUE;
}
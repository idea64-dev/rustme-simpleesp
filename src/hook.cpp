#include "hook.h"
#include "runtime.h"
#include "gui.h"
#include "hook/PGHooker.hpp"
#include <gl/GL.h>

using swap_t = BOOL(WINAPI *)(HDC);

static swap_t orig_swap = nullptr;

BOOL WINAPI hook_swapbuffers(HDC hdc)
{
    HGLRC old = wglGetCurrentContext();

    if (!g_rt.gl_init)
    {
        g_rt.ctx = wglCreateContext(hdc);
        wglMakeCurrent(hdc, g_rt.ctx);

        gui::init(g_rt.hwnd);

        g_rt.gl_init = true;
    }
    else
    {
        wglMakeCurrent(hdc, g_rt.ctx);
        gui::draw();
    }

    wglMakeCurrent(hdc, old);

    PGHooker::DisableHookForOnce(g_rt.wgl_swap);

    return orig_swap(hdc);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hook_wndproc(
    HWND hWnd,
    UINT msg,
    WPARAM wp,
    LPARAM lp)
{
    if (msg == WM_KEYDOWN &&
        wp == VK_INSERT)
    {
        gui::toggle();
    }

    if (gui::active())
    {
        if (ImGui_ImplWin32_WndProcHandler(
            hWnd, msg, wp, lp))
            return 1;
    }

    return CallWindowProcA(
        g_rt.orig_wndproc,
        hWnd, msg, wp, lp
    );
}

DWORD __stdcall wgl_reinstall(void *)
{
    while (true)
    {
        Sleep(10000);
        PGHooker::RemoveHook(g_rt.wgl_swap);

        PGHooker::CreateHook(g_rt.wgl_swap, hook_swapbuffers);
    }
    return 0;
}

void hooks_install()
{
    g_rt.orig_wndproc =
        (WNDPROC)SetWindowLongPtrA(
            g_rt.hwnd,
            GWLP_WNDPROC,
            (LONG_PTR)hook_wndproc
        );

    orig_swap = (swap_t)g_rt.wgl_swap;

    PGHooker::CreateHook(
        g_rt.wgl_swap,
        hook_swapbuffers
    );

    CreateThread(
        0, 0,
        wgl_reinstall,
        0, 0, 0
    );
}

void hooks_remove()
{
    SetWindowLongPtrA(
        g_rt.hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)g_rt.orig_wndproc
    );

    PGHooker::RemoveHook(g_rt.wgl_swap);
}

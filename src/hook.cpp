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

//#include <Windows.h>
//#include "gl/GL.h"
//#include "gui.h"
//#include "hook.h"
//#include "esp.h"
//
//using wglSwapBuffers_t = BOOL(WINAPI *)(HDC);
//
//void *original_wglSwapBuffers_addr = nullptr;
//
//static bool is_init{};
//static HWND wnd_handle{};
//static WNDPROC origin_wndproc{};
//
//static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//static BOOL WINAPI wglSwapBuffers(HDC hDc);
//
//DWORD WINAPI rehook_wgl(LPVOID)
//{
//	while (true)
//	{
//		Sleep(10000);
//
//		PGHooker::RemoveHook(original_wglSwapBuffers_addr);
//
//		//Sleep(25);
//		PGHooker::CreateHook(original_wglSwapBuffers_addr, wglSwapBuffers);
//	}
//
//	return 0;
//}
//
//bool hook::init()
//{
//	if (is_init)
//		return false;
//
//
//	wnd_handle = FindWindowA("LWJGL", nullptr);
//
//	origin_wndproc = (WNDPROC)SetWindowLongPtrW(wnd_handle, GWLP_WNDPROC, (LONG_PTR)WndProc);
//
//	PGHooker::Initialize();
//
//	original_wglSwapBuffers_addr = (void *)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
//
//	if (original_wglSwapBuffers_addr == nullptr)
//	{
//		::std::printf("[-] failed to get wglswapbuffers address");
//		return false;
//	}
//
//	PGHooker::CreateHook(original_wglSwapBuffers_addr, wglSwapBuffers);
//	esp::install();
//
//	is_init = true;
//
//	CreateThread(
//		nullptr,
//		0,
//		rehook_wgl,
//		nullptr,
//		0,
//		nullptr
//	);
//
//	return true;
//}
//
//void hook::shutdown()
//{
//	if (!is_init)
//		return;
//
//	//SetWindowLongPtrW(wnd_handle, GWLP_WNDPROC, (LONG_PTR)origin_wndproc);
//	PGHooker::RemoveHook(original_wglSwapBuffers_addr);
//
//	is_init = false;
//}
//
//bool hook::get_is_init()
//{
//	return is_init;
//}
//
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	if (gui::get_is_init())
//	{
//		if (msg == WM_KEYDOWN && wParam == VK_INSERT)
//			gui::set_do_draw(!gui::get_do_draw());
//
//		if (gui::get_do_draw() && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//			return true;
//	}
//
//	return CallWindowProcA(origin_wndproc, hWnd, msg, wParam, lParam);
//}
//static bool was_init{};
//static HGLRC new_context{};
//BOOL WINAPI wglSwapBuffers(HDC hdc)
//{
//	HGLRC origin_context{ wglGetCurrentContext() };
//
//	if (was_init == false)
//	{
//		new_context = wglCreateContext(hdc);
//		wglMakeCurrent(hdc, new_context);
//
//		GLint viewport[4];
//		glGetIntegerv(GL_VIEWPORT, viewport);
//		glViewport(0, 0, viewport[2], viewport[3]);
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		glDisable(GL_DEPTH_TEST);
//
//		gui::init(wnd_handle);
//		::std::printf("gui_init\n");
//		was_init = true;
//	}
//	else
//	{
//		wglMakeCurrent(hdc, new_context);
//		::std::printf("draw_thread\n");
//		gui::draw();
//	}
//
//	wglMakeCurrent(hdc, origin_context);
//	::std::printf("hooked wgl\n");
//	PGHooker::DisableHookForOnce(original_wglSwapBuffers_addr);
//	return ((wglSwapBuffers_t)original_wglSwapBuffers_addr)(hdc);
//}
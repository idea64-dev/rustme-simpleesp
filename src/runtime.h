#pragma once

#include <Windows.h>

struct runtime_t
{
	HWND	hwnd;
	WNDPROC orig_wndproc;

	void *wgl_swap;

	HGLRC ctx;
	bool  gl_init;

	bool  running;
	bool  initialized;
};

extern runtime_t g_rt;

bool runtime_init();
void runtime_shutdown();

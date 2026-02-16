#include "runtime.h"
#include "hook.h"
#include "gui.h"
#include "esp.h"

#include <cstdio>

runtime_t g_rt{};

bool runtime_init()
{
	if (g_rt.initialized)
		return false;

	g_rt.hwnd = FindWindowA("LWJGL", nullptr);

	if (!g_rt.hwnd)
		return false;

	PGHooker::Initialize();

	g_rt.wgl_swap =
		GetProcAddress(GetModuleHandleA("opengl32.dll"),
			"wglSwapBuffers");

	if (!g_rt.wgl_swap)
		return false;

	hooks_install();
	esp::install();

	g_rt.running		= true;
	g_rt.initialized	= true;

	return true;
}

void runtime_shutdown()
{
	if (!g_rt.initialized)
		return;

	gui::shutdown();

	g_rt.running		= false;
	g_rt.initialized	= false;
}


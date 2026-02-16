#ifndef GUI_H_
#define GUI_H_

#include <Windows.h>

struct gui_state_t
{
	bool show;
	bool esp;
};

namespace gui
{
	bool init(HWND);
	void shutdown();
	void draw();

	bool active();
	void toggle();

	extern gui_state_t state;
}

//#include <cstdio>
//
//#include <Windows.h>
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_opengl3.h"
//
//struct values_t
//{
//	bool esp_enabled;
//};
//
//namespace gui
//{
//	extern values_t *vals;
//
//	bool init(HWND wnd);
//	void shutdown();
//
//	void draw();
//
//	bool get_is_init();
//	bool get_do_draw();
//
//	void set_do_draw(bool draw);
//}

#endif
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
#endif

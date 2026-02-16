#include "gui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

gui_state_t gui::state{ true, false };

static bool inited = false;

bool gui::init(HWND wnd)
{
	if (inited)
		return false;

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(wnd);
	ImGui_ImplOpenGL3_Init();

	inited = true;
	return true;
}

void gui::shutdown()
{
    if (!inited) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    inited = false;
}

bool gui::active()
{
    return state.show;
}

void gui::toggle()
{
    state.show = !state.show;
}

void gui::draw()
{
    if (!state.show) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("RustMe");

    ImGui::Checkbox(
        "ESP",
        &state.esp
    );

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );
}

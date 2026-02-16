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


//static bool is_init{};
//static bool do_draw{ true };
//
//values_t *gui::vals = new values_t();
//
//bool gui::init(HWND wnd)
//{
//	if (is_init)
//		return false;
//
//	ImGui::CreateContext();
//	ImGuiIO &io = ImGui::GetIO(); (void)io;
//	ImGui::StyleColorsDark();
//	ImGui_ImplWin32_Init(wnd);
//	ImGui_ImplOpenGL3_Init();
//
//	is_init = true;
//
//	return true;
//}
//
//void gui::shutdown()
//{
//	if (!is_init)
//		return;
//
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplWin32_Shutdown();
//	ImGui::DestroyContext();
//
//	is_init = false;
//}
//
//void gui::draw()
//{
//	if (!do_draw)
//		return;
//
//	printf("TEST\n");
//
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplWin32_NewFrame();
//	ImGui::NewFrame();
//	ImGui::Begin("RustMe");
//	{
//		ImGui::Checkbox("ESP", &vals->esp_enabled);
//	}
//	ImGui::End();
//	ImGui::EndFrame();
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}
//
//bool gui::get_is_init()
//{
//	return is_init;
//}
//
//bool gui::get_do_draw()
//{
//	return do_draw;
//}
//
//void gui::set_do_draw(bool draw)
//{
//	do_draw = draw;
//}
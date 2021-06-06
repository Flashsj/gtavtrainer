#include "common.hpp"
#include "gui.hpp"
#include "script.hpp"
#include "base_tab.h"
#include "misc.h"
#include "features.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include <script_global.hpp>
#include "math.h"
#include <time.h>
#include <sstream>
#include <../imgui/imgui.h>
#include <../imgui/imgui_internal.h>
#include <../imgui/examples/imgui_impl_dx9.h>
#include <../imgui/examples/imgui_impl_win32.h>

namespace big
{
	void gui::dx_init()
	{
		//clear existing fonts
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		//write new font
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14.f);
		(font != NULL) ? io.FontDefault = font : io.Fonts->AddFontDefault();
		io.Fonts->Build();

		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		ImGui::StyleColorsDark();
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);

		style.WindowRounding = 0.1f;
		style.FrameRounding = 1.5f;
		style.TabRounding = 0.f;
		style.ChildRounding = 0.0f;

		colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 0.670f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 0.670f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.27f, 0.27f, 0.54f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.989f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.989f);
		colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 0.86f);
		colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);

		ImGui::GetStyle() = style;
	}

	static char* sidebar_tabs[] = { "LOCAL", "VEHICLE", "ONLINE", "SETTINGS" };
	enum { TAB_LOCAL, TAB_VEHICLE , TAB_ONLINE, TAB_SETTINGS };
	constexpr static float get_sidebar_item_width() { return 150.0f; }
	constexpr static float get_sidebar_item_height() { return  80.0f; }

	template<size_t N>
	void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
	{
		bool values[N] = { false };
		values[activetab] = true;

		for (auto i = 0; i < N; ++i)
		{
			if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }))
				activetab = i;
			if (sameline && i < N - 1)
				ImGui::SameLine();
		}
	}

	ImVec2 get_sidebar_size()
	{
		constexpr float padding = 10.0f;
		constexpr auto size_w = padding * 2.0f + get_sidebar_item_width();
		constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

		return ImVec2{ size_w, ImMax(325.0f, size_h) };
	}

	void gui::dx_on_tick()
	{
		auto& style = ImGui::GetStyle();
		const auto sidebar_size = get_sidebar_size();
		static int active_sidebar_tab = 0;

		auto size = ImVec2{ 0.0f, sidebar_size.y };

		if (ImGui::Begin("LandrySoftware", &g_gui.m_opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			{
				ImGui::BeginGroupBox("##sidebar", sidebar_size);
				render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
				ImGui::EndGroupBox();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine();

			ImGui::BeginGroupBox("##body", size);

			//clear existing fonts
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->Clear();

			switch (active_sidebar_tab)
			{

			case TAB_LOCAL:
				base_tab::render_local_tab();
				break;
			case TAB_VEHICLE:
				base_tab::render_vehicle_tab();
				break;
			case TAB_ONLINE:
				(*g_pointers->m_is_session_started ? base_tab::render_online_tab() : ImGui::Text("You must be in a session in order to use online features"));
				break;
			case TAB_SETTINGS:
				base_tab::render_settings_tab();
				break;
			}

			ImGui::EndGroupBox();

			ImGui::End();
		}
	}

	void gui::script_init() {}

	void gui::script_on_tick()
	{
		TRY_CLAUSE
		{
			if (g_gui.m_opened)
				PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
		}
		EXCEPT_CLAUSE
	}

	void gui::script_func()
	{
		g_gui.script_init();
		while (true)
		{
			g_gui.script_on_tick();
			script::get_current()->yield();
		}
	}
}

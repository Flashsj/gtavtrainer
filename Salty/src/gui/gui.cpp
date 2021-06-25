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
#include <renderer.hpp>

namespace big
{
	static char* sidebar_tabs[] = { "C", "E", "H", "F" };
	enum { TAB_LOCAL, TAB_VEHICLE, TAB_ONLINE, TAB_SETTINGS };
	constexpr static float get_sidebar_item_width() { return 150.0f; }
	constexpr static float get_sidebar_item_height() { return  79.5f; }

	bool config::save(const string file_name)
	{
		const size_t class_size = sizeof(config);
		void* buffer = malloc(class_size);

		memcpy(buffer, &g_config, class_size);

		ofstream output(file_name, ios::binary);
		if (!output.is_open())
			return false;

		output.write(static_cast<const char*>(buffer), class_size);
		output.flush();
		output.close();

		free(buffer);
		buffer = NULL;
		return true;
	}

	bool config::load(const string file_name)
	{
		ifstream input(file_name, ios::binary);
		if (!input.is_open())
			return false;

		const size_t class_size = sizeof(config);
		void* buffer = malloc(class_size);
		input.read(static_cast<char*>(buffer), class_size);
		input.close();

		memcpy(&g_config, buffer, class_size);
		free(buffer);
		buffer = NULL;

		return true;
	}

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

	void draw()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		const auto sidebar_size = get_sidebar_size();
		static int active_sidebar_tab = 0;
		auto size = ImVec2{ 0.0f, sidebar_size.y };

		if (ImGui::Begin("##body_main", &g_gui.m_opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::PushFont(tabfont);
			{
				ImGui::BeginGroupBox("##sidebar", sidebar_size);
				{ 
					style.FrameRounding = 0.f; //force tab rounding to be sharp
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.5, 1.5));
					render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
					ImGui::PopStyleVar();
				}
				ImGui::EndGroupBox();
			}
			ImGui::PopFont();

			ImGui::SameLine();

			ImGui::PushFont(mainfont);
			{ 
				ImGui::BeginGroupBox("##body_sub", size);
				{
					style.FrameRounding = 1.5f; //force rounding for everything but the tabs
					ImGui::PushItemWidth(145.f);
					{
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
					}
					ImGui::PopItemWidth();
				}
				ImGui::EndGroupBox();
			}
			ImGui::PopFont();

			ImGui::End();
		}
	}

	void gui::dx_init()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		const auto sidebar_size = get_sidebar_size();
		static int active_sidebar_tab = 0;
		auto size = ImVec2{ 0.0f, sidebar_size.y };

		ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);

		style.WindowRounding = 0.1f;
		style.TabRounding = 0.f;
		style.ChildRounding = 0.0f;

		colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.989f);
		colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);

		ImGui::GetStyle() = style;
	}

	void gui::dx_on_tick()
	{
		auto& colors = ImGui::GetStyle().Colors;

		//these are here to auto update the menu colors based on the custom menu colorpicker
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);

		draw();
	}

	void gui::script_init() {} //unused

	// to do: maybe make an enum with the names of the controls and fix Error	C2712	Cannot use __try in functions that require object unwinding	Cheat
	static vector<int> blocked_controls = { 22, 23, 75, 145, 14, 15, 16, 17, 27, 99, 115, 199, 244, 245, 246, 247, 248, 1, 2, 3, 4, 5, 6, 24, 25, 68, 69, 70, 91, 92, 106, 114, 122, 135, 142, 144, 176, 177, 257, 329, 346, 157, 158, 159, 160, 161, 162, 163, 164, 165, 26, 79 };
	void gui::script_on_tick()
	{
		TRY_CLAUSE
		{
			//if (g_gui.m_opened)
			//	PAD::DISABLE_ALL_CONTROL_ACTIONS(0);

			if (g_gui.m_opened)
			{
				for (int i = 0; i < blocked_controls.size(); i++)
				{
					CONTROLS::DISABLE_CONTROL_ACTION(0, blocked_controls.at(i), 1);
					CONTROLS::DISABLE_CONTROL_ACTION(2, blocked_controls.at(i), 1);
				}
			}
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

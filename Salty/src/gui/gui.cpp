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
#include <logger.hpp>

using namespace std;

namespace big
{
	static int selected_tab = 0;
	static vector<string> tabs = { "Local", "Vehicle", "Online", "Config" };
	static vector<int> blocked_controls = { 22, 23, 75, 145, 14, 15, 16, 17, 27, 99, 115, 199, 244, 245, 246, 247, 248, 1, 2, 3, 4, 5, 6, 24, 25, 68, 69, 70, 91, 92, 106, 114, 122, 135, 142, 144, 176, 177, 257, 329, 346, 157, 158, 159, 160, 161, 162, 163, 164, 165, 26, 79 };

	float width = 750;
	float height = 400;

	#pragma region Confiruation

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

#pragma endregion

	#pragma region Script initialization

	void gui::script_on_tick()
	{
		TRY_CLAUSE
		{
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

	void gui::script_init() {}

#pragma endregion

	void gui::style()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
		ImGui::StyleColorsDark();

		style.WindowRounding = 0.1f;
		style.TabRounding = 0.f;
		style.ChildRounding = 0.0f;

		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.98f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.98f);
	}

	void gui::draw() 
	{
		ImGui::SetNextWindowSize(ImVec2{ 750, 400 }, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2{ 100, 200 }, ImGuiCond_Once);

		if (ImGui::Begin("", &g_gui.m_opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::Text("> v0.1 pre-alpha - proof of concept");

			if (ImGui::BeginGroupBox("##body_main", ImVec2{ width - 17, height - 41 }))
			{
				ImGui::BeginColumns("##columns_main_body", 2);
				{
					ImGui::SetColumnWidth(0, 100.f);

					for (size_t i = 0; i < tabs.size(); i++)
					{
						if (ImGui::Selectable(tabs.at(i).c_str(), selected_tab == i))
							selected_tab = i;
					}

					ImGui::NextColumn();

					switch (selected_tab)
					{
					case 0:
						base_tab::render_local_tab();
						break;
					case 1:
						base_tab::render_vehicle_tab();
						break;
					case 2:
						(*g_pointers->m_is_session_started ? base_tab::render_online_tab() : ImGui::Text("You must be in a session in order to use online features"));
						break;
					case 3:
						base_tab::render_settings_tab();
						break;
					}
				}
				ImGui::EndColumns();
			}
			ImGui::EndGroupBox();

			ImGui::End();
		}
	}

	void gui::dx_on_tick()
	{
		draw();
	}

	void gui::dx_init() 
	{
		style();
	}
}

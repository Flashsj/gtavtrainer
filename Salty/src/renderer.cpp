#include "common.hpp"
#include "renderer.hpp"
#include"pointers.hpp"
#include "imgui.h"
#include "fonts.hpp"
#include <gui/gui.hpp>
#include <gui/features.hpp>
#include "../fonts.hpp"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern ImFont* tabfont;
extern ImFont* mainfont;

namespace big
{
		renderer::renderer() :

		m_dxgi_swapchain(*g_pointers->m_swapchain)
		{

		void *d3d_device{};

		if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
			m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
		else
			throw std::runtime_error("Failed to get D3D device.");

		m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());

		auto file_path = std::filesystem::path(std::getenv("appdata"));
		file_path /= "GTAVTRAINER";

		if (!std::filesystem::exists(file_path))
			std::filesystem::create_directory(file_path);
		else if (!std::filesystem::is_directory(file_path))
		{
			std::filesystem::remove(file_path);
			std::filesystem::create_directory(file_path);
		}

		file_path /= "imgui.ini";
		
		ImGuiContext* ctx = ImGui::CreateContext();

		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename = path.c_str();

		ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;

		ImGui::GetIO().Fonts->Clear();
		//m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14.f, &font_cfg); //obsolete

		//mainfont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14.f, &font_cfg);
		//(mainfont != NULL) ? ImGui::GetIO().FontDefault = mainfont : ImGui::GetIO().Fonts->AddFontDefault();
		//tabfont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Users\\cland\\Documents\\GTA Stuff\\gtavtrainer\\font.ttf", 14.f); // use binary_to_compressed_c, if you dont want to make a new vs project for this i already remade this in rotorhack and theres an example, it just puts the array in a text document in the csgo folder
		//(tabfont != NULL) ? ImGui::GetIO().FontDefault = tabfont : ImGui::GetIO().Fonts->AddFontDefault();
		//m_monospace_font = ImGui::GetIO().Fonts->AddFontDefault();

		mainfont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 14.f);
		m_monospace_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 14.f);
		m_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 14.f);
		tabfont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 16.f);

		g_gui.dx_init();
		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_renderer = nullptr;
	}

	void connectSkeleton(ImVec2 from, ImVec2 to, ImU32 col)
	{
		bool fromValid = from.x >= 0.f && from.y >= 0.f;
		bool toValid = to.x >= 0.f && to.y >= 0.f;
		if (fromValid && toValid)
			ImGui::GetBackgroundDrawList()->AddLine(from, to, col);
	}

	bool world_to_screen(Vector3 pos, ImVec2& out, uint32_t	w, uint32_t	h)
	{
		Vector3	tmp;

		float* viewMatrix = g_pointers->viewport->m_matrix;

		tmp.x = (viewMatrix[1] * pos.x) + (viewMatrix[5] * pos.y) + (viewMatrix[9] * pos.z) + viewMatrix[13];	//row 2
		tmp.y = (viewMatrix[2] * pos.x) + (viewMatrix[6] * pos.y) + (viewMatrix[10] * pos.z) + viewMatrix[14];	//row 3
		tmp.z = (viewMatrix[3] * pos.x) + (viewMatrix[7] * pos.y) + (viewMatrix[11] * pos.z) + viewMatrix[15];	//row 4

		if (tmp.z < 0.001f)
			return false;

		tmp.z = 1.0f / tmp.z;

		tmp.x *= tmp.z;
		tmp.y *= tmp.z;

		out.x = ((w / 2.f) + (.5f * tmp.x * w + 1.f)) / w;
		out.y = ((h / 2.f) - (.5f * tmp.y * h + 1.f)) / h;

		if (out.x > 1.f || out.x < 0.f || out.y > 1.f || out.y < 0.f)
			return false;
		return true;
	}

	bool renderer::GetBonePosition2(void* pThis, Vector3* BonePosOut, WORD Mask)
	{
		if (!pThis || !g_pointers->getBone2)
			return false;

		__declspec(align(16)) rage::vector4 BonePos = { 0, 0, 0, 0 };

		if (static_cast<bool>(g_pointers->getBone2(pThis, BonePos, Mask)))
		{
			BonePosOut->x = BonePos.x;
			BonePosOut->y = BonePos.y;
			BonePosOut->z = BonePos.z;
			return false;
		}

		return false;
	}

	void storeSkeleton(void* ped, int bone, ImVec2 screen_size, ImVec2& out)
	{
		if (!ped)
			return;

		Vector3 vec{};
		renderer::GetBonePosition2(ped, &vec, bone);

		world_to_screen(vec, out, screen_size.x, screen_size.y);
		out.x *= screen_size.x;
		out.y *= screen_size.y;
	} 

	void renderer::on_present()
	{
		if (g_gui.m_opened)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_config.esp.enable_d3d_esp && g_running) // this is a lot better than before but sometimes the esp still glitches out, rarely tho
		{
			for (int i = 0; i < 32; i++)
			{
				if (!features::players[i].ped)
					continue;

				if (!features::players[i].info)
					continue;

				if (!features::players[i].info->ped)
					continue;

				if (!features::players[i].exists)
					continue;

				if (!g_config.Lfeatures_invisible && i == features::localIndex) // unloaded the cheat to recompile, tab into vs and my game fucking closes itself 10 seconds later i fucking love this game
					continue;													// to do: figure out edit and continue and the performance profiler since i can attach a debugger now

				if (features::players[i].distanceToLocal > g_config.esp.render_distance)
					continue;

				if (g_config.ESPfeatures_visible && !features::players[i].visible)
					continue;

				//add visible check for skeleton DONE

				int maxHealth = features::players[i].maxHealth - 100; // to do: fix the colorpicker, im too lazy rn
				int health = std::clamp(features::players[i].health - 100, 0, maxHealth);
				Vector3 healthColor = features::FromHSB(std::clamp((float)(health) / (float)(maxHealth * 3.6f), 0.f, 0.277777777778f), 1.f, 1.f);
				if (features::players[i].invincible) 
					{ healthColor.x = 255; healthColor.y = 255; healthColor.z = 255; }
				auto color = ImGui::ColorConvertFloat4ToU32(ImVec4(healthColor.x / 255.f, healthColor.y / 255.f, healthColor.z / 255.f, 1.f));

				auto screen_size = ImGui::GetIO().DisplaySize;

				if (g_config.esp.skeleton_enabled)
				{
					storeSkeleton(features::players[i].info->ped, SKEL_Head, screen_size, features::players[i].skeleton.head); // to do: redo this with less autism next time (for loop)
					storeSkeleton(features::players[i].info->ped, SKEL_Neck_1, screen_size, features::players[i].skeleton.neck1);
					storeSkeleton(features::players[i].info->ped, SKEL_Spine0, screen_size, features::players[i].skeleton.spine0);
					storeSkeleton(features::players[i].info->ped, SKEL_Spine1, screen_size, features::players[i].skeleton.spine1);
					storeSkeleton(features::players[i].info->ped, SKEL_Spine2, screen_size, features::players[i].skeleton.spine2);
					storeSkeleton(features::players[i].info->ped, SKEL_Spine3, screen_size, features::players[i].skeleton.spine3);
					storeSkeleton(features::players[i].info->ped, SKEL_Pelvis, screen_size, features::players[i].skeleton.pelvis);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Thigh, screen_size, features::players[i].skeleton.SKEL_L_Thigh);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Calf, screen_size, features::players[i].skeleton.SKEL_L_Calf);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Foot, screen_size, features::players[i].skeleton.SKEL_L_Foot);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Toe0, screen_size, features::players[i].skeleton.SKEL_L_Toe0);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Thigh, screen_size, features::players[i].skeleton.SKEL_R_Thigh);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Calf, screen_size, features::players[i].skeleton.SKEL_R_Calf);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Foot, screen_size, features::players[i].skeleton.SKEL_R_Foot);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Toe0, screen_size, features::players[i].skeleton.SKEL_R_Toe0);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Clavicle, screen_size, features::players[i].skeleton.SKEL_L_Clavicle);
					storeSkeleton(features::players[i].info->ped, SKEL_L_UpperArm, screen_size, features::players[i].skeleton.SKEL_L_UpperArm);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Forearm, screen_size, features::players[i].skeleton.SKEL_L_Forearm);
					storeSkeleton(features::players[i].info->ped, SKEL_L_Hand, screen_size, features::players[i].skeleton.SKEL_L_Hand);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Clavicle, screen_size, features::players[i].skeleton.SKEL_R_Clavicle);
					storeSkeleton(features::players[i].info->ped, SKEL_R_UpperArm, screen_size, features::players[i].skeleton.SKEL_R_UpperArm);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Forearm, screen_size, features::players[i].skeleton.SKEL_R_Forearm);
					storeSkeleton(features::players[i].info->ped, SKEL_R_Hand, screen_size, features::players[i].skeleton.SKEL_R_Hand);

					connectSkeleton(features::players[i].skeleton.head, features::players[i].skeleton.neck1, color);
					connectSkeleton(features::players[i].skeleton.neck1, features::players[i].skeleton.spine3, color);
					connectSkeleton(features::players[i].skeleton.spine3, features::players[i].skeleton.spine2, color);
					connectSkeleton(features::players[i].skeleton.spine2, features::players[i].skeleton.spine1, color);
					connectSkeleton(features::players[i].skeleton.spine1, features::players[i].skeleton.spine0, color);
					connectSkeleton(features::players[i].skeleton.spine0, features::players[i].skeleton.pelvis, color);
					connectSkeleton(features::players[i].skeleton.pelvis, features::players[i].skeleton.SKEL_L_Thigh, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_Thigh, features::players[i].skeleton.SKEL_L_Calf, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_Calf, features::players[i].skeleton.SKEL_L_Foot, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_Foot, features::players[i].skeleton.SKEL_L_Toe0, color);
					connectSkeleton(features::players[i].skeleton.pelvis, features::players[i].skeleton.SKEL_R_Thigh, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_Thigh, features::players[i].skeleton.SKEL_R_Calf, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_Calf, features::players[i].skeleton.SKEL_R_Foot, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_Foot, features::players[i].skeleton.SKEL_R_Toe0, color);
					connectSkeleton(features::players[i].skeleton.neck1, features::players[i].skeleton.SKEL_L_Clavicle, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_Clavicle, features::players[i].skeleton.SKEL_L_UpperArm, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_UpperArm, features::players[i].skeleton.SKEL_L_Forearm, color);
					connectSkeleton(features::players[i].skeleton.SKEL_L_Forearm, features::players[i].skeleton.SKEL_L_Hand, color);
					connectSkeleton(features::players[i].skeleton.neck1, features::players[i].skeleton.SKEL_R_Clavicle, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_Clavicle, features::players[i].skeleton.SKEL_R_UpperArm, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_UpperArm, features::players[i].skeleton.SKEL_R_Forearm, color);
					connectSkeleton(features::players[i].skeleton.SKEL_R_Forearm, features::players[i].skeleton.SKEL_R_Hand, color);
				}

				if (i == features::localIndex)
					continue;

				if (g_config.ESPfeatures_name)
				{
					//visible only is not accounted for. enabling it still causes the name to be visible across the map.

					// to do: outline or maybe a drop shadow on the name esp font
					storeSkeleton(features::players[i].info->ped, SKEL_ROOT, screen_size, features::players[i].skeleton.name);
					features::players[i].skeleton.name.x -= ImGui::CalcTextSize(features::players[i].name).x / 2;

					ImVec2 pos = features::players[i].skeleton.name;
					//ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x + 1, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)), features::players[i].name); // ghetto bold
					//ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x - 1, pos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)), features::players[i].name);
					//ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x + 1, pos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)), features::players[i].name);
					//ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x - 1, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)), features::players[i].name);

					// i actually really like how this looks
					ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)), features::players[i].name); // ghetto drop shadow
					ImGui::GetBackgroundDrawList()->AddText(features::players[i].skeleton.name, color, features::players[i].name);
				}
			}
		}
		
		//logger not intended for any sort of release, it is for debugging purposes only

#ifdef _DEBUG
		for (size_t i = 0; i < logs.size(); i++)
		{
			if (!features::inSession || !features::sessionActive)
			{
				logs.clear();
				break;
			}

			if (logs.at(i).time < features::network_time)
			{
				logs.erase(logs.begin() + i);
				continue;
			}

			if (i >= g_config.log_limit)
				continue;

			ImGui::GetBackgroundDrawList()->AddText({ 8.f, 5.f + (i * 15.f) }, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f)), logs.at(i).log.c_str());
		}
#endif

		if (g_gui.m_opened)
			g_gui.dx_on_tick();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void renderer::pre_reset() { ImGui_ImplDX11_InvalidateDeviceObjects(); }

	void renderer::post_reset() { ImGui_ImplDX11_CreateDeviceObjects(); }

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto& style = ImGui::GetStyle();

		if ((msg == WM_KEYUP) && (wparam == VK_INSERT || wparam == VK_DELETE))
		{
			//Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui.m_opened)
				GetCursorPos(&cursor_coords);
			else if (cursor_coords.x + cursor_coords.y != 0)
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			
			g_gui.m_opened ^= true;
		}
			
		if (msg == WM_KEYUP && wparam == VK_END)
			g_running = false;

		if (g_gui.m_opened)
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}
}

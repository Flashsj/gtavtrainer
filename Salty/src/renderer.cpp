#include "common.hpp"
#include "renderer.hpp"
#include"pointers.hpp"
#include "imgui.h"
#include "fonts.hpp"
#include "gui.hpp"

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

		mainfont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14.f, &font_cfg);
		(mainfont != NULL) ? ImGui::GetIO().FontDefault = mainfont : ImGui::GetIO().Fonts->AddFontDefault();
		tabfont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\GTAVTRAINER\\Astriumtabs2.ttf", 14.f);
		(tabfont != NULL) ? ImGui::GetIO().FontDefault = tabfont : ImGui::GetIO().Fonts->AddFontDefault();
		m_monospace_font = ImGui::GetIO().Fonts->AddFontDefault();

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

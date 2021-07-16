#pragma once
#include <string>
#include <vector>
#include "natives.hpp"

namespace big
{
	class renderer
	{
	public:
		explicit renderer();
		~renderer();

		static bool GetBonePosition2(void* pThis, Vector3* BonePosOut, WORD Mask);

		void on_present();

		void pre_reset();
		void post_reset();

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		ImFont *m_font;
		ImFont *m_monospace_font;
	private:
		comptr<IDXGISwapChain> m_dxgi_swapchain;
		comptr<ID3D11Device> m_d3d_device;
		comptr<ID3D11DeviceContext> m_d3d_device_context;
	};

	inline ImFont *tabfont;
	inline ImFont* mainfont;
	inline renderer *g_renderer{};

	struct log_t
	{ 
		std::string log;
		int time;
	}; 

	inline std::vector<log_t> logs; // testing purposes 
}

#pragma once

namespace big
{
	class gui
	{
	public:
		void dx_init();
		void dx_on_tick();
		void script_init();
		void script_on_tick();
		static void script_func();

		template<size_t N>
		void render_tab(char* (&names)[N], int& activetab, float w, float h, bool sameline);

		bool m_opened{};
	};

	inline gui g_gui;
}

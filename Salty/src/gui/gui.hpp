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

	class config
	{
	public:

		bool Lfeatures_godmode = false;
		bool Lfeatures_noragdoll = false;
		bool Lfeatures_neverwanted = false;
		bool Lfeatures_offradar = false;
		bool Lfeatures_invisible = false;
		bool Lfeatures_teleportwp = false;
		bool Lfeatures_noclip = false;
		bool Lfeatures_nophone = false;
		bool Lfeatures_nomental = false;
		bool Lfeatures_fastrun = false;

		bool Wfeatures_infammo = false;
		bool Wfeatures_autoshoot = false; 
		bool Wfeatures_instakill = false;
		bool Wfeatures_addweapons = false;

		bool ESPfeatures_health = false;
		bool ESPfeatures_visible = false;
		bool ESPfeatures_box = false;
		bool ESPfeatures_name = false;
		bool ESPfeatures_distance = false;
		bool ESPfeatures_marker = false;
		bool ESPfeatures_snapline = false;
		struct // i fucking hate your letter shit, heres an idea if you want to redo this shit
		{
			bool enable_d3d_esp = false;
			bool skeleton_enabled = false;
			float render_distance = 3000.f;
		}esp;

		bool Pfeatures_teleport = false;
		bool Pfeatures_kickfromveh = false;
		bool Pfeatures_kick = false;
		bool Pfeatures_crash = false;
		bool Pfeatures_crashall = false;
		bool Pfeatures_kickall = false;

		bool Ofeatures_skipcutscene = false;
		bool auto_host_kick = false;

		bool Vfeatures_spawngodmode = false;
		bool Vfeatures_requestentity = false;
		bool Vfeatures_spawninveh = false;
		bool Vfeatures_randomizeveh = false;
		bool Vfeatures_randomizecol = false;
		bool Vfeatures_spawnupgraded = false;
		bool Vfeatures_autoupgrade = false;
		bool Vfeatures_autoclean = false;
		bool Vfeatures_hornboost = false;
		bool Vfeatures_godmode = false;
		struct
		{
			bool seatbelt = false;
		}vehicle;

		float Vfeatures_vcol[3] = { .624f, .792f, .169f };
		float ESPfeatures_boxcol[3] = { .624f, .792f, .169f };
		float ESPfeatures_namecol[3] = { .624f, .792f, .169f };
		float ESPfeatures_snapcol[3] = { .624f, .792f, .169f };
		float ESPfeatures_markercol[3] = { .624f, .792f, .169f };
		float menucolor[3] = { .624f, .792f, .169f };

		bool save(const std::string file_name);
		bool load(const std::string file_name);
	};

	inline gui g_gui;
	inline config g_config;
}

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
		void draw();
		void style();		
		void style_async();
		bool m_opened{};
		std::string title;
	};

	class config
	{
	public:
		struct
		{
			bool godmode = false;
			bool ragdoll = false;
			bool neverwanted = false;
			bool fastrun = false;
			bool offradar = false;
			bool invisible = false;
			bool nophone = false;
			bool nomental = false;
			bool noclip = false;
			bool teleportwp = false;
		}localped;

		struct
		{
			bool infammo = false;
			bool triggerbot = false;
			bool instakill = false;
			bool addweapons = false;
		}weapons;

		struct
		{
			bool health = false;
			bool visible = false;
			bool box = false;
			bool name = false;
			bool marker = false;
			bool snapline = false;
			bool skeleton = false;
			bool distance = false;
			float render_distance = 3000.f;
		}esp;

		struct
		{
			bool teleport = false;
			bool kickfromvehicle = false;
			bool kick = false;
			bool kickall = false;
			bool crash = false;
			bool crashall = false;
			bool ahk = false; //auto host kick
		}players;

		struct
		{
			bool seatbelt = false;

			bool godmode = false;
			bool autorepair = false;
			bool invisible = false;
			bool phandling = false;
			bool hornboost = false;
			bool upgrade = false;

			bool requestentity = false;
			bool spawnggodmode = false;
			bool spawninvehicle = false;
			bool spawnupgraded = false;
			bool randomizecolor = false;
			bool randomizevehicle = false;

		}vehicle;

		struct
		{
			bool enabled = false;
			int log_limit = 6;
		}debug;

		float Vfeatures_vcol[3] = { 0.374f, 0.196f, 0.879f };
		float ESPfeatures_color[3] = { 0.374f, 0.196f, 0.879f };
		float menucolor[3] = { 0.374f, 0.196f, 0.879f };

		bool save(const std::string file_name);
		bool load(const std::string file_name);
	};

	inline gui g_gui;
	inline config g_config;
}

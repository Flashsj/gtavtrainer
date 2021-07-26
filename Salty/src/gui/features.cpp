#include "common.hpp"
#include "features.hpp"
#include "script.hpp"
#include "natives.hpp"
#include "gui/misc.h"
#include "gui.hpp"
#include "pointers.hpp"
#include <script_global.hpp>
#include "renderer.hpp"

using namespace rage;

namespace big::features
{
	bool injected = false;

	// why do you have this shit here when theres g_config
	bool protection = true; //crash protection
	bool features_kickprotection = true; //kick protection
	bool features_gameeventprotection = true; //game event protection
	bool features_maleventprotection = false; //malicious event protection
	bool god_mode = true;

	volatile Player player = 0;
	volatile Ped ped = 0;
	volatile Player _cam = 0;

	volatile uint64_t sync = 0;
	volatile uint64_t network = 0;
	volatile uint64_t script = 0;
	volatile uint64_t script2 = 0;

	std::list<std::string> user;
	std::mutex mutex;

	bool first = true;

	const bool LOG_LOG = false;

	enum class logtype { LOG_NONE, LOG_INFO, LOG_WARN, LOG_ERROR, };

	void log_map(std::string str, logtype type)
	{
		std::string a;

		switch (type)
		{
		case logtype::LOG_NONE:
			a = "~g~";
			break;
		case logtype::LOG_INFO:
			a = "~m~[~f~INFO~m~]~w~";
			break;
		case logtype::LOG_WARN:
			a = "~m~[~r~WARN~m~]~w~";
			break;
		case logtype::LOG_ERROR:
			a = "~m~[~y~ERROR~m~]~w~";
			break;
		}

		str = (fmt::format("{} {}", a, str));

		QUEUE_JOB_BEGIN_CLAUSE(str)
		{
			UI::SET_TEXT_OUTLINE();
			UI::_SET_NOTIFICATION_TEXT_ENTRY((char*)"STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(str.c_str());
			UI::_DRAW_NOTIFICATION(FALSE, FALSE);

		} QUEUE_JOB_END_CLAUSE
	}

	void show_bye() {}

	void show_stat()
	{
		if (first)
		{
			log_map("Successfully injected", logtype::LOG_INFO);
			first = false;
		}
		else
			char log[256];
	}

	void add_user(std::string _user)
	{
		std::unique_lock<std::mutex> mlock(mutex);
		user.push_back(_user);
	}

	void show_user() //removed, obsolete
	{
		std::unique_lock<std::mutex> mlock(mutex);

		if (!user.empty())
		{
			char log[256];
			std::string _user = user.front();
			user.pop_front();
		}
	}

	#pragma region TO BE RELOCATED

	rage::CNetGamePlayer* getNetGamePlayer(int player) { return reinterpret_cast<rage::CNetGamePlayer*>(g_pointers->m_net_player(player)); }

	double wDegreeToRadian(double n) { return n * 0.017453292519943295; }

	Vector3 wRotationToDirection(Vector3 rot)
	{
		double num = wDegreeToRadian(rot.z);
		double num2 = wDegreeToRadian(rot.x);
		double val = cos(num2);
		double num3 = abs(val);
		rot.x = (float)(-(float)sin(num) * num3);
		rot.y = (float)(cos(num) * num3);
		rot.z = (float)sin(num2);
		return rot;
	}

	Vector3 waddVector(Vector3 vector, Vector3 vector2)
	{
		vector.x += vector2.x;
		vector.y += vector2.y;
		vector.z += vector2.z;
		return vector;
	}

	Vector3 wmultiplyVector(Vector3 vector, float inc) //test
	{
		vector.x *= inc;
		vector.y *= inc;
		vector.z *= inc;
		return vector;
	}

	float deg_to_rad(float deg) { return deg * 3.141592653589793f / 180.f; }

	Vector3 transformRotToDir(Vector3 nig)
	{
		double	a = deg_to_rad(nig.x), b = deg_to_rad(nig.z), c = cos(a);
		nig.x = (float)-(c * sin(b));
		nig.y = (float)(c * cos(b));
		nig.z = (float)sin(a);

		return nig;
	}

	Vector3	get_coords_infront_of_coords(Vector3 pos, Vector3 rot, float dist)
	{
		Vector3 transformed = transformRotToDir(rot);

		transformed.x *= dist;
		transformed.y *= dist;
		transformed.z *= dist;

		transformed.x += pos.x;
		transformed.y += pos.y;
		transformed.z += pos.z;

		return transformed;
	}

	Vector3 get_coords_above_coords(Vector3 pos, float dist)
	{
		pos.z += dist;
		return pos;
	}

	Vector3 FromHSB(float hue, float saturation, float brightness) // put this shit somewhere else
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));
		if (h < 1)
			return Vector3((brightness * 255), (t * 255), (p * 255));
		else if (h < 2)
			return Vector3((q * 255), (brightness * 255), (p * 255));
		else if (h < 3)
			return Vector3((p * 255), (brightness * 255), (t * 255));
		else if (h < 4)
			return Vector3((p * 255), (q * 255), (brightness * 255));
		else if (h < 5)
			return Vector3((t * 255), (p * 255), (brightness * 255));
		else
			return Vector3((brightness * 255), (p * 255), (q * 255));

	}

	Vector3 find_mission_blip() 
	{
		bool waypoint = false;
		static Vector3 zero;
		Vector3 coords;
		bool blipFound = false;
		int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
		for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
		{
			switch (UI::GET_BLIP_INFO_ID_TYPE(i))
			{
			default:
				log_map("", logtype::LOG_ERROR);
				coords = UI::GET_BLIP_INFO_ID_COORD(i);
				blipFound = true;
				
				if (blipFound)
				{
					waypoint = true;
					return coords;
				}
			}
		}

		return zero;
		
	}

	Vector3 find_blip()
	{
		bool waypoint = false;
		static Vector3 zero;
		Vector3 coords;
		bool blipFound = false;
		int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
		for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
		{
			if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
			{
				coords = UI::GET_BLIP_INFO_ID_COORD(i);
				blipFound = true;
				break;
			}
		}
		
		if (blipFound)
		{
			waypoint = true;
			return coords;
		}
		else
			waypoint = false;
		return zero;
	}

	Hash load(const char* name)
	{
		Hash hash = MISC::GET_HASH_KEY(name);
		STREAMING::REQUEST_MODEL(hash);
		while (!STREAMING::HAS_MODEL_LOADED(hash))
			script::get_current()->yield();
		return hash;
	}

	void RequestControlOfid(Entity netid)
	{
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(netid))
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netid);
			script::get_current()->yield();
		}
	}

	void RequestControlOfEnt(Entity entity)
	{
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
			script::get_current()->yield();
		}

		if (NETWORK::NETWORK_IS_SESSION_STARTED())
		{
			int netID = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
			RequestControlOfid(netID);
			NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, 1);
		}
	}

	bool isPlayerFriend(int player)
	{
		int handle[76];
		NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &handle[0], 13);

		if (NETWORK::NETWORK_IS_HANDLE_VALID(&handle[0], 13))
			return NETWORK::NETWORK_IS_FRIEND(&handle[0]) == 1;
		else
			return false;
	}

	bool features::owns_veh(Ped _ped) //move somewhere else
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(_ped, player);
		if (ENTITY::DOES_ENTITY_EXIST(vehicle) || ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle) && VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, -1) == _ped)
				return true;
		}
		return false;
	}

	void fireBullet(Vector3 startCoords, Vector3 target, bool instakill = false)
	{
		if (g_config.Wfeatures_instakill)
		{
			for (int i = 0; i < 6; i++)
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startCoords.x, startCoords.y, startCoords.z, target.x, target.y, target.z, 250, true, GAMEPLAY::GET_HASH_KEY("WEAPON_REMOTESNIPER"), ped, true, true, -1.0f);
		}
		else
			PED::SET_PED_SHOOTS_AT_COORD(ped, target.x, target.y, target.z, true);
	}

	void kick(int player)
	{
		LOG(INFO) << "attempting to kick " << player;

		if (NETWORK::NETWORK_IS_HOST())
		{
			NETWORK::NETWORK_SESSION_KICK_PLAYER(player);
			LOG(INFO) << "host kicked " << player;
			return;
		}
		

		auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player), 0);
		static std::vector< std::int64_t> rotor = { -1726396442, 154008137, 428882541, -1714354434 };

		// all of this shit was pasted from 2take1 luas, u can find them on github if u want to find some more or find updated ones
		std::int64_t args1[3] = { -435067392, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		std::int64_t args2[24] = { 1070934291, player, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10 };
		std::int64_t args3[4] = { -1729804184, player, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(-2147483647, 2147483647), player };
		std::int64_t args4[3] = { 1428412924, player, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(-2147483647, 2147483647) };
		std::int64_t args5[6] = { 823645419, player, -1, -1, -1, -1 };
		std::int64_t args6[4] = { -442306200, player, -1, 0 };
		std::int64_t args7[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		std::int64_t args8[17] = { -922075519, player, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };
		std::int64_t args9[11] = { -1975590661, 84857178, 61749268, -80053711, -78045655, 56341553, -78686524, -46044922, -22412109, 29388428, -56335450 };
		std::int64_t args10[11] = { -1975590661, player, (int)pos.x, (int)pos.y, (int)pos.z, 0, 0, 2147483647, 0, *script_global(1590682).at(player, 883).at(99).at(28).as<int*>(), 1 };
		std::int64_t args11[11] = { -1975590661, player, (int)pos.x, (int)pos.y, (int)pos.z, 0, 0, 1000, 0, *script_global(1590682).at(player, 883).at(99).at(28).as<int*>(), 1 };
		std::int64_t args12[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 };
		std::int64_t args13[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		std::int64_t args14[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 };
		std::int64_t args15[3] = { 0xE6116600, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		std::int64_t args16[6] = { 0xF5CB92DB, 0, 0, 46190868, 0, 2 };
		std::int64_t args17[6] = { 0xF5CB92DB,  46190868, 0, 46190868, 46190868, 2 };
		std::int64_t args18[8] = { 0xF5CB92DB, 1337, -1, 1, 1, 0, 0, 0 };
		std::int64_t args19[9] = { 0xF5CB92DB, player, 1337, -1, 1, 1, 0, 0, 0 };


		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args1, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args2, 24, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args3, 4, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args4, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args5, 6, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args6, 4, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args8, 17, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args9, 11, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args10, 11, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args11, 11, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args12, 7, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args13, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args14, 7, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args15, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args16, 6, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args17, 6, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args18, 8, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args19, 9, 1 << player);

		for (int i = 0; i < rotor.size(); i++)
		{
			std::int64_t args7[14] = { -1949011582, player, rotor.at(i), i, 1, -10, -10, -10, -10, -10, player, -10, -10, -10 };
			SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 4, 1 << player);
		}

		//disturbed / requiem events
		static std::vector<std::int64_t> kicks = { -1559754940, 2017765964, 324865135, -1212832151, -1890951223, 1302185744, 639032041, 665709549 };

		for (int i = 0; i < kicks.size(); i++)
		{
			std::int64_t args[2] = { kicks.at(i), player };
			SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 2, 1 << player);
		}

		LOG(INFO) << "kicked " << player;
	}

	void scriptCrash(int player) // ive noticed that the events with the // ? next to them arent in freemode.c, and if i comment them out and script crash someone they still crash
	{
		std::int64_t args[7] = { 3317451851, -72614, 63007, 59027, -12012, -26996, 33399 };
		std::int64_t args2[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 };
		std::int64_t args3[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() }; // ?
		std::int64_t args4[3] = { 3859899904, player, *script_global(1630317).at(player, 595).at(506).as<int*>() }; // ?
		std::int64_t args5[6] = { -977515445, -1, 500000, 849451549, -1, -1 };
		std::int64_t args6[6] = { 767605081, -1, 500000, 849451549, -1, -1 };
		std::int64_t args7[5] = { -1949011582, -1139568479, -1, 1, 100099 };
		std::int64_t args8[16] = { -2122716210, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 }; // only sending this instantly closes your game
		std::int64_t args9[20] = { -922075519, -1, -1, -1, -1, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 }; // only sending this event does nothing
		std::int64_t args10[16] = { -1975590661, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 };  // only sending this event does nothing

		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 7, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args2, 7, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args3, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args4, 3, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args5, 6, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args6, 6, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 5, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args8, 16, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args9, 20, 1 << player);
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args10, 16, 1 << player);
	}

#pragma endregion

	void features::kickFunc()
	{
		while (true)
		{
			if (g_config.Pfeatures_kick)
			{
				kick(selectedPlayer);
				log_map(fmt::format("Attempting to kick player ~r~{}", features::players[features::selectedPlayer].name), logtype::LOG_WARN);
				g_config.Pfeatures_kick = false;
			}

			if (g_config.Pfeatures_crash)
			{
				log_map(fmt::format("Attempting to crash player ~r~{}", features::players[features::selectedPlayer].name), logtype::LOG_WARN);
				scriptCrash(selectedPlayer);
				g_config.Pfeatures_crash = false;
			}

			if (g_config.Pfeatures_kickfromveh)
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer), FALSE))
				{
					AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));
					AI::CLEAR_PED_TASKS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));
					AI::CLEAR_PED_SECONDARY_TASK(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));

					std::int64_t args[2] = { -1333236192, features::selectedPlayer };
					SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 2, 1 << features::selectedPlayer);

					log_map(fmt::format("Removing ~g~{}~w~ from their vehicle", features::players[features::selectedPlayer].name), logtype::LOG_INFO);
				}
				else
					log_map(fmt::format("~g~{}~w~ is not operating a vehicle", features::players[features::selectedPlayer].name), logtype::LOG_ERROR);

				g_config.Pfeatures_kickfromveh = false;
			}

			if (g_config.Pfeatures_kickall)
			{
				log_map("Attempting to kick ~r~all players", logtype::LOG_WARN);

				for (int i = 0; i < 32; i++)
				{
					if (i == player)
						continue;

					if (!PLAYER::GET_PLAYER_PED(i))
						continue;

					if (features::players[i].isfriend)
						continue;

					kick(i);

					script::get_current()->yield();
				}
				g_config.Pfeatures_kickall = false;
			}

			if (g_config.Pfeatures_crashall)
			{
				log_map("Attempting to desktop ~r~all players", logtype::LOG_WARN);

				for (int i = 0; i < 32; i++)
				{
					if (i == player)
						continue;

					if (!PLAYER::GET_PLAYER_PED(i))
						continue;

					if (features::players[i].isfriend)
						continue;

					scriptCrash(i);

					script::get_current()->yield();
				}
				g_config.Pfeatures_crashall = false;
			}

			if (g_config.auto_host_kick)
			{
				if (features::isHost)
				{
					for (int i = 0; i < 32; i++)
					{
						if (i == player)
							continue;

						if (!PLAYER::GET_PLAYER_PED(i))
							continue;

						if (features::players[i].isfriend)
							continue;

						log_map(fmt::format("auto kicking {}", features::players[i].name), logtype::LOG_INFO);
						kick(i);
						script::get_current()->yield();
					}
				}
				else
					g_config.auto_host_kick = false;
			}

			script::get_current()->yield();
		}
	}

	void features::spawnvehicle(const char* name)
	{
		//need to add delete previous vehicle

		QUEUE_JOB_BEGIN_CLAUSE(name)
		{
			Hash hash_vehicle;

			auto pos = ENTITY::GET_ENTITY_COORDS(big::features::ped, TRUE);
			auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(big::features::ped);
			auto heading = ENTITY::GET_ENTITY_HEADING(big::features::ped);

			pos.x += 10 * forward.x;
			pos.y += 10 * forward.y;

			MISC::GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z, &pos.z, FALSE, FALSE);

			int arrSize = sizeof(features::vehicleModels) / sizeof(features::vehicleModels[0]); //TODO: fix, spawns the same random vehicle
			int RandIndex = rand() % arrSize;
			static const char* randomModel = features::vehicleModels[RandIndex];

			g_config.Vfeatures_randomizeveh ? hash_vehicle = load(randomModel) : hash_vehicle = load(name);

			//the model spawn bypass needs to be updated
			//*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x9090; 
			auto veh = VEHICLE::CREATE_VEHICLE(hash_vehicle, pos.x, pos.y, pos.z + 3, heading + 90.0f, TRUE, TRUE, FALSE);
			//*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x0574;

			NETWORK::NETWORK_FADE_IN_ENTITY(veh, TRUE);

			if (g_config.Vfeatures_randomizecol)
			{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_MOD_COLOR_1(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), 0);
				VEHICLE::SET_VEHICLE_MOD_COLOR_2(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
			}
			else
			{
				float a, b, c;
				a = g_config.Vfeatures_vcol[0] * 255, b = g_config.Vfeatures_vcol[1] * 255, c = g_config.Vfeatures_vcol[2] * 255;
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, a, b, c);
				VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, a, b, c);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, a, b, c);
			}

			if (g_config.Vfeatures_spawninveh)
			{
				for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(veh); i++)
				{
					if (VEHICLE::IS_VEHICLE_SEAT_FREE(veh, i))
					{
						RequestControlOfEnt(veh);
						PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, i);
						break;
					}
				}
			}

			if (g_config.Vfeatures_spawngodmode)
			{
				ENTITY::SET_ENTITY_INVINCIBLE(veh, TRUE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(veh, FALSE);
				ENTITY::SET_ENTITY_PROOFS(veh, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(veh, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, FALSE);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(veh, FALSE);
				VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, 1000.0f);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(veh, FALSE);
				VEHICLE::SET_VEHICLE_CAN_BE_TARGETTED(veh, FALSE);
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, FALSE);
				if (VEHICLE::IS_THIS_MODEL_A_PLANE(hash_vehicle))
					VEHICLE::SET_PLANE_TURBULENCE_MULTIPLIER(veh, 0.0f);
			}

			if (g_config.Vfeatures_spawnupgraded)
			{
				VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

				for (int i = 0; i < 48; i++) //49, 50 = livery
				{
					VEHICLE::TOGGLE_VEHICLE_MOD(veh, i, TRUE);
					VEHICLE::SET_VEHICLE_MOD(veh, i, VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, FALSE);
				}

				for (int i = 0; i < 8; i++)
					VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, 1);
			}

			RequestControlOfEnt(veh);

		} QUEUE_JOB_END_CLAUSE
	}

	void features_local()
	{
		//Invincibility / godmode

		if (g_config.Lfeatures_godmode)
		{
			PED::CLEAR_PED_BLOOD_DAMAGE(ped);
			PED::CLEAR_PED_DECORATIONS(ped);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, FALSE);
			ENTITY::SET_ENTITY_INVINCIBLE(ped, TRUE);
			ENTITY::SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(ped, TRUE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped, FALSE);
			ENTITY::SET_ENTITY_PROOFS(ped, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
			PLAYER::SET_PLAYER_INVINCIBLE(player, TRUE);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(player, 10);
			ENTITY::SET_ENTITY_HEALTH(ped, ENTITY::GET_ENTITY_MAX_HEALTH(ped));
		}
		else
		{
			ENTITY::SET_ENTITY_INVINCIBLE(ped, FALSE);
			ENTITY::SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(ped, FALSE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped, TRUE);
			ENTITY::SET_ENTITY_PROOFS(ped, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
			PLAYER::SET_PLAYER_INVINCIBLE(player, FALSE);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(player, 1);
		}

		//No ragdoll

		if (g_config.Lfeatures_noragdoll)
		{
			PED::SET_PED_CAN_RAGDOLL(ped, FALSE);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, FALSE);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, FALSE);
		}
		else
		{
			PED::SET_PED_CAN_RAGDOLL(ped, TRUE);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, TRUE);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, TRUE);
		}

		//Never wanted / no police

		if (g_config.Lfeatures_neverwanted) //this needs to be redone, cops are broken when disabled
		{
			PLAYER::SET_MAX_WANTED_LEVEL(0);
			PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
			PLAYER::RESET_WANTED_LEVEL_DIFFICULTY(player);
			PLAYER::SET_POLICE_IGNORE_PLAYER(player, TRUE);
			PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(player, FALSE);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, FALSE);
		}
		else
		{
			PLAYER::SET_POLICE_IGNORE_PLAYER(player, FALSE);
			PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(player, TRUE);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, TRUE);
		}

		//Ghost organization

		if (g_config.Lfeatures_offradar) //this needs to be updated, it no longer functions
		{
			*script_global(2426097).at(player, 443).at(204).as<bool*>() = true;
			*script_global(2440277).at(70).as<int32_t*>() = NETWORK::GET_NETWORK_TIME();
			*script_global(2540612).at(4625).as<int32_t*>() = 4;
		}

		//Fast run

		if (g_config.Lfeatures_fastrun)
			PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.45f);
		else
			PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.f);

		//Ped invisibility

		if (g_config.Lfeatures_invisible)
			ENTITY::SET_ENTITY_VISIBLE(ped, FALSE, FALSE);
		else
			ENTITY::SET_ENTITY_VISIBLE(ped, TRUE, TRUE);

		Ped _ped = ped; //to prevent declaration confusion in the follow statements
		if (PED::IS_PED_IN_ANY_VEHICLE(_ped, FALSE) && owns_veh(_ped)) // kinda a ghetto fix but it works for now 
			_ped = PED::GET_VEHICLE_PED_IS_IN(_ped, FALSE);

		//Waypoint Teleport

		if (GetAsyncKeyState(VK_F5) || g_config.Lfeatures_teleportwp || (UI::IS_WAYPOINT_ACTIVE() && CONTROLS::IS_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_DOWN)))
		{
			if (!find_blip().z || !find_blip().x || !find_blip().y || !UI::IS_WAYPOINT_ACTIVE()) // ?
			{
				features::log_map("You do not have a waypoint set", logtype::LOG_ERROR);
				return;
			}

			Vector3 coords = find_blip();

			//if you really care then add a ground check so you can teleport to chilliad because i dont
			if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0) && owns_veh(ped))
				ped = PED::GET_VEHICLE_PED_IS_USING(ped);

			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, coords.x, coords.y, coords.z + 50, 0, 0, 1);

			features::log_map(fmt::format("Teleporting to ~g~x{} y{} z{}", coords.x, coords.y, coords.z), logtype::LOG_INFO);

			g_config.Lfeatures_teleportwp = false;
		}

		//Noclip

		if (g_config.Lfeatures_noclip) //needs to be completely redone using local face camera angles. it's also messy.
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(_ped, FALSE) && !features::owns_veh(ped))
			{
				log_map("Noclip is unavailable as a passenger", logtype::LOG_ERROR);
				g_config.Lfeatures_noclip = false;
				return;
			}

			auto speed = GetAsyncKeyState(VK_SHIFT) ? 10.f : 2.f;
			Vector3 rot = { 0.f, 0.f, 0.f };
			Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(_ped, 1);

			ENTITY::SET_ENTITY_COLLISION(_ped, FALSE, FALSE);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, playerCoords.x, playerCoords.y, playerCoords.z, 0, 0, 0);

			//forward
			if (GetAsyncKeyState(VK_KEY_W))
			{
				rot = ENTITY::GET_ENTITY_ROTATION(_ped, 0);
				Vector3 to = get_coords_infront_of_coords(playerCoords, rot, .25f * (speed * 1.5f));
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//back
			if (GetAsyncKeyState(VK_KEY_S))
			{
				rot = ENTITY::GET_ENTITY_ROTATION(_ped, 0);
				Vector3 to = get_coords_infront_of_coords(playerCoords, rot, -.25f * (speed * 1.5f));
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//left
			if (GetAsyncKeyState(VK_KEY_A))
				ENTITY::SET_ENTITY_HEADING(_ped, ENTITY::GET_ENTITY_HEADING(_ped) + 2.5f * 2.f);

			//right
			if (GetAsyncKeyState(VK_KEY_D))
				ENTITY::SET_ENTITY_HEADING(_ped, ENTITY::GET_ENTITY_HEADING(_ped) - 2.5f * 2.f);

			//up
			if (GetAsyncKeyState(VK_SPACE))
			{
				Vector3 to = get_coords_above_coords(playerCoords, .2f * speed);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//down
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Vector3 to = get_coords_above_coords(playerCoords, -.2f * speed);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}
		}
		else
			ENTITY::SET_ENTITY_COLLISION(_ped, TRUE, TRUE);
	}

	void features_weapon()
	{
		//Infinite ammo

		if (g_config.Wfeatures_infammo)
		{
			Hash cur;
			if (WEAPON::GET_CURRENT_PED_WEAPON(ped, &cur, 1))
			{
				if (WEAPON::IS_WEAPON_VALID(cur))
				{
					int maxAmmo;
					if (WEAPON::GET_MAX_AMMO(ped, cur, &maxAmmo))
					{
						WEAPON::SET_PED_AMMO(ped, cur, maxAmmo);

						maxAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(ped, cur, 1);
						if (maxAmmo > 0)
							WEAPON::SET_AMMO_IN_CLIP(ped, cur, maxAmmo);
					}
				}
			}
		}
		
		//Instakill

		if (g_config.Wfeatures_instakill)
		{
			Vector3 CamCoords = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 CamRotation = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 CamDirection = wRotationToDirection(CamRotation);
			Vector3 StartCoords = waddVector(CamCoords, (wmultiplyVector(CamDirection, 1.0f)));
			Vector3 EndCoords = waddVector(StartCoords, wmultiplyVector(CamDirection, 500.0f));
			if (PED::IS_PED_SHOOTING(ped))
			{
				for (int i = 0; i < 3; i++)
					GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(StartCoords.x, StartCoords.y, StartCoords.z, EndCoords.x, EndCoords.y, EndCoords.z, 250, true, GAMEPLAY::GET_HASH_KEY("WEAPON_REMOTESNIPER"), ped, true, true, -1.0f);
			}
		}

		//Triggerbot

		if (g_config.Wfeatures_triggerbot)
		{
			Entity AimedAtEntity;
			Vector3 CamCoords = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 CamRotation = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 CamDirection = wRotationToDirection(CamRotation);
			Vector3 StartCoords = waddVector(CamCoords, (wmultiplyVector(CamDirection, 1.0f)));

			if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &AimedAtEntity))
			{
				if (PED::IS_PED_A_PLAYER(AimedAtEntity) && *g_pointers->m_is_session_started) //Online players
				{
					auto player = peds[AimedAtEntity];
					if (getNetGamePlayer(player) && getNetGamePlayer(player)->m_PlayerInfo && getNetGamePlayer(player)->m_PlayerInfo->ped)
					{
						Vector3 vec{}; renderer::GetBonePosition2(getNetGamePlayer(player)->m_PlayerInfo->ped, &vec, 31086);
						fireBullet(StartCoords, vec);
					}
				}
				else if (ENTITY::IS_ENTITY_A_PED(AimedAtEntity) && !ENTITY::IS_ENTITY_DEAD(AimedAtEntity) && ENTITY::GET_ENTITY_ALPHA(AimedAtEntity) == 255) //Pedestrians
				{
					Vector3 Mouth = PED::GET_PED_BONE_COORDS(AimedAtEntity, 31086, 0.1f, 0.0f, 0.0f);
					fireBullet(StartCoords, Mouth);
				}
			}
		}

		//Add all weapons

		if (g_config.Wfeatures_addweapons)
		{
			log_map("Adding all weapons to weapon wheel", logtype::LOG_INFO);

			for (int i = 0; i < (sizeof(features::Weapons) / 4); i++)
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, features::Weapons[i], 9999, 1);
				g_config.Wfeatures_addweapons = false;
			}
		}
	}

	void features_vehicle_delay()
	{
		//Vehicle godmodee

		Vehicle v = PED::GET_VEHICLE_PED_IS_IN(ped, player);

		if (g_config.Vfeatures_godmode) //a little cleaner but still messy
		{
			if (features::owns_veh(ped)) //boss patch
			{
				ENTITY::SET_ENTITY_INVINCIBLE(v, TRUE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(v, FALSE);
				ENTITY::SET_ENTITY_PROOFS(v, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
				ENTITY::SET_ENTITY_MAX_SPEED(v, 1000000000);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(v, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, FALSE);
				PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, FALSE);

				VEHICLE::SET_VEHICLE_ENGINE_HEALTH(v, 1000.0f);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(v, FALSE);
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(v, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, FALSE);
				if (VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(v))
				{
					VEHICLE::_SET_VEHICLE_ROCKET_BOOST_REFILL_TIME(v, 0);
					VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(v, 100.0f);
				}
			}
		}
		else
		{
			if (features::owns_veh(ped))
			{
				ENTITY::SET_ENTITY_INVINCIBLE(v, FALSE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(v, TRUE);
				ENTITY::SET_ENTITY_PROOFS(v, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(v, TRUE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, TRUE);
			}
		}

		//Auto repair

		if (g_config.Vfeatures_autoclean)
		{
			if (features::owns_veh(ped))
			{
				Vehicle _v = PED::GET_VEHICLE_PED_IS_USING(scriptIndex);
				VEHICLE::SET_VEHICLE_FIXED(_v);
				VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(_v);
				VEHICLE::SET_VEHICLE_DIRT_LEVEL(_v, 0);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(v, FALSE);
			}

		}
		else
		{
			if (features::owns_veh(ped))
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(v, TRUE);
		}
	}

	void features_vehicle()
	{
		//Model import call

		if (g_config.Vfeatures_requestentity)
		{
			if (g_config.Vfeatures_randomizeveh)
				log_map("Importing random vehicle model", logtype::LOG_INFO);
			else
				log_map(fmt::format("Importing vehicle model ~g~{}", features::carToSpawn), logtype::LOG_INFO);
			features::spawnvehicle(features::carToSpawn.c_str());
			g_config.Vfeatures_requestentity = false;
		}

		//Vehicle invisibility

		if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE) && features::owns_veh(ped))
		{
			if (g_config.Vfeatures_invisible)
				ENTITY::SET_ENTITY_VISIBLE(vehicle, TRUE, TRUE);
			else
				ENTITY::SET_ENTITY_VISIBLE(vehicle, FALSE, FALSE);
		}

		//Upgrade current vehicle

		if (g_config.Vfeatures_autoupgrade)
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE))
			{
				if (features::owns_veh(ped))
				{
					log_map("Current vehicle has been fully upgraded", logtype::LOG_INFO);

					VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);

					for (int i = 0; i < 48; i++) //49, 50 = livery
					{
						VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, TRUE);
						VEHICLE::SET_VEHICLE_MOD(vehicle, i, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) - 1, FALSE);
					}

					for (int i = 0; i < 8; i++)
						VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(vehicle, i, 1);
				}
				else
					log_map("You are not the owner of the vehicle", logtype::LOG_ERROR);
			}
			else
				log_map("You are not in a vehicle", logtype::LOG_ERROR);

			g_config.Vfeatures_autoupgrade = false;
		}

		//Horn boost

		if (g_config.Vfeatures_hornboost)
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0))
			{
				if (features::owns_veh(ped) && PLAYER::IS_PLAYER_PRESSING_HORN(player))
				{
					//RequestControlOfEnt(vehicle);
					if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle))
						VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 79);
				}
			}
		}
		
		//Perfect vehicle control

		if (g_config.Vfeatures_perfecthandling)
		{
			if (features::localCPed && features::lastVehicle) //fuck you
			{
				if (features::owns_veh(ped)) //boss patch function created by god himself
					features::lastVehicle->gravity = 30.0f;
			}
		}
	}

	void cache_player(int i)
	{
		int netHandle[13];
		auto ped = PLAYER::GET_PLAYER_PED(i);
		auto netPlayer = getNetGamePlayer(i);
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, 1);
		Vector3 mycoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);

		if (netPlayer)
		{
			players[i].player = netPlayer;
			players[i].netData = netPlayer->get_net_data();

			players[i].info = netPlayer->m_PlayerInfo;

			if (players[i].netData)
				players[i].rockstarId2 = players[i].netData->m_rockstar_id;

			if (players[i].info && players[i].info->ped)
			{
				players[i].invincible = players[i].info->ped->m_godmode;
				players[i].rockstarId3 = players[i].info->rockstarID;
			}

			players[i].isSessionHost = netPlayer->is_host();
		}

		//cache local info real quick
		if (i == player) { localPed = ped;	localIndex = i; } // why are you doing this here | this is where its done in niggahack

		NETWORK::NETWORK_HANDLE_FROM_PLAYER(i, netHandle, 13);
		peds[ped] = i; // fuck you
		players[i].ped = ped;
		players[i].index = i;
		players[i].exists = ENTITY::DOES_ENTITY_EXIST(ped);;
		players[i].name = PLAYER::GET_PLAYER_NAME(i);
		players[i].invehicle = PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED(i), 0);
		players[i].interior = INTERIOR::GET_INTERIOR_FROM_ENTITY(ped);
		players[i].isfriend = isPlayerFriend(i);
		players[i].mutedMe = NETWORK::NETWORK_AM_I_MUTED_BY_PLAYER(i);
		players[i].blockedMe = NETWORK::NETWORK_AM_I_BLOCKED_BY_PLAYER(i);
		players[i].connected = NETWORK::NETWORK_IS_PLAYER_CONNECTED(i);
		players[i].active = NETWORK::NETWORK_IS_PLAYER_ACTIVE(i);
		players[i].health = ENTITY::GET_ENTITY_HEALTH(ped);
		players[i].maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(ped);
		players[i].rank = *script_global(1590682).at(i, 883).at(211).at(6).as<int32_t*>();
		players[i].RP = *script_global(1590682).at(i, 883).at(211).at(1).as<int32_t*>();
		players[i].cash = *script_global(1590682).at(i, 883).at(211).at(3).as<int32_t*>();
		players[i].visible = ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player), 17);
		players[i].distanceToLocal = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pos.x, pos.y, pos.z, mycoords.x, mycoords.y, mycoords.z, true);
		players[i].rockstarId = NETWORK::NETWORK_MEMBER_ID_FROM_GAMER_HANDLE(&netHandle[0]);
	}

	void features_esp(int p)
	{
		if (*g_pointers->m_is_session_started)
		{
			int handle = PLAYER::GET_PLAYER_PED(p);
			auto pname = PLAYER::GET_PLAYER_NAME(p);

			Vector3 theircoords = ENTITY::GET_ENTITY_COORDS(handle, TRUE);
			Vector3 mycoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);

			float xoffset, yoffset;

			float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(theircoords.x, theircoords.y, theircoords.z, mycoords.x, mycoords.y, mycoords.z, true);

			int maxHealth = features::players[p].maxHealth - 100;
			int health = std::clamp(features::players[p].health - 100, 0, maxHealth);

			Vector3 healthColor = features::FromHSB(std::clamp((float)(health) / (float)(maxHealth * 3.6f), 0.f, 0.277777777778f), 1.f, 1.f);

			float ColR, ColG, ColB;

			if (g_config.ESPfeatures_health)
			{
				ColR = healthColor.x, ColG = healthColor.y, ColB = healthColor.z;
				if (p < 32 && features::players[p].invincible) { ColR = 255, ColG = 255, ColB = 255; }
				if (features::players[features::selectedPlayer].invincible) { ColR = 255, ColG = 255, ColB = 255; }
			}
			else
				ColR = g_config.ESPfeatures_color[0] * 255, ColG = g_config.ESPfeatures_color[1] * 255, ColB = g_config.ESPfeatures_color[2] * 255;

			if (!ENTITY::DOES_ENTITY_EXIST(handle))
				return;

			if (p == player)
				return;

			if (distance > g_config.esp.render_distance)
				return;

			if (g_config.ESPfeatures_visible && !ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(p), 17)) // to do: fix this for players under the ground
				return;

			GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(theircoords.x, theircoords.y, theircoords.z + 1.3f, &xoffset, &yoffset);

			if (g_config.ESPfeatures_snapline)
				GRAPHICS::DRAW_LINE(mycoords.x, mycoords.y, mycoords.z, theircoords.x, theircoords.y, theircoords.z, ColR, ColG, ColB, 255); //snapline
			if (g_config.ESPfeatures_marker)
				GRAPHICS::DRAW_MARKER(20, theircoords.x, theircoords.y, theircoords.z + 2.0f, 0, 0, 0, 180.0f, 360.0f, 0, 1.5f, 1.5f, 1.5f, ColR, ColG, ColB, 100, true, false, 2, true, false, false, false); //marker

			if (g_config.ESPfeatures_box)
			{
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
				GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
			}
		}
	}

	void features_online()
	{
		//Initial ESP and player cache call

		if (*g_pointers->m_is_session_started)
		{
			for (std::uint32_t i = 0; i < 32; ++i) //initialize functions for each player in the lobby
			{
				cache_player(i);
				features_esp(i);
			}
		}
		
		//Disable phone

		if (g_config.Lfeatures_nophone)
		{
			misc::set_global(19664, 1); //needs to be updated
			MOBILE::_CELL_CAM_DISABLE_THIS_FRAME(TRUE);
			PAD::DISABLE_CONTROL_ACTION(2, INPUT_PHONE, TRUE);
		}

		//No mental state

		if (g_config.Lfeatures_nomental)
			STATS::STAT_SET_FLOAT(GAMEPLAY::GET_HASH_KEY("MP0_PLAYER_MENTAL_STATE"), 0.1, 1);

		//Skip cutscene
		//Needs to be reimplemented

		if (g_config.Ofeatures_skipcutscene)
		{
			if (CUTSCENE::IS_CUTSCENE_ACTIVE() || CUTSCENE::IS_CUTSCENE_PLAYING())
			{
				CUTSCENE::STOP_CUTSCENE;
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			}
			else
				log_map("No cutscene is currently active", logtype::LOG_ERROR);

			g_config.Ofeatures_skipcutscene = false;
		}
	}

	void features_player()
	{
		//Teleport to player

		if (g_config.Pfeatures_teleport)
		{
			Ped handle = ped; //to prevent redeclaration confusion autism
			Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(features::selectedPlayer), FALSE);
			int player = PLAYER::GET_PLAYER_PED(features::selectedPlayer);

			if (PED::IS_PED_IN_ANY_VEHICLE(handle, FALSE))
			{
				if (features::owns_veh(handle))
					handle = PED::GET_VEHICLE_PED_IS_IN(handle, vehicle);
				ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);
			}
			else if (PED::IS_PED_IN_ANY_VEHICLE(player, FALSE))
			{
				bool foundSeat = false;
				Vehicle playersVehicle = PED::GET_VEHICLE_PED_IS_IN(player, false);
				if (ENTITY::DOES_ENTITY_EXIST(playersVehicle) && ENTITY::IS_ENTITY_A_VEHICLE(playersVehicle))
				{
					for (int i = 0; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(playersVehicle); i++)
					{
						if (VEHICLE::IS_VEHICLE_SEAT_FREE(playersVehicle, i))
						{
							RequestControlOfEnt(playersVehicle);
							PED::SET_PED_INTO_VEHICLE(handle, playersVehicle, i);
							foundSeat = true;
							break;
						}
					}

					if (!foundSeat)
					{
						if (PED::IS_PED_IN_ANY_VEHICLE(handle, FALSE))
							handle = PED::GET_VEHICLE_PED_IS_IN(handle, 0);

						ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);
					}
				}
			}
			else
				ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);

			log_map(fmt::format("Teleporting to ~g~{}", features::players[features::selectedPlayer].name), logtype::LOG_INFO);
			g_config.Pfeatures_teleport = false;
		}
	}

#pragma region init

	void block_main()
	{
		//block afk, needs to be updated
		misc::set_global(2550148 + 296, -1000000);
		misc::set_global(1377236 + 1149, -1000000);

		//block vote, needs to be updated
		misc::set_global(1388057, 0);
		misc::set_global(1388059, 0);
		misc::set_global(1388060, 0);
		misc::set_global(1388062, 0);
		misc::set_global(1388063, 0);
	}

	void dec_var()
	{
		//bool a = player || ped || local ||inSession || sessionActive || scriptIndex || vehicle || isHost || network_time || local || localInfo || localCPed || lastVehicle || lastVehicleHandling;
		//if (a = NULL)
		//	return;

		player = PLAYER::PLAYER_ID();
		ped = PLAYER::PLAYER_PED_ID();
		local = getNetGamePlayer(player);
		inSession = NETWORK::NETWORK_IS_IN_SESSION();
		sessionActive = NETWORK::NETWORK_IS_SESSION_ACTIVE();
		scriptIndex = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
		vehicle = PED::GET_VEHICLE_PED_IS_IN(ped, player); //calls a function that checks if ur in a vehicle, what seat, and if you're the owner. will make everything else easier.
		isHost = NETWORK::NETWORK_IS_HOST();
		network_time = NETWORK::GET_NETWORK_TIME();

		if (inSession)
		{
			numberOfPlayers = PLAYER::GET_NUMBER_OF_PLAYERS();
			scriptHost = NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0);
		}

		if (local)
		{
			localInfo = local->m_PlayerInfo;
			if (localInfo)
			{
				localCPed = localInfo->ped;
				if (localCPed)
				{
					lastVehicle = localCPed->m_vehicle;
					if (lastVehicle)
						lastVehicleHandling = lastVehicle->handlingData;

					if (g_config.vehicle.seatbelt)
						features::localCPed->seatbelt = features::localCPed->seatbelt | 0x01;
					else
						features::localCPed->seatbelt = (features::localCPed->seatbelt | 0x01) ^ 0x01;
				}
			}
		}
	}

	void run_tick()
	{
		dec_var(); //global variable declaration function, see above

		QUEUE_JOB_BEGIN_CLAUSE()
		{
			static ULONGLONG tick_1 = 0;
			static ULONGLONG tick_60 = 0;
			ULONGLONG now = GetTickCount64();

			if (now - tick_60 > 60000)
			{
				if (features::features_kickprotection)
					block_main();
				show_stat();
				tick_60 = now;
			}

			//Prevents vehicle seizures
			if (now - tick_1 > 1000) 
			{
				features_vehicle_delay();
				tick_1 = now;
			}

			if (inSession)
			{
				features_online();
				features_player();
			}

			features_vehicle();
			features_local();
			features_weapon();

		} QUEUE_JOB_END_CLAUSE
	}

	void script_func()
	{
		while (true)
		{
			TRY_CLAUSE{ run_tick(); }EXCEPT_CLAUSE
			script::get_current()->yield();
		}
	}
#pragma endregion
}

#include "common.hpp"
#include "base_tab.h"
#include "script.hpp"
#include "gui.hpp"
#include "misc.h"
#include "features.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include <script_global.hpp>
#include "math.h"
#include <time.h>
#include <sstream>


// https://github.com/Sainan/GTA-V-Decompiled-Scripts
// https://github.com/Pocakking/BigBaseV2
// https://bitbucket.org/gir489/bigbasev2-fix/src/master/
// https://wiki.rage.mp/index.php?title=Category:Assets
// https://github.com/MAFINS/MenyooSP
// https://www.reddit.com/r/Gta5Modding/comments/h17xia/list_of_gta_v_paid_mod_menus_for_pc/

using namespace std;
using namespace fmt;
using namespace rage;

namespace big::base_tab
{
	static const int DISTANCE_SPAWN = 10;

	const bool LOG_LOG = false;

#pragma region move to imgui
	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			string* str = (string*)data->UserData;
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}
		return 0;
	}

	bool InputText(const char* label, string* str, ImGuiInputTextFlags flags)
	{
		flags |= ImGuiInputTextFlags_CallbackResize;
		return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, (void*)str);
	}
#pragma endregion

	void render_local_tab()
	{
		if (ImGui::BeginTabBar("##tabs2", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Player"))
			{
				ImGui::Checkbox("Godmode", &big::features::Lfeatures_godmode);
				ImGui::Checkbox("Ragdoll prevention", &big::features::Lfeatures_noragdoll);
				ImGui::Checkbox("No wanted level", &big::features::Lfeatures_neverwanted);
				ImGui::Checkbox("Ghost organization", &big::features::Lfeatures_offradar);
				ImGui::Checkbox("No mental state", &big::features::Lfeatures_nomental);
				ImGui::Checkbox("No phone", &big::features::Lfeatures_nophone);
				ImGui::Checkbox("Invisibility", &big::features::Lfeatures_invisible);
				ImGui::Checkbox("Noclip", &big::features::Lfeatures_noclip);

				ImGui::Separator();

				if (ImGui::Button("Teleport to waypoint"))
					features::Lfeatures_teleportwp = true;

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Weapons"))
			{
				ImGui::Checkbox("Infinite ammo", &big::features::Wfeatures_infammo);
				ImGui::Checkbox("Insta kill", &big::features::Wfeatures_instakill);
				ImGui::Checkbox("Triggerbot", &big::features::Wfeatures_autoshoot);

				ImGui::Separator();

				if (ImGui::Button("Give all weapons"))
					features::Wfeatures_addweapons = true;

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void render_vehicle_tab()
	{
		if (ImGui::BeginTabBar("##tabs2", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Spawner"))
			{
				static string currentModelSearch = "";
				static const char* currentModel = NULL;
				if (ImGui::BeginCombo("Vehicles", currentModel))
				{
					InputText("Search", &currentModelSearch, 0);

					for (int n = 0; n < IM_ARRAYSIZE(features::vehicleModels); n++)
					{
						string str = features::vehicleModels[n], search = currentModelSearch;
						transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return tolower(c); });
						transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return tolower(c); });
						bool searchFound = str.find(search) != string::npos;

						if (currentModelSearch.size() > 0 && !searchFound)
							continue;

						if (ImGui::Selectable(features::vehicleModels[n]))
						{
							currentModel = features::vehicleModels[n];
							features::carToSpawn = currentModel;
						}
					}
					ImGui::EndCombo();
				}

				InputText("Custom vehicle", &features::carToSpawn, 0);

				ImGui::ColorEdit3("Vehicle color", features::Vfeatures_vcol);
				ImGui::Checkbox("Spawn with godmode", &big::features::Vfeatures_spawngodmode);
				ImGui::Checkbox("Spawn fully upgraded", &big::features::Vfeatures_spawnupgraded);
				ImGui::Checkbox("Spawn in vehicle", &big::features::Vfeatures_spawninveh);
				ImGui::Checkbox("Spawn with random colors", &big::features::Vfeatures_randomizecol);

				if (ImGui::Button("Import vehicle"))
					features::Vfeatures_requestentity = true;

				//ImGui::SameLine();
				//ImGui::Checkbox("Randomize?", &features::Vfeatures_spawnrvehicle); //note: fix this, it spawns the same random vehicle
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Options"))
			{
				ImGui::Checkbox("Indestructable vehicle", &big::features::Vfeatures_godmode);
				ImGui::Checkbox("Clean vehicle", &big::features::Vfeatures_autoclean);
				ImGui::Checkbox("Horn boost", &big::features::Vfeatures_hornboost);

				ImGui::Separator();

				if (ImGui::Button("Upgrade current vehicle"))
					features::Vfeatures_autoupgrade = true;

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void render_online_tab()
	{
		if (ImGui::BeginTabBar("##tabs6", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("ESP"))
			{
				ImGui::Checkbox("Enable ESP", &big::features::ESPfeatures);
				ImGui::SameLine();
		
				if (big::features::ESPfeatures)
				{
					ImGui::ColorEdit3("ESP Color", features::ESPfeatures_col);
					ImGui::Checkbox("Health based colors", &big::features::ESPfeatures_health);
					ImGui::Checkbox("Visible only", &big::features::ESPfeatures_visible);
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Lobby"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, .0f, 1.f));

				if (ImGui::Button("Kick all players"))
					features::Pfeatures_kickall = true;

				if (ImGui::Button("Crash all players"))
					features::Pfeatures_crashall = true;

				ImGui::PopStyleColor();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Players"))
			{
				static string currentPlayerSearch = "";

				ImGui::Text(format("[Players in lobby]: {}", features::numberOfPlayers).c_str());

				ImGui::Separator();
				InputText("Search##players", &currentPlayerSearch, 0);
				ImGui::Separator();

				for (int i = 0; i < 32; i++)
				{
					string name = features::players[i].name; name += " ##"; name += to_string(i).c_str();
					string str = features::players[i].name, search = currentPlayerSearch;
					transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return tolower(c); });
					transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return tolower(c); });
					bool searchFound = str.find(search) != string::npos;

					if (!features::players[i].ped)
						continue;

					if (!features::players[features::selectedPlayer].name)
					{
						ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NAME IS NULLPTR");
						continue;
					}

					if (currentPlayerSearch.size() > 0 && !searchFound)
						continue;

					if (ImGui::Selectable(name.c_str(), features::selectedPlayer == i))
						features::selectedPlayer = i;

					if (features::players[i].index == features::localIndex)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "LOCAL");
					}
					if (features::players[i].isfriend)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.1f, 1.f, 0.1f, 1.f), "FRIEND");
					}
					if (features::players[i].index == features::scriptHost)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.1f, 1.f), "SCRIPT HOST");
					}
					if (features::players[i].isSessionHost)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.1f, 1.f, 0.1f, 1.f), "SESSION HOST");
					}
					if (features::players[i].interior)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.f), "(interior)"); //todo: patch flickering caused by casino interior
					}
					if (features::players[i].invehicle)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.f), "(vehicle)");
					}
				}

				if (ImGui::Begin("###SELECTED PLAYER"))
				{
					ImGui::Text("Name:");
					ImGui::SameLine();
					if (features::players[features::selectedPlayer].name)
						ImGui::TextColored(ImVec4(0.1f, 1.0f, 1.0f, 1.f), features::players[features::selectedPlayer].name);
					else
						ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NAME IS NULLPTR");

					ImGui::Text("Health: ");
					int maxHealth = features::players[features::selectedPlayer].maxHealth - 100;
					int health = clamp(features::players[features::selectedPlayer].health - 100, 0, maxHealth);
					Vector3 healthColor = features::FromHSB(clamp((float)(health) / (float)(maxHealth * 3.6f), 0.f, 0.277777777778f), 1.f, 1.f);
					if (features::players[features::selectedPlayer].invincible) { healthColor.x = 255;	healthColor.y = 255;	healthColor.z = 255; }
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(healthColor.x / 255.f, healthColor.y / 255.f, healthColor.z / 255.f, 1.f));
					ImGui::Text(format("{}/{}", features::players[features::selectedPlayer].health, features::players[features::selectedPlayer].maxHealth).c_str());
					ImGui::PopStyleColor();

					ImGui::Text(format("Rank: {}", features::players[features::selectedPlayer].rank).c_str());

					ImGui::Text(format("Index: {}", features::players[features::selectedPlayer].index).c_str());
					ImGui::Text(format("Ped: {}", features::players[features::selectedPlayer].ped).c_str());

					string rockstarId = features::players[features::selectedPlayer].rockstarId;

					ImGui::Separator();
					InputText("RSID", &rockstarId, 0);

					ImGui::Separator();

					if (ImGui::Button("Teleport to player"))
						features::Pfeatures_teleport = true;

					if (ImGui::Button("Kick from vehicle"))
						features::Pfeatures_kickfromveh = true;

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 0.2f, 1.f));

					if (ImGui::Button("Kick from lobby"))
						features::Pfeatures_kick = true;

					ImGui::PopStyleColor();

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, .0f, 1.f));

					if (ImGui::Button("Crash player"))
						features::Pfeatures_crash = true;

					ImGui::PopStyleColor();

					//if (features::players[features::selectedPlayer].player)
					//{
					//	string debug = format("{}", reinterpret_cast<void*>(features::players[features::selectedPlayer].player));
					//	InputText("CNetGamePlayer", &debug, 0);

					//	debug = format("{}", reinterpret_cast<void*>(features::players[features::selectedPlayer].info));
					//	InputText("CPlayerInfo", &debug, 0);

					//	debug = format("{}", reinterpret_cast<void*>(features::players[features::selectedPlayer].netData));
					//	InputText("netData", &debug, 0);,0xb131133a

					//	if (features::players[features::selectedPlayer].info) //broken, need to fix
					//	{
					//		debug = format("{}", reinterpret_cast<void*>(features::players[features::selectedPlayer].info->ped));
					//		InputText("CPed", &debug, 0);
					//	}
					//}
				}

				ImGui::End();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Protex"))
			{
				ImGui::Checkbox("Crash protection", &big::features::protection);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Recovery"))
			{
		
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				if (ImGui::Button("Skip current cutscene"))
					features::Ofeatures_skipcutscene = true;
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void render_settings_tab()
	{
		if (ImGui::BeginTabBar("##tabs6", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Config"))
			{
				if (ImGui::Button("Unload"))
					g_running = false;
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Debug"))
			{
				if (*g_pointers->m_is_session_started)
				{
					InputText("##localIndex", &format("localIndex: {}", reinterpret_cast<void*>(features::localIndex)), 0);
					InputText("##localPed", &format("localPed: {}", reinterpret_cast<void*>(features::localPed)), 0);
					InputText("##local", &format("local: {}", reinterpret_cast<void*>(features::local)), 0);
					InputText("##localInfo", &format("localInfo: {}", reinterpret_cast<void*>(features::localInfo)), 0);
					InputText("##localCPed", &format("localCPed: {}", reinterpret_cast<void*>(features::localCPed)), 0); //bugged, will return nullptr
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}

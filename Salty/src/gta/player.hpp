#pragma once
#include "extensible.hpp"
#include "vector.hpp"

#pragma pack(push, 1)
namespace rage
{
#	pragma warning(push)
#	pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
	union netAddress
	{
		uint32_t m_raw;
		struct { uint8_t m_field4;	uint8_t m_field3;	uint8_t m_field2;	uint8_t m_field1; };
	};
#	pragma warning(pop)

	//Size: 0x0098

	class netPlayerData
	{
	public:
		char pad_0000[8]; //0x0000
		int64_t m_rockstar_id_0; //0x0008
		char pad_0010[56]; //0x0010
		uint16_t N000005BF; //0x0048
		char pad_004A[2]; //0x004A
		netAddress m_online_ip; //0x0054
		int16_t m_online_port; //0x0058
		netAddress m_relay_ip; //0x004C
		int32_t m_relay_port; //0x0050
		char pad_005A[6]; //0x005A
		uint32_t m_host_token; //0x0060
		char pad_0064[12]; //0x0064
		int64_t m_rockstar_id; //0x0070
		char pad_0078[12]; //0x0078
		char m_name[20]; //0x0084
	}; 
	
	class netPlayer
	{
	public:
		virtual ~netPlayer() = default;            // 0 (0x00)
		virtual void reset() = 0;                  // 1 (0x08)
		virtual bool is_valid() const = 0;         // 2 (0x10)
		virtual const char* get_name() const = 0;  // 3 (0x18)
		virtual void _0x20() = 0;                  // 4 (0x20)
		virtual bool is_host() = 0;                // 5 (0x28)
		virtual netPlayerData* get_net_data() = 0; // 6 (0x30)
		virtual void _0x38() = 0;                  // 7 (0x38)
	};

	class nonPhysicalPlayerDataBase
	{
	public:
		virtual ~nonPhysicalPlayerDataBase() = default;    // 0 (0x00)
		virtual void unk_0x08() = 0;                       // 1 (0x08)
		virtual void unk_0x10() = 0;                       // 2 (0x10)
		virtual void unk_0x18() = 0;                       // 3 (0x18)
		virtual void log(netLoggingInterface* logger) = 0; // 4 (0x20)
	};

	class CNonPhysicalPlayerData : public nonPhysicalPlayerDataBase {};

	class CNetGamePlayer : public rage::netPlayer
	{
	public:
		char pad_0x0008[0x10]; //0x0008 
		std::uint32_t msg_id; // 0x18
		char pad_0x001C[0x4]; //0x001C
		std::int8_t active_id; //0x0020 
		std::int8_t player_id; //0x0021 
		char pad_0x0022[0x6E]; //0x0022
		BYTE local_player_check;//0x0090
		char pad_0x00A1[0xF]; //0x0091
		CPlayerInfo* m_PlayerInfo; //0x00A0 

		inline bool is_local_player() { return local_player_check & 1; }
	};

	class netPlayerMgrBase
	{
	public:
		virtual ~netPlayerMgrBase() = default; // 0 (0x00)
	};

	class CNetworkPlayerMgr : netPlayerMgrBase {};

	class CPlayerInfo : public rage::fwExtensibleBase
	{
	public:
		char pad_0000[108]; //0x0000
		uint32_t externalIP; //0x006C
		uint16_t externalPort; //0x0070 
		char pad_0072[30]; //0x0072
		uint64_t rockstarID; //0x0090
		char pad_0098[12]; //0x0098
		char name[20]; //0x00A4
		char pad_00B8[304]; //0x00B8
		class CPed* ped; //0x01E8
		char pad_01F0[40]; //0x01F0
		uint32_t frameFlags; //0x0218
		char pad_021C[1636]; //0x021C
		bool isWanted; //0x0880
		char pad_0881[7]; //0x0881
		uint8_t wantedLevel; //0x0888
		char pad_0889[1975]; //0x0889
	};
}

#pragma pack(pop)


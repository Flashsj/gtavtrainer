#pragma once
#pragma pack(push, 1)

#include "common.hpp"
#include "fwddec.hpp"

namespace rage
{
	class fwEntity
	{
	public:
		char pad_0000[32]; //0x0000
		class CBaseModelInfo* m_model_info; //0x0020
		int8_t m_invisible; //0x0028
		char pad_0029[7]; //0x0029
		class CNavigation* m_navigation; //0x0030
		char pad_0038[1]; //0x0038
		int8_t m_entity_type; //0x0039
		char pad_003A[150]; //0x003A
		class netObject* m_net_object; //0x00D0
		char pad_00D8[176]; //0x00D8
		uint32_t m_damage_bits; //0x0188

		bool is_invincible() { return(m_damage_bits & (1 << 8)); }
		void enable_invincible() { m_damage_bits |= (1 << 8); }
		void disable_invincible() { m_damage_bits &= ~(1 << 8); }
	}; //Size: 0x018C
	static_assert(sizeof(fwEntity) == 0x18C, "fwEntity is not properly sized");


	class CVehicle : public fwEntity
	{
	public:
		char pad_018C[2716]; //0x018C
		//class CPed* m_driver; //0x0C28
		//class CPed* m_occupants[8]; //0x0C30
	}; //Size: 0x018C

	class CNavigation
	{
	public:
		char pad_0000[32]; //0x0000
		float m_heading; //0x0020
		float m_heading2; //0x0024
		char pad_0028[8]; //0x0028
		vector3 m_rotation; //0x0030
		char pad_003C[20]; //0x003C
		vector3 m_position; //0x0050
	}; //Size: 0x005C

	//class CPed : public fwEntity
	//{
	//public:
	//	char pad_018C[2972]; //0x018C
	//	class CVehicle* m_last_vehicle; //0x0D28
	//	char pad_0D30[892]; //0x0D30
	//	int8_t m_decal_index; //0x10AC
	//	int8_t m_decal_info; //0x10AD
	//	char pad_10AE[26]; //0x10AE
	//	class CPlayerInfo* m_player_info; //0x10C8
	//};

	class CAutomobile
	{
	public:
		char pad_0000[880]; //0x0000
		float speedLimit; //0x0370
		char pad_0374[1360]; //0x0374
		float acceleration; //0x08C4
		char pad_08C8[8]; //0x08C8
		float engineRPM; //0x08D0
		float engineRPM2; //0x08D4
		char pad_08D8[4]; //0x08D8
		float clutch; //0x08DC
		float throttle; //0x08E0
		char pad_08E4[84]; //0x08E4
		void* handlingData; //0x0938 // to do: redo CHandlingData with the handling.meta file
		char pad_0940[340]; //0x0940
		float dashboardSpeed; //0x0A94
		char pad_0A98[452]; //0x0A98
		float gravity; //0x0C5C
		char pad_0C60[3452]; //0x0C60
		float rideHeight1; //0x19DC
		float rideHeight2; //0x19E0
		char pad_19E4[1653]; //0x19E4
	}; //Size: 0x2059

	class CPed
	{
	public:
		char pad_0000[144]; //0x0000
		rage::vector3 m_position; //0x0090
		char pad_009C[237]; //0x009C
		uint8_t m_godmode; //0x0189
		char pad_018A[246]; //0x018A
		float m_health; //0x0280 
		char pad_0284[28]; //0x0284
		float m_maxhealth; //0x02A0
		char pad_02A4[2700]; //0x02A4
		CAutomobile* m_vehicle; //0x0D30
		char pad_0D38[912]; //0x0D38
		class CPlayerInfo* m_player_info; //0x10C8
		char pad_10D0[8]; //0x10D0
		class CPedWeaponManager* m_weapon_manager; //0x10D8
		char pad_10E0[812]; //0x10E0
		uint8_t seatbelt; //0x140C
		char pad_140D[117]; //0x140D
		uint8_t unk; //0x1482
		uint8_t gettingInVehicle; //0x1483
		char pad_1484[5]; //0x1484
		uint8_t inVehicle; //0x1489
		char pad_148A[86]; //0x148A
		float armor; //0x14E0
	}; //Size: 0x14E4

	class CObject : public fwEntity
	{
	public:
	}; //Size: 0x018C

	class CPickup : public fwEntity
	{
	public:
	}; //Size: 0x018C

	class CBaseModelInfo
	{
	public:
		char pad_0000[24]; //0x0000
		uint32_t m_model; //0x0018
	}; //Size: 0x001C

	class CPedFactory
	{
	public:
		virtual ~CPedFactory() = default;
		class CPed* m_local_ped; //0x0008
	}; //Size: 0x0010

	class CPickupHandle
	{
	public:
		class CPickup* m_pickup; //0x0000
		int32_t m_handle; //0x0008
		char pad_000C[4]; //0x000C
	}; //Size: 0x0010
	static_assert(sizeof(CPickupHandle) == 0x10, "CPickupHandle is not properly sized");

	class CObjectHandle
	{
	public:
		class CObject* m_object; //0x0000
		int32_t m_handle; //0x0008
		char pad_000C[4]; //0x000C
	}; //Size: 0x0010
	static_assert(sizeof(CObjectHandle) == 0x10, "CObjectHandle is not properly sized");

	class CObjectList
	{
	public:
		class CObjectHandle m_objects[2300]; //0x0000
	}; //Size: 0x8FC0

	class CPickupList
	{
	public:
		class CPickupHandle m_pickups[73]; //0x0000
	};

	class CObjectInterface
	{
	public:
		char pad_0000[344]; //0x0000
		class CObjectList* m_list; //0x0158
		int32_t m_max; //0x0160
		char pad_0164[4]; //0x0164
		int32_t m_cur; //0x0168
	}; //Size: 0x016C

	class CPedHandle
	{
	public:
		class CPed* m_ped; //0x0000
		int32_t m_handle; //0x0008
		char pad_000C[4]; //0x000C
	}; //Size: 0x0010
	static_assert(sizeof(CPedHandle) == 0x10, "CPedHandle is not properly sized");

	class CPedList
	{
	public:
		class CPedHandle m_peds[256]; //0x0000
	}; //Size: 0x1000

	class CPedInterface
	{
	public:
		char pad_0000[256]; //0x0000
		class CPedList* m_list; //0x0100
		int32_t m_max; //0x0108
		char pad_010C[4]; //0x010C
		int32_t m_cur; //0x0110
	}; //Size: 0x0114

	class CPickupInterface
	{
	public:
		char pad_0000[256]; //0x0000
		class CPickupList* m_list; //0x0100
		int32_t m_max; //0x0108
		char pad_010C[4]; //0x010C
		int32_t m_cur; //0x0110
	}; //Size: 0x0114

	class CVehicleHandle
	{
	public:
		class CVehicle* m_vehicle; //0x0000
		int32_t m_handle; //0x0008
		char pad_000C[4]; //0x000C
	}; //Size: 0x0010
	static_assert(sizeof(CVehicleHandle) == 0x10, "CVehicleHandle is not properly sized");

	class CVehicleList
	{
	public:
		class CVehicleHandle m_vehicles[300]; //0x0000
	}; //Size: 0x12C0

	class CVehicleInterface
	{
	public:
		char pad_0000[384]; //0x0000
		class CVehicleList* m_list; //0x0180
		int32_t m_max; //0x0188
		char pad_018C[4]; //0x018C
		int32_t m_cur; //0x0190
	}; //Size: 0x0194

	class CCameraInterface
	{
	public:
		char pad_0x0000[0x248]; //0x0000
		class CCameraList* m_list; //0x0180
		int32_t m_max; //0x0188
		char pad_018C[4]; //0x018C
		int32_t m_cur; //0x0190
	}; //Size: 0x0194

	class CReplayInterface
	{
	public:
		void* N000006F5; //0x0000 
		class CCameraInterface* m_camera; //0x0008 
		class CVehicleInterface* m_vehicle; //0x0010
		class CPedInterface* m_ped; //0x0018
		class CPickupInterface* m_pickup; //0x0020 
		class CObjectInterface* m_object; //0x0028
	}; //Size: 0x0030
}
#pragma pack(pop)
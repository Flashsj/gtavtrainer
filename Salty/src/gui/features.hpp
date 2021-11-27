#pragma once

#include "natives.hpp"
#include "common.hpp"
#include <imgui.h>
#include "gta/player.hpp"
#include "gta/natives.hpp"
#include <math.h>
#include "gta/replay.hpp"

//This file is a complete mess as well lol

namespace big::features
{
	extern bool injected;
	extern bool protection;

	extern volatile Player player;
	extern volatile Ped ped;

	extern volatile uint64_t sync;
	extern volatile uint64_t network;
	extern volatile uint64_t script;
	extern volatile uint64_t script2;

	extern void show_bye();
	extern void add_user(std::string _user);

	extern void run_tick();
	extern void script_func();

	//features added by me

	void kickFunc();
	//void anti_tamper();

	extern bool features_kickprotection;
	extern bool features_gameeventprotection;
	extern bool features_maleventprotection;

	inline std::string carToSpawn = "";

	inline int inSession;
	inline int isHost = 0;
	inline int sessionActive;
	inline int scriptHost = 0;
	inline int numberOfPlayers = 0;
	inline int selectedPlayer = 0;
	inline ImVec2 screenSize = { 0.f, 0.f };
	inline int network_time = 0;

	inline std::unordered_map<Ped, Player> peds; // this is so retarded

	struct playerList
	{
		int index;
		const char* name;
		Ped ped;
		int invehicle;
		//CPed* cped;
		rage::CPlayerInfo* info;
		rage::CNetGamePlayer* player;
		int interior;
		bool isfriend;
		bool invincible;
		std::string rockstarId;
		uint64_t rockstarId2;
		uint64_t rockstarId3;
		float distanceToLocal;
		int exists;
		int mutedMe;
		int blockedMe;
		int connected;
		int active;
		int health;
		int maxHealth;
		bool isSessionHost;
		int rank;
		int RP;
		int cash;
		rage::netPlayerData* netData;
		int visible;

		struct
		{
			ImVec2 name;
			ImVec2 head;
			ImVec2 neck1;
			ImVec2 spine0;
			ImVec2 spine1;
			ImVec2 spine2;
			ImVec2 spine3;
			ImVec2 pelvis;
			ImVec2 SKEL_L_Thigh;
			ImVec2 SKEL_L_Calf;
			ImVec2 SKEL_L_Foot;
			ImVec2 SKEL_L_Toe0;
			ImVec2 SKEL_R_Thigh;
			ImVec2 SKEL_R_Calf;
			ImVec2 SKEL_R_Foot;
			ImVec2 SKEL_R_Toe0;
			ImVec2 SKEL_L_Clavicle;
			ImVec2 SKEL_L_UpperArm;
			ImVec2 SKEL_L_Forearm;
			ImVec2 SKEL_L_Hand;
			ImVec2 SKEL_R_Clavicle;
			ImVec2 SKEL_R_UpperArm;
			ImVec2 SKEL_R_Forearm;
			ImVec2 SKEL_R_Hand;
		}skeleton;
	};

	inline rage::CNetGamePlayer* local;
	inline rage::CPlayerInfo* localInfo;
	inline rage::CPed* localCPed;
	inline rage::CAutomobile* lastVehicle;
	inline rage::CHandlingData* lastVehicleHandling;
	inline Vehicle vehicle;
	inline Ped scriptIndex;

	inline Ped localPed;
	inline int localIndex;

	inline playerList players[32];

	Vector3 findBlip();
	Vector3 FromHSB(float hue, float saturation, float brightness);
	extern void spawnvehicle(const char* name);
	extern void addweapons();
	extern bool owns_veh(Ped ped);

	//input declaration

#define VK_NOTHING	0x00				/*NULL*/
#define VK_KEY_0	0x30                //('0')	0
#define VK_KEY_1	0x31                //('1')	1
#define VK_KEY_2	0x32                //('2')	2
#define VK_KEY_3	0x33                //('3')	3
#define VK_KEY_4	0x34                //('4')	4
#define VK_KEY_5	0x35                //('5')	5
#define VK_KEY_6	0x36                //('6')	6
#define VK_KEY_7	0x37                //('7')	7
#define VK_KEY_8	0x38                //('8')	8
#define VK_KEY_9	0x39                //('9')	9
#define VK_KEY_A	0x41                //('A')	A
#define VK_KEY_B	0x42                //('B')	B
#define VK_KEY_C	0x43                //('C')	C
#define VK_KEY_D	0x44                //('D')	D
#define VK_KEY_E	0x45                //('E')	E
#define VK_KEY_F	0x46                //('F')	F
#define VK_KEY_G	0x47                //('G')	G
#define VK_KEY_H	0x48                //('H')	H
#define VK_KEY_I	0x49                //('I')	I
#define VK_KEY_J	0x4A                //('J')	J
#define VK_KEY_K	0x4B                //('K')	K
#define VK_KEY_L	0x4C                //('L')	L
#define VK_KEY_M	0x4D                //('M')	M 
#define VK_KEY_N	0x4E                //('N')	N
#define VK_KEY_O	0x4F                //('O')	O
#define VK_KEY_P	0x50                //('P')	P
#define VK_KEY_Q	0x51                //('Q')	Q
#define VK_KEY_R	0x52                //('R')	R
#define VK_KEY_S	0x53                //('S')	S
#define VK_KEY_T	0x54                //('T')	T
#define VK_KEY_U	0x55                //('U')	U
#define VK_KEY_V	0x56                //('V')	V
#define VK_KEY_W	0x57                //('W')	W
#define VK_KEY_X	0x58                //('X')	X
#define VK_KEY_Y	0x59                //('Y')	Y
#define VK_KEY_Z	0x5A                //('Z')	Z

//ped bone ids for skeleton esp

	enum ePedBoneId : uint16_t
	{
		SKEL_ROOT = 0x0,
		SKEL_Pelvis = 0x2E28,
		SKEL_L_Thigh = 0xE39F,
		SKEL_L_Calf = 0xF9BB,
		SKEL_L_Foot = 0x3779,
		SKEL_L_Toe0 = 0x83C,
		EO_L_Foot = 0x84C5,
		EO_L_Toe = 0x68BD,
		IK_L_Foot = 0xFEDD,
		PH_L_Foot = 0xE175,
		MH_L_Knee = 0xB3FE,
		SKEL_R_Thigh = 0xCA72,
		SKEL_R_Calf = 0x9000,
		SKEL_R_Foot = 0xCC4D,
		SKEL_R_Toe0 = 0x512D,
		EO_R_Foot = 0x1096,
		EO_R_Toe = 0x7163,
		IK_R_Foot = 0x8AAE,
		PH_R_Foot = 0x60E6,
		MH_R_Knee = 0x3FCF,
		RB_L_ThighRoll = 0x5C57,
		RB_R_ThighRoll = 0x192A,
		SKEL_Spine_Root = 0xE0FD,
		SKEL_Spine0 = 0x5C01,
		SKEL_Spine1 = 0x60F0,
		SKEL_Spine2 = 0x60F1,
		SKEL_Spine3 = 0x60F2,
		SKEL_L_Clavicle = 0xFCD9,
		SKEL_L_UpperArm = 0xB1C5,
		SKEL_L_Forearm = 0xEEEB,
		SKEL_L_Hand = 0x49D9,
		SKEL_L_Finger00 = 0x67F2,
		SKEL_L_Finger01 = 0xFF9,
		SKEL_L_Finger02 = 0xFFA,
		SKEL_L_Finger10 = 0x67F3,
		SKEL_L_Finger11 = 0x1049,
		SKEL_L_Finger12 = 0x104A,
		SKEL_L_Finger20 = 0x67F4,
		SKEL_L_Finger21 = 0x1059,
		SKEL_L_Finger22 = 0x105A,
		SKEL_L_Finger30 = 0x67F5,
		SKEL_L_Finger31 = 0x1029,
		SKEL_L_Finger32 = 0x102A,
		SKEL_L_Finger40 = 0x67F6,
		SKEL_L_Finger41 = 0x1039,
		SKEL_L_Finger42 = 0x103A,
		PH_L_Hand = 0xEB95,
		IK_L_Hand = 0x8CBD,
		RB_L_ForeArmRoll = 0xEE4F,
		RB_L_ArmRoll = 0x1470,
		MH_L_Elbow = 0x58B7,
		SKEL_R_Clavicle = 0x29D2,
		SKEL_R_UpperArm = 0x9D4D,
		SKEL_R_Forearm = 0x6E5C,
		SKEL_R_Hand = 0xDEAD,
		SKEL_R_Finger00 = 0xE5F2,
		SKEL_R_Finger01 = 0xFA10,
		SKEL_R_Finger02 = 0xFA11,
		SKEL_R_Finger10 = 0xE5F3,
		SKEL_R_Finger11 = 0xFA60,
		SKEL_R_Finger12 = 0xFA61,
		SKEL_R_Finger20 = 0xE5F4,
		SKEL_R_Finger21 = 0xFA70,
		SKEL_R_Finger22 = 0xFA71,
		SKEL_R_Finger30 = 0xE5F5,
		SKEL_R_Finger31 = 0xFA40,
		SKEL_R_Finger32 = 0xFA41,
		SKEL_R_Finger40 = 0xE5F6,
		SKEL_R_Finger41 = 0xFA50,
		SKEL_R_Finger42 = 0xFA51,
		PH_R_Hand = 0x6F06,
		IK_R_Hand = 0x188E,
		RB_R_ForeArmRoll = 0xAB22,
		RB_R_ArmRoll = 0x90FF,
		MH_R_Elbow = 0xBB0,
		SKEL_Neck_1 = 0x9995,
		SKEL_Head = 0x796E,
		IK_Head = 0x322C,
		FACIAL_facialRoot = 0xFE2C,
		FB_L_Brow_Out_000 = 0xE3DB,
		FB_L_Lid_Upper_000 = 0xB2B6,
		FB_L_Eye_000 = 0x62AC,
		FB_L_CheekBone_000 = 0x542E,
		FB_L_Lip_Corner_000 = 0x74AC,
		FB_R_Lid_Upper_000 = 0xAA10,
		FB_R_Eye_000 = 0x6B52,
		FB_R_CheekBone_000 = 0x4B88,
		FB_R_Brow_Out_000 = 0x54C,
		FB_R_Lip_Corner_000 = 0x2BA6,
		FB_Brow_Centre_000 = 0x9149,
		FB_UpperLipRoot_000 = 0x4ED2,
		FB_UpperLip_000 = 0xF18F,
		FB_L_Lip_Top_000 = 0x4F37,
		FB_R_Lip_Top_000 = 0x4537,
		FB_Jaw_000 = 0xB4A0,
		FB_LowerLipRoot_000 = 0x4324,
		FB_LowerLip_000 = 0x508F,
		FB_L_Lip_Bot_000 = 0xB93B,
		FB_R_Lip_Bot_000 = 0xC33B,
		FB_Tongue_000 = 0xB987,
		RB_Neck_1 = 0x8B93,
		SPR_L_Breast = 0xFC8E,
		SPR_R_Breast = 0x885F,
		IK_Root = 0xDD1C,
		SKEL_Neck_2 = 0x5FD4,
		SKEL_Pelvis1 = 0xD003,
		SKEL_PelvisRoot = 0x45FC,
		SKEL_SADDLE = 0x9524,
		MH_L_CalfBack = 0x1013,
		MH_L_ThighBack = 0x600D,
		SM_L_Skirt = 0xC419,
		MH_R_CalfBack = 0xB013,
		MH_R_ThighBack = 0x51A3,
		SM_R_Skirt = 0x7712,
		SM_M_BackSkirtRoll = 0xDBB,
		SM_L_BackSkirtRoll = 0x40B2,
		SM_R_BackSkirtRoll = 0xC141,
		SM_M_FrontSkirtRoll = 0xCDBB,
		SM_L_FrontSkirtRoll = 0x9B69,
		SM_R_FrontSkirtRoll = 0x86F1,
		SM_CockNBalls_ROOT = 0xC67D,
		SM_CockNBalls = 0x9D34,
		MH_L_Finger00 = 0x8C63,
		MH_L_FingerBulge00 = 0x5FB8,
		MH_L_Finger10 = 0x8C53,
		MH_L_FingerTop00 = 0xA244,
		MH_L_HandSide = 0xC78A,
		MH_Watch = 0x2738,
		MH_L_Sleeve = 0x933C,
		MH_R_Finger00 = 0x2C63,
		MH_R_FingerBulge00 = 0x69B8,
		MH_R_Finger10 = 0x2C53,
		MH_R_FingerTop00 = 0xEF4B,
		MH_R_HandSide = 0x68FB,
		MH_R_Sleeve = 0x92DC,
		FACIAL_jaw = 0xB21,
		FACIAL_underChin = 0x8A95,
		FACIAL_L_underChin = 0x234E,
		FACIAL_chin = 0xB578,
		FACIAL_chinSkinBottom = 0x98BC,
		FACIAL_L_chinSkinBottom = 0x3E8F,
		FACIAL_R_chinSkinBottom = 0x9E8F,
		FACIAL_tongueA = 0x4A7C,
		FACIAL_tongueB = 0x4A7D,
		FACIAL_tongueC = 0x4A7E,
		FACIAL_tongueD = 0x4A7F,
		FACIAL_tongueE = 0x4A80,
		FACIAL_L_tongueE = 0x35F2,
		FACIAL_R_tongueE = 0x2FF2,
		FACIAL_L_tongueD = 0x35F1,
		FACIAL_R_tongueD = 0x2FF1,
		FACIAL_L_tongueC = 0x35F0,
		FACIAL_R_tongueC = 0x2FF0,
		FACIAL_L_tongueB = 0x35EF,
		FACIAL_R_tongueB = 0x2FEF,
		FACIAL_L_tongueA = 0x35EE,
		FACIAL_R_tongueA = 0x2FEE,
		FACIAL_chinSkinTop = 0x7226,
		FACIAL_L_chinSkinTop = 0x3EB3,
		FACIAL_chinSkinMid = 0x899A,
		FACIAL_L_chinSkinMid = 0x4427,
		FACIAL_L_chinSide = 0x4A5E,
		FACIAL_R_chinSkinMid = 0xF5AF,
		FACIAL_R_chinSkinTop = 0xF03B,
		FACIAL_R_chinSide = 0xAA5E,
		FACIAL_R_underChin = 0x2BF4,
		FACIAL_L_lipLowerSDK = 0xB9E1,
		FACIAL_L_lipLowerAnalog = 0x244A,
		FACIAL_L_lipLowerThicknessV = 0xC749,
		FACIAL_L_lipLowerThicknessH = 0xC67B,
		FACIAL_lipLowerSDK = 0x7285,
		FACIAL_lipLowerAnalog = 0xD97B,
		FACIAL_lipLowerThicknessV = 0xC5BB,
		FACIAL_lipLowerThicknessH = 0xC5ED,
		FACIAL_R_lipLowerSDK = 0xA034,
		FACIAL_R_lipLowerAnalog = 0xC2D9,
		FACIAL_R_lipLowerThicknessV = 0xC6E9,
		FACIAL_R_lipLowerThicknessH = 0xC6DB,
		FACIAL_nose = 0x20F1,
		FACIAL_L_nostril = 0x7322,
		FACIAL_L_nostrilThickness = 0xC15F,
		FACIAL_noseLower = 0xE05A,
		FACIAL_L_noseLowerThickness = 0x79D5,
		FACIAL_R_noseLowerThickness = 0x7975,
		FACIAL_noseTip = 0x6A60,
		FACIAL_R_nostril = 0x7922,
		FACIAL_R_nostrilThickness = 0x36FF,
		FACIAL_noseUpper = 0xA04F,
		FACIAL_L_noseUpper = 0x1FB8,
		FACIAL_noseBridge = 0x9BA3,
		FACIAL_L_nasolabialFurrow = 0x5ACA,
		FACIAL_L_nasolabialBulge = 0xCD78,
		FACIAL_L_cheekLower = 0x6907,
		FACIAL_L_cheekLowerBulge1 = 0xE3FB,
		FACIAL_L_cheekLowerBulge2 = 0xE3FC,
		FACIAL_L_cheekInner = 0xE7AB,
		FACIAL_L_cheekOuter = 0x8161,
		FACIAL_L_eyesackLower = 0x771B,
		FACIAL_L_eyeball = 0x1744,
		FACIAL_L_eyelidLower = 0x998C,
		FACIAL_L_eyelidLowerOuterSDK = 0xFE4C,
		FACIAL_L_eyelidLowerOuterAnalog = 0xB9AA,
		FACIAL_L_eyelashLowerOuter = 0xD7F6,
		FACIAL_L_eyelidLowerInnerSDK = 0xF151,
		FACIAL_L_eyelidLowerInnerAnalog = 0x8242,
		FACIAL_L_eyelashLowerInner = 0x4CCF,
		FACIAL_L_eyelidUpper = 0x97C1,
		FACIAL_L_eyelidUpperOuterSDK = 0xAF15,
		FACIAL_L_eyelidUpperOuterAnalog = 0x67FA,
		FACIAL_L_eyelashUpperOuter = 0x27B7,
		FACIAL_L_eyelidUpperInnerSDK = 0xD341,
		FACIAL_L_eyelidUpperInnerAnalog = 0xF092,
		FACIAL_L_eyelashUpperInner = 0x9B1F,
		FACIAL_L_eyesackUpperOuterBulge = 0xA559,
		FACIAL_L_eyesackUpperInnerBulge = 0x2F2A,
		FACIAL_L_eyesackUpperOuterFurrow = 0xC597,
		FACIAL_L_eyesackUpperInnerFurrow = 0x52A7,
		FACIAL_forehead = 0x9218,
		FACIAL_L_foreheadInner = 0x843,
		FACIAL_L_foreheadInnerBulge = 0x767C,
		FACIAL_L_foreheadOuter = 0x8DCB,
		FACIAL_skull = 0x4221,
		FACIAL_foreheadUpper = 0xF7D6,
		FACIAL_L_foreheadUpperInner = 0xCF13,
		FACIAL_L_foreheadUpperOuter = 0x509B,
		FACIAL_R_foreheadUpperInner = 0xCEF3,
		FACIAL_R_foreheadUpperOuter = 0x507B,
		FACIAL_L_temple = 0xAF79,
		FACIAL_L_ear = 0x19DD,
		FACIAL_L_earLower = 0x6031,
		FACIAL_L_masseter = 0x2810,
		FACIAL_L_jawRecess = 0x9C7A,
		FACIAL_L_cheekOuterSkin = 0x14A5,
		FACIAL_R_cheekLower = 0xF367,
		FACIAL_R_cheekLowerBulge1 = 0x599B,
		FACIAL_R_cheekLowerBulge2 = 0x599C,
		FACIAL_R_masseter = 0x810,
		FACIAL_R_jawRecess = 0x93D4,
		FACIAL_R_ear = 0x1137,
		FACIAL_R_earLower = 0x8031,
		FACIAL_R_eyesackLower = 0x777B,
		FACIAL_R_nasolabialBulge = 0xD61E,
		FACIAL_R_cheekOuter = 0xD32,
		FACIAL_R_cheekInner = 0x737C,
		FACIAL_R_noseUpper = 0x1CD6,
		FACIAL_R_foreheadInner = 0xE43,
		FACIAL_R_foreheadInnerBulge = 0x769C,
		FACIAL_R_foreheadOuter = 0x8FCB,
		FACIAL_R_cheekOuterSkin = 0xB334,
		FACIAL_R_eyesackUpperInnerFurrow = 0x9FAE,
		FACIAL_R_eyesackUpperOuterFurrow = 0x140F,
		FACIAL_R_eyesackUpperInnerBulge = 0xA359,
		FACIAL_R_eyesackUpperOuterBulge = 0x1AF9,
		FACIAL_R_nasolabialFurrow = 0x2CAA,
		FACIAL_R_temple = 0xAF19,
		FACIAL_R_eyeball = 0x1944,
		FACIAL_R_eyelidUpper = 0x7E14,
		FACIAL_R_eyelidUpperOuterSDK = 0xB115,
		FACIAL_R_eyelidUpperOuterAnalog = 0xF25A,
		FACIAL_R_eyelashUpperOuter = 0xE0A,
		FACIAL_R_eyelidUpperInnerSDK = 0xD541,
		FACIAL_R_eyelidUpperInnerAnalog = 0x7C63,
		FACIAL_R_eyelashUpperInner = 0x8172,
		FACIAL_R_eyelidLower = 0x7FDF,
		FACIAL_R_eyelidLowerOuterSDK = 0x1BD,
		FACIAL_R_eyelidLowerOuterAnalog = 0x457B,
		FACIAL_R_eyelashLowerOuter = 0xBE49,
		FACIAL_R_eyelidLowerInnerSDK = 0xF351,
		FACIAL_R_eyelidLowerInnerAnalog = 0xE13,
		FACIAL_R_eyelashLowerInner = 0x3322,
		FACIAL_L_lipUpperSDK = 0x8F30,
		FACIAL_L_lipUpperAnalog = 0xB1CF,
		FACIAL_L_lipUpperThicknessH = 0x37CE,
		FACIAL_L_lipUpperThicknessV = 0x38BC,
		FACIAL_lipUpperSDK = 0x1774,
		FACIAL_lipUpperAnalog = 0xE064,
		FACIAL_lipUpperThicknessH = 0x7993,
		FACIAL_lipUpperThicknessV = 0x7981,
		FACIAL_L_lipCornerSDK = 0xB1C,
		FACIAL_L_lipCornerAnalog = 0xE568,
		FACIAL_L_lipCornerThicknessUpper = 0x7BC,
		FACIAL_L_lipCornerThicknessLower = 0xDD42,
		FACIAL_R_lipUpperSDK = 0x7583,
		FACIAL_R_lipUpperAnalog = 0x51CF,
		FACIAL_R_lipUpperThicknessH = 0x382E,
		FACIAL_R_lipUpperThicknessV = 0x385C,
		FACIAL_R_lipCornerSDK = 0xB3C,
		FACIAL_R_lipCornerAnalog = 0xEE0E,
		FACIAL_R_lipCornerThicknessUpper = 0x54C3,
		FACIAL_R_lipCornerThicknessLower = 0x2BBA,
		MH_MulletRoot = 0x3E73,
		MH_MulletScaler = 0xA1C2,
		MH_Hair_Scale = 0xC664,
		MH_Hair_Crown = 0x1675,
		SM_Torch = 0x8D6,
		FX_Light = 0x8959,
		FX_Light_Scale = 0x5038,
		FX_Light_Switch = 0xE18E,
		BagRoot = 0xAD09,
		BagPivotROOT = 0xB836,
		BagPivot = 0x4D11,
		BagBody = 0xAB6D,
		BagBone_R = 0x937,
		BagBone_L = 0x991,
		SM_LifeSaver_Front = 0x9420,
		SM_R_Pouches_ROOT = 0x2962,
		SM_R_Pouches = 0x4141,
		SM_L_Pouches_ROOT = 0x2A02,
		SM_L_Pouches = 0x4B41,
		SM_Suit_Back_Flapper = 0xDA2D,
		SPR_CopRadio = 0x8245,
		SM_LifeSaver_Back = 0x2127,
		MH_BlushSlider = 0xA0CE,
		SKEL_Tail_01 = 0x347,
		SKEL_Tail_02 = 0x348,
		MH_L_Concertina_B = 0xC988,
		MH_L_Concertina_A = 0xC987,
		MH_R_Concertina_B = 0xC8E8,
		MH_R_Concertina_A = 0xC8E7,
		MH_L_ShoulderBladeRoot = 0x8711,
		MH_L_ShoulderBlade = 0x4EAF,
		MH_R_ShoulderBladeRoot = 0x3A0A,
		MH_R_ShoulderBlade = 0x54AF,
		FB_R_Ear_000 = 0x6CDF,
		SPR_R_Ear = 0x63B6,
		FB_L_Ear_000 = 0x6439,
		SPR_L_Ear = 0x5B10,
		FB_TongueA_000 = 0x4206,
		FB_TongueB_000 = 0x4207,
		FB_TongueC_000 = 0x4208,
		SKEL_L_Toe1 = 0x1D6B,
		SKEL_R_Toe1 = 0xB23F,
		SKEL_Tail_03 = 0x349,
		SKEL_Tail_04 = 0x34A,
		SKEL_Tail_05 = 0x34B,
		SPR_Gonads_ROOT = 0xBFDE,
		SPR_Gonads = 0x1C00,
		FB_L_Brow_Out_001 = 0xE3DB,
		FB_L_Lid_Upper_001 = 0xB2B6,
		FB_L_Eye_001 = 0x62AC,
		FB_L_CheekBone_001 = 0x542E,
		FB_L_Lip_Corner_001 = 0x74AC,
		FB_R_Lid_Upper_001 = 0xAA10,
		FB_R_Eye_001 = 0x6B52,
		FB_R_CheekBone_001 = 0x4B88,
		FB_R_Brow_Out_001 = 0x54C,
		FB_R_Lip_Corner_001 = 0x2BA6,
		FB_Brow_Centre_001 = 0x9149,
		FB_UpperLipRoot_001 = 0x4ED2,
		FB_UpperLip_001 = 0xF18F,
		FB_L_Lip_Top_001 = 0x4F37,
		FB_R_Lip_Top_001 = 0x4537,
		FB_Jaw_001 = 0xB4A0,
		FB_LowerLipRoot_001 = 0x4324,
		FB_LowerLip_001 = 0x508F,
		FB_L_Lip_Bot_001 = 0xB93B,
		FB_R_Lip_Bot_001 = 0xC33B,
		FB_Tongue_001 = 0xB987
	};


	static const Hash Weapons[] =
	{
		0x92A27487, 0x958A4A8F, 0xF9E6AA4B, 0x84BD7BFD, 0xA2719263, 0x8BB05FD7, 0x440E4788, 0x4E875F73, 0xF9DCBF2D, 0xD8DF3C3C, 0x99B507EA, 0xDD5DF8D9, 0xDFE37640, 0x678B81B1, 0x19044EE0, 0xCD274149, 0x94117305, 0x3813FC08, // Melee
		0x1B06D571, 0xBFE256D4, 0x5EF9FEC4, 0x22D8FE39, 0x3656C8C1, 0x99AEEB3B, 0xBFD21232, 0x88374054, 0xD205520E, 0x83839C4, 0x47757124, 0xDC4DB296, 0xC1B3C3D1, 0xCB96392F, 0x97EA20B8, // Handguns
		0x13532244, 0x2BE6766B, 0x2BE6766B, 0xEFE7E2DF, 0x0A3D4D34, 0xDB1AA450, 0xBD248B55, // Submachine Guns
		0x1D073A89, 0x555AF99A, 0x7846A318, 0xE284C527, 0x9D61E50F, 0xA89CB99E, 0x3AABBBAA, 0xEF951FBB, 0x12E82D3D, // Shotguns
		0xBFEFFF6D, 0x394F415C, 0x83BF0278, 0xFAD1F1C9, 0xAF113F99, 0xC0A3098D, 0x969C3D67, 0x7F229F94, 0x84D6FAFD, 0x624FE830, // Assault Rifles
		0x9D07F764, 0x7FD62962, 0xDBBD7280, 0x61012683, // Light Machine Guns
		0x05FC3C11, 0x0C472FE2, 0xA914799, 0xC734385A, 0x6A6C02E0, // Sniper Rifles
		0xB1CA77B1, 0xA284510B, 0x4DD2DC56, 0x42BF8A85, 0x7F7497E5, 0x6D544C99, 0x63AB0442, 0x0781FE4A, // Heavy Weapons
		0x93E220BD, 0xA0973D5E, 0x24B17070, 0x2C3731D9, 0xAB564B93, 0x787F0BB, 0xBA45E8B8, 0x23C9F95C, 0xFDBC8A50, 0x497FACC3, // Throwables
		0x34A67B97, 0xFBAB5776, 0x060EC506, 0xAF3696A1, 0x476BF155, 0xB62D1F67, // Miscellaneous 
	};

	//I really don't care to format this, it will compile the same regardless.

	static const char* vehicleModels[] = {
"CARACARA2",
"DRAFTER",
"DYNASTY",
"EMERUS",
"GAUNTLET3",
"GAUNTLET4",
"HELLION",
"ISSI7",
"JUGULAR",
"KRIEGER",
"LOCUST",
"NEBULA",
"NEO",
"NOVAK",
"PARAGON",
"PARAGON2",
"PEYOTE2",
"RROCKET",
"S80",
"THRAX",
"ZION3",
"ZORRUSSO",
"BRUISER", "BRUISER2", "BRUISER3", "BRUTUS", "BRUTUS2", "BRUTUS3", "CERBERUS",
"CERBERUS2", "CERBERUS3", "CLIQUE", "DEATHBIKE", "DEATHBIKE2", "DEATHBIKE3", "DEVESTE",
"DEVIANT", "DOMINATOR4", "DOMINATOR5", "DOMINATOR6", "IMPALER", "IMPALER2", "IMPALER3", "IMPALER4",
"IMPERATOR", "IMPERATOR2", "IMPERATOR3", "ISSI4", "ISSI5", "ISSI6", "ITALIGTO", "MONSTER3",
"MONSTER4", "MONSTER5", "RCBANDITO", "SCARAB2", "SCARAB3", "SCHLAGEN", "SLAMVAN4", "SLAMVAN5", "SLAMVAN6", "TOROS", "TULIP", "VAMOS",
"ZR380", "ZR3802", "ZR3803",
"BLIMP3", "STRIKEFORCE", "FREECRAWLER", "MENACER", "MULE4", "OPPRESSOR2", "PATRIOT2", "PBUS2",
"POUNDER2", "SCRAMJET", "SPEEDO4", "STAFFORD", "SWINGER", "TERBYTE"
"CARACARA", "CHEBUREK", "DOMINATOR3", "ELLIE", "ENTITY2", "FAGALOA", "FLASHGT",
"GB200", "HOTRING", "ISSI3", "JESTER3", "MICHELLI", "SEASPARROW", "TAIPAN", "TEZERACT",
"TYRANT",
"AUTARCH", "AVENGER", "BARRAGE", "CHERNOBOG", "COMET4", "COMET5",
"DELUXO", "GT500", "HERMES", "HUSTLER", "KAMACHO", "KHANJALI",
"NEON", "PARIAH", "RAIDEN", "REVOLTER", "RIATA", "RIOT2", "SAVESTRA",
"SC1", "SENTINEL3", "STREITER", "STROMBERG", "THRUSTER", "VISERIS",
"VOLATOL", "YOSEMITE", "Z190",
"HAVOK", "RAPIDGT3", "RETINUE", "VIGILANTE", "VISIONE", "CYCLONE", "ALPHAZ1",
"BOMBUSHKA", "HOWARD", "HUNTER", "MICROLIGHT", "MOGUL", "MOLOTOK", "NOKOTA", "PYRO",
"ROGUE", "SEABREEZE", "STARLING", "TULA",
"APC", "ARDENT", "CADDY3", "CHEETAH2", "DUNE3", "HALFTRACK", "HAULER2",
"INSURGENT3", "NIGHTSHARK", "PHANTOM3", "TAMPA3", "TECHNICAL3",
"TORENO", "TRAILERLARGE", "TRAILERS4", "TRAILERSMALL2", "VAGNER", "XA21",
"BF400", "BRIOSO", "CLIFFHANGER", "CONTENDER", "GARGOYLE", "LE7B", "LYNX",
"OMNIS", "RALLYTRUCK", "SHEAVA", "TAMPA2", "TROPHYTRUCK", "TROPHYTRUCK2",
"TROPOS", "TYRUS",
"BLAZER5", "BOXVILLE5", "COMET3", "DIABLOUS", "DIABLOUS2", "DUNE4", "DUNE5",
"ELEGY", "FCR", "FCR2", "ITALIGTB", "ITALIGTB2", "NERO", "NERO2", "PENETRATOR",
"PHANTOM2", "RUINER2", "SPECTER", "SPECTER2", "TECHNICAL2", "TEMPESTA", "VOLTIC2",
"WASTELANDER",
"DINGHY", "DINGHY2", "DINGHY3", "DINGHY4", "JETMAX",
"MARQUIS", "PREDATOR", "SEASHARK", "SEASHARK2", "SEASHARK3",
"SPEEDER", "SPEEDER2", "SQUALO", "SUBMERSIBLE", "SUBMERSIBLE2",
"SUNTRAP", "TORO", "TORO2", "TROPIC", "TROPIC2",
"TUG",
"BENSON", "BIFF", "HAULER", "MULE", "MULE2",
"MULE3", "PACKER", "PHANTOM", "POUNDER", "STOCKADE",
"STOCKADE3",
"BLISTA", "BRIOSO", "DILETTANTE", "DILETTANTE2", "ISSI2",
"PANTO", "PRAIRIE", "RHAPSODY",
"COGCABRIO", "EXEMPLAR", "F620", "FELON", "FELON2",
"JACKAL", "ORACLE", "ORACLE2", "SENTINEL", "SENTINEL2",
"WINDSOR", "WINDSOR2", "ZION", "ZION2",
"BMX", "CRUISER", "FIXTER", "SCORCHER", "TRIBIKE",
"TRIBIKE2", "TRIBIKE3",
"AMBULANCE", "FBI", "FBI2", "FIRETRUK", "LGUARD",
"PBUS", "PRANGER", "POLICE", "POLICE2", "POLICE3",
"POLICE4", "POLICEB", "POLICEOLD1", "POLICEOLD2", "POLICET",
"SHERIFF", "SHERIFF2", "RIOT",
"BESRA", "CARGOPLANE", "CUBAN800", "DODO", "DUSTER",
"HYDRA", "JET", "LAZER", "LUXOR", "LUXOR2",
"MAMMATUS", "MILJET", "NIMBUS", "SHAMAL", "STUNT",
"TITAN", "VELUM", "VELUM2", "VESTRA",
"ANNIHILATOR", "BLIMP", "BLIMP2", "BUZZARD", "BUZZARD2",
"CARGOBOB", "CARGOBOB2", "CARGOBOB3", "CARGOBOB4", "FROGGER",
"FROGGER2", "MAVERICK", "POLMAV", "SAVAGE", "SKYLIFT",
"SUPERVOLITO", "SUPERVOLITO2", "SWIFT", "SWIFT2", "VALKYRIE",
"VALKYRIE2", "VOLATUS",
"AKUMA", "AVARUS", "BAGGER", "BATI", "BATI2",
"BF400", "CARBONRS", "CHIMERA", "CLIFFHANGER", "DAEMON",
"DAEMON2", "DEFILER", "DOUBLE", "ENDURO", "ESSKEY",
"FAGGIO", "FAGGIO2", "FAGGIO3", "GARGOYLE", "HAKUCHOU",
"HAKUCHOU2", "HEXER", "INNOVATION", "LECTRO", "MANCHEZ",
"NEMESIS", "NIGHTBLADE", "PCJ", "RATBIKE", "RUFFIAN",
"SANCHEZ", "SANCHEZ2", "SANCTUS", "SHOTARO", "SOVEREIGN",
"THRUST", "VADER", "VINDICATOR", "VORTEX", "WOLFSBANE",
"ZOMBIEA", "ZOMBIEB", "DIABLOUS", "DIABLOUS2", "FCR",
"FCR2", "OPPRESSOR",

"ADDER", "BANSHEE2", "BULLET", "CHEETAH", "ENTITYXF",
"FMJ", "SHEAVA", "INFERNUS", "NERO", "NERO2", "OSIRIS", "LE7B",
"ITALIGTB", "ITALIGTB2", "PFISTER811", "PROTOTIPO", "REAPER", "SULTANRS", "T20",
"TEMPESTA", "TURISMOR", "TYRUS", "VACCA", "VOLTIC", "ZENTORNO", "VOLTIC2", "PENETRATOR", "GP1"
"ALPHA", "BANSHEE", "BESTIAGTS", "BLISTA2", "BLISTA3",
"BUFFALO", "BUFFALO2", "BUFFALO3", "CARBONIZZARE", "COMET2",
"COQUETTE", "ELEGY", "ELEGY2", "FELTZER2", "FUROREGT", "FUSILADE",
"FUTO", "JESTER", "JESTER2", "KHAMELION", "KURUMA",
"KURUMA2", "LYNX", "MASSACRO", "MASSACRO2", "NINEF",
"NINEF2", "OMNIS", "PENUMBRA", "RAPIDGT", "RAPIDGT2",
"RAPTOR", "SCHAFTER3", "SCHAFTER4", "SCHWARZER", "SEVEN70",
"SULTAN", "SURANO", "SPECTER", "SPECTER2", "TAMPA2", "TROPOS", "VERLIERER2",
"RUINER2", "PHANTOM2", "RUSTON",
"BFINJECTION", "BIFTA", "BLAZER", "BLAZER2", "BLAZER3",
"BLAZER4", "BODHI2", "BRAWLER", "DLOADER", "DUBSTA3",
"DUNE", "DUNE2", "INSURGENT", "INSURGENT2", "KALAHARI",
"MARSHALL", "MESA3", "MONSTER", "RANCHERXL", "RANCHERXL2",
"REBEL", "REBEL2", "SANDKING", "SANDKING2", "TECHNICAL",
"TROPHYTRUCK", "TROPHYTRUCK2", "TECHNICAL2",
"BLAZER5",
"BTYPE", "BTYPE2", "BTYPE3", "CASCO", "COQUETTE2",
"FELTZER3", "JB700", "MAMBA", "MANANA", "MONROE",
"PEYOTE", "PIGALLE", "STINGER", "STINGERGT", "TORNADO",
"TORNADO2", "TORNADO3", "TORNADO4", "TORNADO5", "TORNADO6",
"ZTYPE", "INFERNUS2", "TURISMO2",
"BALLER", "BALLER2", "BALLER3", "BALLER4", "BALLER5",
"BALLER6", "BJXL", "CAVALCADE", "CAVALCADE2", "CONTENDER",
"DUBSTA", "DUBSTA2", "FQ2", "GRANGER", "GRESLEY",
"HABANERO", "HUNTLEY", "LANDSTALKER", "MESA", "MESA2",
"PATRIOT", "RADI", "ROCOTO", "SEMINOLE", "SERRANO",
"XLS", "XLS2",
"ASEA", "ASEA2", "ASTEROPE", "COG55", "COG552",
"COGNOSCENTI", "COGNOSCENTI2", "EMPEROR", "EMPEROR2", "EMPEROR3",
"FUGITIVE", "GLENDALE", "INGOT", "INTRUDER", "LIMO2",
"PREMIER", "PRIMO", "PRIMO2", "REGINA", "ROMERO",
"SCHAFTER2", "SCHAFTER5", "SCHAFTER6", "STANIER", "STRATUM",
"STRETCH", "SUPERD", "SURGE", "TAILGATER", "WARRENER",
"WASHINGTON",
"BLADE", "BUCCANEER", "BUCCANEER2", "CHINO", "CHINO2",
"COQUETTE3", "DOMINATOR", "DOMINATOR2", "DUKES", "DUKES2",
"GAUNTLET", "GAUNTLET2", "FACTION", "FACTION2", "FACTION3",
"HOTKNIFE", "LURCHER", "MOONBEAM", "MOONBEAM2", "NIGHTSHADE",
"PHOENIX", "PICADOR", "RATLOADER", "RATLOADER2", "RUINER", "RUINER2", "RUINER3",
"SABREGT", "SABREGT2", "SLAMVAN", "SLAMVAN2", "SLAMVAN3",
"STALION", "STALION2", "TAMPA", "VIGERO", "VIRGO",
"VIRGO2", "VIRGO3", "VOODOO", "VOODOO2",
"BISON", "BISON2", "BISON3", "BOBCATXL", "BOXVILLE",
"BOXVILLE2", "BOXVILLE3", "BOXVILLE4", "BURRITO", "BURRITO2",
"BURRITO3", "BURRITO4", "BURRITO5", "CAMPER", "GBURRITO",
"GBURRITO2", "JOURNEY", "MINIVAN", "MINIVAN2", "PARADISE",
"PONY", "PONY2", "RUMPO", "RUMPO2", "RUMPO3",
"SPEEDO", "SPEEDO2", "SURFER", "SURFER2", "TACO",
"YOUGA", "YOUGA2",
	"BARRACKS", "BARRACKS2", "BARRACKS3", "CRUSADER", "RHINO",
	"AIRTUG", "CADDY", "CADDY2", "DOCKTUG", "FORKLIFT",
	"MOWER", "RIPLEY", "SADLER", "SADLER2", "SCRAP",
	"TOWTRUCK", "TOWTRUCK2", "TRACTOR", "TRACTOR2", "TRACTOR3",
	"UTILLITRUCK", "UTILLITRUCK2", "UTILLITRUCK3"
	"CABLECAR", "FREIGHT", "FREIGHTCAR", "FREIGHTCONT1", "FREIGHTCONT2",
	"FREIGHTGRAIN", "METROTRAIN", "TANKERCAR"
	"AIRBUS", "BRICKADE", "BUS", "COACH", "RALLYTRUCK",
	"RENTALBUS", "TAXI", "TOURBUS", "TRASH", "TRASH2",
	"WASTELANDER",
	"BULLDOZER", "CUTTER", "DUMP", "FLATBED", "GUARDIAN",
	"HANDLER", "MIXER", "MIXER2", "RUBBLE", "TIPTRUCK",
	"TIPTRUCK2",
	"ARMYTANKER", "ARMYTRAILER", "ARMYTRAILER2", "BALETRAILER", "BOATTRAILER",
	"DOCKTRAILER", "FREIGHTTRAILER", "GRAINTRAILER", "PROPTRAILER", "RAKETRAILER",
	"TANKER", "TANKER2", "TR2", "TR3", "TR4",
	"TRAILERLOGS", "TRAILERS", "TRAILERS2", "TRAILERS3", "TRAILERSMALL",
	"TRFLAT", "TVTRAILER",


		//Hash load(const char* name)
	};
}
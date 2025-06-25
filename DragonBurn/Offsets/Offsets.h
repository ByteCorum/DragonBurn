#pragma once
#include <Windows.h>
#include "../Core/MemoryMgr.h"
#include <json.hpp>
#include "../Helpers/WebApi.h"

using json = nlohmann::json;

class Offsets
{
public:
	Offsets();
	~Offsets();
	int UpdateOffsets();

	DWORD EntityList;
	DWORD Matrix;
	DWORD ViewAngle;
	DWORD LocalPlayerController;
	DWORD LocalPlayerPawn;
	DWORD GlobalVars;
	DWORD PlantedC4;
	DWORD InputSystem;
	DWORD Sensitivity;

	struct
	{
		DWORD Jump;
		DWORD Left;
		DWORD Right;
		DWORD Attack;
	}Buttons;

	struct
	{
		DWORD IsAlive;
		DWORD PlayerPawn;
		DWORD iszPlayerName;
	}Entity;

	struct
	{
		DWORD BulletServices;
		DWORD CameraServices;
		DWORD pClippingWeapon;

		DWORD isScoped;
		DWORD isDefusing;
		DWORD TotalHit;
		DWORD Pos;
		DWORD CurrentArmor;
		DWORD MaxHealth;
		DWORD CurrentHealth;
		DWORD GameSceneNode;
		DWORD BoneArray;
		DWORD angEyeAngles;
		DWORD vecLastClipCameraPos;
		DWORD iShotsFired;
		DWORD flFlashDuration;
		DWORD aimPunchAngle;
		DWORD aimPunchCache;
		DWORD iIDEntIndex;
		DWORD iTeamNum;
		DWORD iFovStart;
		DWORD fFlags;
		DWORD bSpottedByMask;
		DWORD AbsVelocity;
		DWORD m_bWaitForNoAttack;

	} Pawn;

	struct
	{
		DWORD RealTime;
		DWORD FrameCount;
		DWORD MaxClients;
		DWORD IntervalPerTick;
		DWORD CurrentTime;
		DWORD CurrentTime2;
		DWORD TickCount;
		DWORD IntervalPerTick2;
		DWORD CurrentNetchan;
		DWORD CurrentMap;
		DWORD CurrentMapName;
	} GlobalVar;

	struct
	{
		DWORD m_steamID;
		DWORD m_hPawn;
		DWORD m_pObserverServices;
		DWORD m_hObserverTarget;
		DWORD m_hController;
		DWORD PawnArmor;
		DWORD HasDefuser;
		DWORD HasHelmet;
	} PlayerController;

	struct
	{
		DWORD AttributeManager;
	} EconEntity;

	struct
	{
		DWORD WeaponDataPTR;
		DWORD szName;
		DWORD Clip1;
		DWORD MaxClip;
		DWORD Item;
		DWORD ItemDefinitionIndex;
	} WeaponBaseData;

	struct
	{
		DWORD m_bBeingDefused;
		DWORD m_flDefuseCountDown;
		DWORD m_nBombSite;
	} C4;

private:
	void SetOffsets(const std::string&, const std::string&, const std::string&);
	bool LoadOffsetsFromFile(const std::string& filename);
	bool SaveOffsetsToFile(const std::string& filename, const std::string& offsetsData, const std::string& buttonsData, const std::string& client_dllData);
	bool CompareOffsets(const std::string& filename, const std::string& offsetsData, const std::string& buttonsData, const std::string& client_dllData);
};

inline Offsets Offset;
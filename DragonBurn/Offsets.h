#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"
#include "OffsetsList/Client.hpp"
#include "OffsetsList/Offsets.hpp"

namespace dll = cs2_dumper::schemas::client_dll;
namespace offsets = cs2_dumper::offsets::client_dll;
namespace offsetsInput = cs2_dumper::offsets::inputsystem_dll;

namespace Offset
{
	inline long long CS2ver = 8972920; //nano "D:\SteamLibrary\steamapps\common\Counter-Strike Global Offensive\game\bin\built_from_cl.txt"

	//https://github.com/a2x/cs2-dumper/blob/main/output/offsets.hpp
	inline DWORD EntityList =				offsets::dwEntityList;
	inline DWORD Matrix =					offsets::dwViewMatrix;
	inline DWORD ViewAngle =				offsets::dwViewAngles;
	inline DWORD LocalPlayerController =	offsets::dwLocalPlayerController;
	inline DWORD LocalPlayerPawn =			offsets::dwLocalPlayerPawn;
	inline DWORD GlobalVars =				offsets::dwGlobalVars;
	inline DWORD PlantedC4 =				offsets::dwPlantedC4;
	inline DWORD InputSystem =				offsetsInput::dwInputSystem;
	inline DWORD Sensitivity =				offsets::dwSensitivity;

	//https://github.com/a2x/cs2-dumper/blob/main/output/client.dll.hpp
	struct
	{
		DWORD IsAlive =				dll::CCSPlayerController::m_bPawnIsAlive;
		DWORD PlayerPawn =			dll::CCSPlayerController::m_hPlayerPawn;
		DWORD iszPlayerName =		dll::CBasePlayerController::m_iszPlayerName;
	}Entity;

	struct
	{
		DWORD BulletServices =		dll::C_CSPlayerPawn::m_pBulletServices;
		DWORD CameraServices =		dll::C_BasePlayerPawn::m_pCameraServices;
		DWORD pClippingWeapon =		dll::C_CSPlayerPawnBase::m_pClippingWeapon;

		DWORD isScoped =			dll::C_CSPlayerPawn::m_bIsScoped;
		DWORD isDefusing =			dll::C_CSPlayerPawn::m_bIsDefusing;
		DWORD TotalHit =			dll::CCSPlayer_BulletServices::m_totalHitsOnServer;
		DWORD Pos =					dll::C_BasePlayerPawn::m_vOldOrigin;
		DWORD CurrentArmor =		dll::C_CSPlayerPawn::m_ArmorValue;
		DWORD MaxHealth =			dll::C_BaseEntity::m_iMaxHealth;
		DWORD CurrentHealth =		dll::C_BaseEntity::m_iHealth;
		DWORD GameSceneNode =		dll::C_BaseEntity::m_pGameSceneNode;
		DWORD BoneArray =			0x1F0;
		DWORD angEyeAngles =		dll::C_CSPlayerPawnBase::m_angEyeAngles;
		DWORD vecLastClipCameraPos= dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos;
		DWORD iShotsFired =			dll::C_CSPlayerPawn::m_iShotsFired;
		DWORD flFlashDuration =		dll::C_CSPlayerPawnBase::m_flFlashDuration;
		DWORD aimPunchAngle =		dll::C_CSPlayerPawn::m_aimPunchAngle;
		DWORD aimPunchCache =		dll::C_CSPlayerPawn::m_aimPunchCache;
		DWORD iIDEntIndex =			dll::C_CSPlayerPawnBase::m_iIDEntIndex;
		DWORD iTeamNum =			dll::C_BaseEntity::m_iTeamNum;
		DWORD iFovStart =			dll::CCSPlayerBase_CameraServices::m_iFOVStart;
		DWORD fFlags =				dll::C_BaseEntity::m_fFlags;
		DWORD bSpottedByMask =		dll::C_CSPlayerPawn::m_entitySpottedState + dll::EntitySpottedState_t::m_bSpottedByMask;
		DWORD AbsVelocity =			dll::C_BaseEntity::m_vecAbsVelocity;
		DWORD m_bWaitForNoAttack =	dll::C_CSPlayerPawn::m_bWaitForNoAttack;

	} Pawn;

	struct
	{
		DWORD RealTime =			0x00;
		DWORD FrameCount =			0x04;
		DWORD MaxClients =			0x10;
		DWORD IntervalPerTick =		0x14;
		DWORD CurrentTime =			0x2C;
		DWORD CurrentTime2 =		0x30;
		DWORD TickCount =			0x40;
		DWORD IntervalPerTick2 =	0x44;
		DWORD CurrentNetchan =		0x0048;
		DWORD CurrentMap =			0x0180;
		DWORD CurrentMapName =		0x0188;
	} GlobalVar;

	struct
	{
		DWORD m_hPawn =				dll::CBasePlayerController::m_hPawn;
		DWORD m_pObserverServices = dll::C_BasePlayerPawn::m_pObserverServices;
		DWORD m_hObserverTarget =	dll::CPlayer_ObserverServices::m_hObserverTarget;
		DWORD m_hController =		dll::C_BasePlayerPawn::m_hController;
		DWORD PawnArmor =			dll::CCSPlayerController::m_iPawnArmor;
		DWORD HasDefuser =			dll::CCSPlayerController::m_bPawnHasDefuser;
		DWORD HasHelmet =			dll::CCSPlayerController::m_bPawnHasHelmet;
	} PlayerController;

	struct
	{
		DWORD AttributeManager =	dll::C_EconEntity::m_AttributeManager;
	} EconEntity;

	struct
	{
		DWORD WeaponDataPTR =		dll::C_BaseEntity::m_nSubclassID + 0x08;
		DWORD szName =				dll::CCSWeaponBaseVData::m_szName;
		DWORD Clip1 =				dll::C_BasePlayerWeapon::m_iClip1;
		DWORD MaxClip =				dll::CBasePlayerWeaponVData::m_iMaxClip1;
		DWORD Item =				dll::C_AttributeContainer::m_Item;
		DWORD ItemDefinitionIndex = dll::C_EconItemView::m_iItemDefinitionIndex;
	} WeaponBaseData;

	struct
	{
		DWORD m_bBeingDefused =		dll::C_PlantedC4::m_bBeingDefused;
		DWORD m_flDefuseCountDown = dll::C_PlantedC4::m_flDefuseCountDown;
		DWORD m_nBombSite =			dll::C_PlantedC4::m_nBombSite;
	} C4;
}
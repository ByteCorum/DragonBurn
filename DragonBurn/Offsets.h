#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"

namespace Offset
{

	inline DWORD EntityList;
	inline DWORD Matrix;
	inline DWORD ViewAngle;
	inline DWORD LocalPlayerController;
	inline DWORD LocalPlayerPawn;
	inline DWORD GlobalVars;
	inline DWORD PlantedC4;
	inline DWORD InputSystem;
	inline DWORD Sensitivity;
	inline DWORD Pointer;

	struct
	{
		DWORD IsAlive = 0x7EC;
		DWORD PlayerPawn = 0x7E4;
		DWORD iszPlayerName = 0x638;
	}Entity;

	struct
	{
		DWORD BulletServices = 0x1470;
		DWORD CameraServices = 0x1138;
		DWORD pClippingWeapon = 0x12F8;

		DWORD isScoped = 0x2290;
		DWORD isDefusing = 0x2292;
		DWORD TotalHit = 0x40;
		DWORD Pos = 0x127C;
		DWORD CurrentArmor = 0x22C0;
		DWORD MaxHealth = 0x330;
		DWORD CurrentHealth = 0x334;
		DWORD GameSceneNode = 0x318;
		DWORD BoneArray = 0x160 + 0x80;
		DWORD angEyeAngles = 0x1390;
		DWORD vecLastClipCameraPos = 0x12DC;
		DWORD iShotsFired = 0x22A4;
		DWORD flFlashDuration = 0x1364;
		DWORD aimPunchAngle = 0x14D4;
		DWORD aimPunchCache = 0x14F8;
		DWORD iIDEntIndex = 0x13B0;
		DWORD iTeamNum = 0x3CB;
		DWORD iFovStart = 0x214;
		DWORD fFlags = 0x3D4;
		DWORD bSpottedByMask = 0x2278 + 0xC;
		DWORD AbsVelocity = 0x3D8;
	} Pawn;

	struct
	{
		DWORD RealTime = 0x00;
		DWORD FrameCount = 0x04;
		DWORD MaxClients = 0x10;
		DWORD IntervalPerTick = 0x14;
		DWORD CurrentTime = 0x2C;
		DWORD CurrentTime2 = 0x30;
		DWORD TickCount = 0x40;
		DWORD IntervalPerTick2 = 0x44;
		DWORD CurrentNetchan = 0x0048;
		DWORD CurrentMap = 0x0180;
		DWORD CurrentMapName = 0x0188;
	} GlobalVar;

	struct
	{
		DWORD m_hPawn = 0x604;
		DWORD m_pObserverServices = 0x1118;
		DWORD m_hObserverTarget = 0x44;
		DWORD m_hController = 0x1294;
		DWORD PawnArmor = 0x7F4;
		DWORD HasDefuser = 0x7F8;
		DWORD HasHelmet = 0x7F9;
	} PlayerController;

	struct
	{
		DWORD AttributeManager = 0x1098;		
	} EconEntity;

	struct
	{
		DWORD WeaponDataPTR = 0x368;
		DWORD szName = 0xC20;
		DWORD Clip1 = 0x15C8;	
		DWORD MaxClip = 0x1FC;	
		DWORD Item = 0x50;	
		DWORD ItemDefinitionIndex = 0x1BA;
	} WeaponBaseData;

	struct
	{
		DWORD m_bBeingDefused = 0xF14;
		DWORD m_flDefuseCountDown = 0xF28;
		DWORD m_nBombSite = 0xEDC;
	} C4;

	bool UpdateOffsets();
}

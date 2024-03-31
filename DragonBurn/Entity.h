#pragma once
#include "Game.h"
#include "View.hpp"
#include "Bone.h"
#include "Utils/Globals.hpp"
#include <map>

struct C_UTL_VECTOR
{
	DWORD64 Count = 0;
	DWORD64 Data = 0;
};

class PlayerController
{
public:
	DWORD64 Address = 0;
	int Money = 0;
	int CashSpent = 0;
	int CashSpentTotal = 0;
	int TeamID = 0;
	int Health = 0;
	int AliveStatus = 0;
	DWORD Pawn = 0;
	std::string PlayerName;
public:
	bool GetTeamID();
	bool GetHealth();
	bool GetIsAlive();
	bool GetPlayerName();
	DWORD64 GetPlayerPawnAddress();
};

class PlayerPawn
{
public:
	enum class Flags
	{
		NONE,
		IN_AIR = 1 << 0,
		IN_CROUCH = 1 << 1
	};

	DWORD64 Address = 0;
	CBone BoneData;
	Vec2 ViewAngle;
	Vec3 Pos;
	Vec2 ScreenPos;
	Vec3 CameraPos;
	float Speed;
	std::string WeaponName;
	DWORD ShotsFired;
	Vec2 AimPunchAngle;
	C_UTL_VECTOR AimPunchCache;
	int Health;
	int Ammo;
	int MaxAmmo;
	int Armor;
	int TeamID;
	int Fov;
	DWORD64 bSpottedByMask;
	int fFlags;
	float FlashDuration;
	bool isDefusing;

public:
	bool GetPos();
	bool GetViewAngle();
	bool GetCameraPos();
	bool GetWeaponName();
	bool GetShotsFired();
	bool GetAimPunchAngle();
	bool GetHealth();
	bool GetTeamID();
	bool GetFov();
	bool GetSpotted();
	bool GetFFlags();
	bool GetAimPunchCache();
	bool GetAmmo();
	bool GetMaxAmmo();
	bool GetArmor();
	bool GetDefusing();
	bool GetFlashDuration();
	bool GetVelocity();

	constexpr bool HasFlag(const Flags Flag) const noexcept {
		return fFlags & (int)Flag;
	}
};

class Client
{
public:
	float Sensitivity;

public:
	bool GetSensitivity();
};

class CEntity
{
public:
	PlayerController Controller;
	PlayerPawn Pawn;
	Client Client;
public:
	bool UpdateController(const DWORD64& PlayerControllerAddress);
	bool UpdatePawn(const DWORD64& PlayerPawnAddress);
	bool UpdateClientData();
	bool IsAlive();
	bool IsInScreen();
	CBone GetBone() const;

public:
	static std::map<int, std::string> weaponNames;

	static std::string GetWeaponName(int weaponID);
};

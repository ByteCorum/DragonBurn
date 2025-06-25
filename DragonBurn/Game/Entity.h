#pragma once
#include "Game.h"
#include "View.h"
#include "Bone.h"
#include "../Core/Globals.h"
#include <map>

struct C_UTL_VECTOR
{
	DWORD64 Count = 0;
	DWORD64 Data = 0;
};

struct EntityBatchData {
	// Controller data
	int teamID;
	int health;
	int aliveStatus;
	char playerName[MAX_PATH];
	INT64 steamID;
	DWORD pawn;

	// Pawn data
	Vec2 viewAngle;
	Vec3 cameraPos;
	Vec3 pos;
	DWORD64 spottedMask;
	DWORD shotsFired;
	Vec2 aimPunchAngle;
	int pawnTeamID;
	int pawnHealth;
	int armor;
	float flashDuration;
	Vec3 velocity;
	int fov;
	int fFlags;
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
	INT64 SteamID = 0;
	DWORD Pawn = 0;
	std::string PlayerName;
	std::vector<std::string> spectators = {};
public:
	bool GetTeamID();
	bool GetHealth();
	bool GetIsAlive();
	bool GetPlayerName();
	bool GetPlayerSteamID();
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
	bool IsAlive() const;
	bool IsInScreen();
	CBone GetBone() const;

	bool UpdateControllerBatch(const DWORD64& PlayerControllerAddress);
	bool UpdatePawnBatch(const DWORD64& PlayerPawnAddress);
	static std::vector<CEntity> BatchUpdateEntities(const std::vector<DWORD64>& controllerAddresses);

public:
	static std::unordered_map<int, std::string> weaponNames;
	const std::string& GetWeaponName(int weaponID);
	//static std::string GetWeaponName(int weaponID);
};

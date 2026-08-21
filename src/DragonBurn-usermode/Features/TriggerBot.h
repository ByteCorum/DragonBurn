#pragma once
#include <chrono>

#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Core/Config.h"

namespace TriggerBot
{
    // Configuration
	inline int TriggerDelay = 10; // ms
	inline int ShotDuration = 400; // ms
	inline bool ScopeOnly = true;
	inline bool IgnoreFlash = false;
	inline bool StopedOnly = false;
	inline bool TTDtimeout = false;
	inline bool VisibleCheck = true;

    // Input configuration
    inline int HotKey = VK_XBUTTON2;

    // Timing variables
	inline std::chrono::time_point<std::chrono::system_clock> g_LastShotTime;
    inline std::chrono::time_point<std::chrono::system_clock> g_TargetFoundTime;
	inline bool g_HasValidTarget = false;

    // Main functions
	void Run(const CEntity& LocalEntity, const int& LocalPlayerControllerIndex);

    // Validation functions
    bool CanTrigger(const CEntity& LocalEntity, const CEntity& TargetEntity, const int& LocalPlayerControllerIndex);

    // Execution functions
	void ExecuteShot();

    // Utility functions
	std::string GetWeapon(const CEntity& LocalEntity);
	bool CheckWeapon(const std::string& WeaponName);
	bool CheckScopeWeapon(const std::string& WeaponName);
}
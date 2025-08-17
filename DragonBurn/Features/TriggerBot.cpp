#include "TriggerBot.h"
#include <chrono>
#include <random>
#include <thread>

void TriggerBot::Run(const CEntity& LocalEntity)
{
    if (MenuConfig::ShowMenu)
        return;

    if (LocalEntity.Controller.AliveStatus == 0)
        return;

    // Get the entity under the crosshair
    DWORD uHandle = 0;
    if (!memoryManager.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle))
    {
        g_HasValidTarget = false;
        g_CanShoot = false;
        return;
    }

    if (uHandle == -1)
    {
        g_HasValidTarget = false;
        g_CanShoot = false;
        return;
    }

    DWORD64 PawnAddress = CEntity::ResolveEntityHandle(uHandle);
    if (PawnAddress == 0)
    {
        g_HasValidTarget = false;
        g_CanShoot = false;
        return;
    }

    CEntity targetEntity;
    if (!targetEntity.UpdatePawn(PawnAddress))
    {
        g_HasValidTarget = false;
        g_CanShoot = false;
        return;
    }

    // Validate the targeted entity
    if (!CanTrigger(LocalEntity, targetEntity))
    {
        g_HasValidTarget = false;
        g_CanShoot = false;
        return;
    }

    // If we reach here, we have a valid target
    g_HasValidTarget = true;

    auto now = std::chrono::system_clock::now();

    // Handle shot duration cooldown
    if (g_CanShoot)
    {
        auto timeSinceShot = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_LastShotTime).count();

        if (timeSinceShot < ShotDuration)
        {
            return; // Still in shot cooldown
        }
        else
        {
            g_CanShoot = false; // Reset shot state
        }
    }

    // Process trigger logic if we have a valid target
    if (g_HasValidTarget)
    {
        // Handle trigger delay
        if (!g_CanShoot)
        {
            auto timeSinceFound = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_TargetFoundTime).count();

            if (timeSinceFound >= TriggerDelay)
            {
                g_CanShoot = true;
            }
        }

        // Execute shot if ready
        if (g_CanShoot && (GetAsyncKeyState(TriggerBot::HotKey) || LegitBotConfig::TriggerAlways))
        {
            ExecuteShot();
        }
    }
}

bool TriggerBot::CanTrigger(const CEntity& LocalEntity, const CEntity& TargetEntity)
{
    // Check if target is in a valid state
    if (TargetEntity.Pawn.Address == 0)
        return false;

    // Check team
    if (MenuConfig::TeamCheck && LocalEntity.Pawn.TeamID == TargetEntity.Pawn.TeamID)
        return false;

    // Check if weapon is ready
    bool waitForNoAttack = false;
    if (!memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.m_bWaitForNoAttack, waitForNoAttack))
        return false;

    if (waitForNoAttack)
        return false;

    // Check weapon type
    std::string currentWeapon = GetWeapon(LocalEntity);
    if (!CheckWeapon(currentWeapon))
        return false;

    //check is velocity == 0
    if(StopedOnly && LocalEntity.Pawn.Speed != 0)
        return false;

    // Check flash duration
    if (!IgnoreFlash && LocalEntity.Pawn.FlashDuration > 0.0f)
        return false;

    // Check scope requirement
    if (ScopeOnly && CheckScopeWeapon(currentWeapon))
    {
        bool isScoped = false;
        memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
        if (!isScoped)
            return false;
    }

    return true;
}

void TriggerBot::ExecuteShot()
{
    // Check if already shooting to avoid double-click
    if (GetAsyncKeyState(VK_LBUTTON) < 0)
        return;

    // Update timing
    g_LastShotTime = std::chrono::system_clock::now();

    // Execute shot with random timing
    std::random_device RandomDevice;
    std::mt19937 RandomNumber(RandomDevice());
    std::uniform_int_distribution<> Range(1, 5);
    auto rand = std::chrono::microseconds(Range(RandomNumber));

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    std::this_thread::sleep_for(std::chrono::microseconds(Range(RandomNumber)));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

std::string TriggerBot::GetWeapon(const CEntity& LocalEntity)
{
    // Single memory read to get the weapon pointer
    DWORD64 CurrentWeapon;
    if (!memoryManager.ReadMemory(LocalEntity.Pawn.Address + Offset.Pawn.pClippingWeapon, CurrentWeapon) || CurrentWeapon == 0)
        return "";

    // Calculate the final address for weapon index directly
    DWORD64 weaponIndexAddress = CurrentWeapon + Offset.EconEntity.AttributeManager +
        Offset.WeaponBaseData.Item + Offset.WeaponBaseData.ItemDefinitionIndex;

    // Single memory read to get weapon index
    short weaponIndex;
    if (!memoryManager.ReadMemory(weaponIndexAddress, weaponIndex) || weaponIndex == -1)
        return "";

    // Inline weapon name lookup
    static const std::string defaultWeapon = "";
    auto it = CEntity::weaponNames.find(weaponIndex);
    return (it != CEntity::weaponNames.end()) ? it->second : defaultWeapon;
}

bool TriggerBot::CheckScopeWeapon(const std::string& WeaponName)
{
    return (WeaponName == "awp" || WeaponName == "g3Sg1" || WeaponName == "ssg08" || WeaponName == "scar20");
}

bool TriggerBot::CheckWeapon(const std::string& WeaponName)
{
    return !(WeaponName == "smokegrenade" || WeaponName == "flashbang" || WeaponName == "hegrenade" ||
        WeaponName == "molotov" || WeaponName == "decoy" || WeaponName == "incgrenade" ||
        WeaponName == "t_knife" || WeaponName == "ct_knife" || WeaponName == "c4");
}
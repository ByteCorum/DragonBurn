#include "TriggerBot.h"

DWORD uHandle = 0;
DWORD64 ListEntry = 0;
DWORD64 PawnAddress = 0;
CEntity Entity;
bool AllowShoot = false;
bool WaitForNoAttack = false;

void TriggerBot::Run(const CEntity& LocalEntity)
{
    if (MenuConfig::ShowMenu)
        return;

    if (LocalEntity.Controller.AliveStatus == 0)
        return;

    if (!memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.m_bWaitForNoAttack, WaitForNoAttack))
        return;

    if (!memoryManager.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle))
        return;

    if (uHandle == -1)
        return;

    ListEntry = memoryManager.TraceAddress(gGame.GetEntityListAddress(), { 0x8 * (uHandle >> 9) + 0x10, 0x0 });
    if (ListEntry == 0)
        return;

    if (!memoryManager.ReadMemory<DWORD64>(ListEntry + 0x78 * (uHandle & 0x1FF), PawnAddress))
        return;

    if (!Entity.UpdatePawn(PawnAddress))
        return;

    std::string curWeapon = GetWeapon(LocalEntity);
    if (!CheckWeapon(curWeapon))
        return;

    if (!IgnoreFlash && LocalEntity.Pawn.FlashDuration > 0.f)
        return;

    if (ScopeOnly)
    {
        bool isScoped;
        memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
        
        if (!isScoped and CheckScopeWeapon(curWeapon))
            return;
    }

    if (MenuConfig::TeamCheck)
        AllowShoot = LocalEntity.Pawn.TeamID != Entity.Pawn.TeamID && Entity.Pawn.Health > 0;
    else
        AllowShoot = Entity.Pawn.Health > 0;


    if (!AllowShoot)
        return;


    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> difference = now - timepoint;
    if (!recorded && difference.count() >= ShotDuration)
    {
        startTime = std::chrono::system_clock::now();
        recorded = true;
    }
    std::chrono::duration<double, std::milli> difference1 = now - startTime;
    if (difference.count() >= ShotDuration && difference1.count() >= TriggerDelay)
    {
        const bool isAlreadyShooting = GetAsyncKeyState(VK_LBUTTON) < 0;
        if (!isAlreadyShooting)
        {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            timepoint = std::chrono::system_clock::now();
            recorded = false;
        }
    }
}

std::string TriggerBot::GetWeapon(const CEntity& LocalEntity)
{
    DWORD64 WeaponNameAddress = 0;
    char Buffer[256]{};

    WeaponNameAddress = memoryManager.TraceAddress(LocalEntity.Pawn.Address + Offset.Pawn.pClippingWeapon, { 0x10, 0x20, 0x0 });
    if (WeaponNameAddress == 0)
        return "";

    DWORD64 CurrentWeapon;
    short weaponIndex;
    memoryManager.ReadMemory(LocalEntity.Pawn.Address + Offset.Pawn.pClippingWeapon, CurrentWeapon);
    memoryManager.ReadMemory(CurrentWeapon + Offset.EconEntity.AttributeManager + Offset.WeaponBaseData.Item + Offset.WeaponBaseData.ItemDefinitionIndex, weaponIndex);

    if (weaponIndex == -1)
        return "";

    return CEntity::GetWeaponName(weaponIndex);
}

bool TriggerBot::CheckScopeWeapon(const std::string& WeaponName)
{
    if (WeaponName == "awp" || WeaponName == "g3Sg1" || WeaponName == "ssg08" || WeaponName == "scar20")
        return true;
    else
        return false;
}

bool TriggerBot::CheckWeapon(const std::string& WeaponName)
{
    if (WeaponName == "smokegrenade" || WeaponName == "flashbang" || WeaponName == "hegrenade" || WeaponName == "molotov" || WeaponName == "decoy" || WeaponName == "incgrenade" || WeaponName == "t_knife" || WeaponName == "ct_knife" || WeaponName == "c4")
        return false;
    else
        return true;
}

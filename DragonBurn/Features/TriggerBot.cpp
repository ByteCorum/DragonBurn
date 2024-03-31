#include "TriggerBot.h"

DWORD uHandle = 0;
DWORD64 ListEntry = 0;
DWORD64 PawnAddress = 0;
CEntity Entity;
bool AllowShoot = false;

void TriggerBot::ReleaseMouseButton()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(FakeShotDelay));
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void TriggerBot::Run(const CEntity& LocalEntity)
{
    DWORD uHandle;
    DWORD64 ListEntry;
    DWORD64 PawnAddress;

    if (!ProcessMgr.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset::Pawn.iIDEntIndex, uHandle) || uHandle == -1)
        return;

    ListEntry = ProcessMgr.TraceAddress(gGame.GetEntityListAddress(), { 0x8 * (uHandle >> 9) + 0x10, 0x0 });
    if (ListEntry == 0)
        return;

    if (!ProcessMgr.ReadMemory<DWORD64>(ListEntry + 0x78 * (uHandle & 0x1FF), PawnAddress))
        return;

    if (!Entity.UpdatePawn(PawnAddress))
        return;

    if (!IgnoreFlash && LocalEntity.Pawn.FlashDuration > 0.f)
        return;

    if (ScopeOnly)
    {
        bool isScoped;
        ProcessMgr.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset::Pawn.isScoped, isScoped);
        if (!isScoped)
            return;
    }

    bool AllowShoot;
    if (MenuConfig::TeamCheck)
        AllowShoot = LocalEntity.Pawn.TeamID != Entity.Pawn.TeamID && Entity.Pawn.Health > 0;
    else
        AllowShoot = Entity.Pawn.Health > 0;

    if (!AllowShoot)
        return;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> elapsed_milliseconds = now - start;
    if (elapsed_milliseconds.count() >= FakeShotDelay)
    {
        const bool isAlreadyShooting = GetAsyncKeyState(VK_LBUTTON) < 0;
        if (!isAlreadyShooting)
        {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            start = std::chrono::system_clock::now();
        }
    }
}

void TriggerBot::TargetCheck(const CEntity& LocalEntity) noexcept {
    if (!ProcessMgr.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset::Pawn.iIDEntIndex, uHandle) || uHandle == -1) {
        CrosshairsCFG::isAim = false;
        return;
    }

    const unsigned long long ENTITY_OFFSET = 0x78;
    const unsigned long long ENTITY_INDEX_MASK = 0x1FF;

    DWORD64 ListEntry = ProcessMgr.TraceAddress(gGame.GetEntityListAddress(), { 0x8 * (uHandle >> 9) + 0x10, 0x0 });
    if (ListEntry == 0) {
        CrosshairsCFG::isAim = false;
        return;
    }

    DWORD64 PawnAddress;
    const DWORD64 ENTITY_ADDRESS_OFFSET = ENTITY_OFFSET * (uHandle & ENTITY_INDEX_MASK);
    if (!ProcessMgr.ReadMemory<DWORD64>(ListEntry + ENTITY_ADDRESS_OFFSET, PawnAddress)) {
        CrosshairsCFG::isAim = false;
        return;
    }

    if (!Entity.UpdatePawn(PawnAddress)) {
        CrosshairsCFG::isAim = false;
        return;
    }

    CrosshairsCFG::isAim = CrosshairsCFG::TeamCheck ? (LocalEntity.Pawn.TeamID != Entity.Pawn.TeamID) : true;
}

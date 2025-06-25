#pragma once
#include "..\Core\Config.h"
#include "..\Game\Entity.h"
#include <vector>
#include <string>
#include <iostream>

namespace SpecList
{
    void SpectatorWindowList(CEntity& LocalEntity)
    {
        if ((!MiscCFG::SpecList || LocalEntity.Controller.TeamID == 0) && !(MiscCFG::SpecList && MenuConfig::ShowMenu))
                return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        static float fontHeight = ImGui::GetFontSize();
        float requiredHeight = LocalEntity.Controller.spectators.size() * (fontHeight + 5) + 20;
        ImGui::SetNextWindowPos(MenuConfig::SpecWinPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ 150.0f, requiredHeight }, ImGuiCond_Always);
        ImGui::GetStyle().WindowRounding = 8.0f;

        ImGui::Begin("Spectators", NULL, flags);

        if (MenuConfig::SpecWinChengePos) 
        {
            ImGui::SetWindowPos("Spectators", MenuConfig::SpecWinPos);
            MenuConfig::SpecWinChengePos = false;
        }

        for (const auto& spectator : LocalEntity.Controller.spectators)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
            ImGui::TextColored(ImColor(100, 108, 177, 220), spectator.c_str());
        }


        MenuConfig::SpecWinPos = ImGui::GetWindowPos();
        ImGui::End();
        LocalEntity.Controller.spectators.clear();
    }

    uintptr_t getAddressBase(uintptr_t entityList, uintptr_t playerPawn)
    {
        uintptr_t listEntrySecond;
        memoryManager.ReadMemory<uintptr_t>(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16, listEntrySecond);

        uintptr_t isPawn;
        memoryManager.ReadMemory<uintptr_t>(listEntrySecond + 120 * (playerPawn & 0x1FF), isPawn);

        return listEntrySecond == 0 ? 0 : isPawn;
    }

    void GetSpectatorList(CEntity Entity, CEntity& LocalEntity)
    {
        if (!MiscCFG::SpecList)
            return;

        uintptr_t LocalPlayer;
        memoryManager.ReadMemory<uintptr_t>(gGame.GetClientDLLAddress() + Offset.LocalPlayerController, LocalPlayer);
        uintptr_t localPlayerPawn;
        memoryManager.ReadMemory<uintptr_t>(LocalPlayer + Offset.PlayerController.m_hPawn, localPlayerPawn);

        uintptr_t CSlocalPlayerPawn;
        memoryManager.ReadMemory<uintptr_t>(gGame.GetEntityListEntry() + 120 * (localPlayerPawn & 0x1FF), CSlocalPlayerPawn);

        uint32_t spectatorPawn;
        memoryManager.ReadMemory<uint32_t>(Entity.Controller.Address + Offset.PlayerController.m_hPawn, spectatorPawn);

        uintptr_t entityList;
        memoryManager.ReadMemory<uintptr_t>(gGame.GetEntityListAddress(),entityList);

        uintptr_t pawn = getAddressBase(entityList, spectatorPawn);

        uintptr_t observed;
        memoryManager.ReadMemory<uintptr_t>(pawn + Offset.PlayerController.m_pObserverServices, observed);

        uint64_t observedTarget;
        memoryManager.ReadMemory<uintptr_t>(observed + Offset.PlayerController.m_hObserverTarget, observedTarget);

        uintptr_t spectatorTarget = getAddressBase(entityList, observedTarget);

        if (observed)
        {
            if (spectatorTarget == CSlocalPlayerPawn)
            {
                LocalEntity.Controller.spectators.push_back(Entity.Controller.PlayerName);
            }
        }
    }
}
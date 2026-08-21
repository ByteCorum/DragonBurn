#pragma once
#include "..\Game\Entity.h"
#include <unordered_set>

namespace SpecList
{
    struct SpecData
    {
        std::unordered_set<std::string> current_spectators;
        uintptr_t spectated_pawn;
        bool needs_refresh;
        uintptr_t prev_target_pawn = 0;
    };

    static SpecData g_spec_data;

    uintptr_t GetObserverTarget(uintptr_t entityPawnAddress)
    {
        if (!entityPawnAddress)
            return 0;

        uintptr_t observerServices = 0;
        if (!memoryManager.ReadMemory<uintptr_t>(entityPawnAddress + Offset.PlayerController.m_pObserverServices, observerServices) || !observerServices)
            return 0;

        uint32_t observedTargetHandle = 0;
        if (!memoryManager.ReadMemory<uint32_t>(observerServices + Offset.PlayerController.m_hObserverTarget, observedTargetHandle) || !observedTargetHandle)
            return 0;

        return CEntity::ResolveEntityHandle(observedTargetHandle);
    }

    void GetSpectatorList(const std::vector<CEntity>& allEntities, CEntity& LocalEntity)
    {
        auto prev_spectators = g_spec_data.current_spectators;

        g_spec_data.current_spectators.clear();
        g_spec_data.spectated_pawn = 0;
        g_spec_data.prev_target_pawn = 0;

        if (!LocalEntity.IsAlive() && LocalEntity.Pawn.Address != 0)
        {
            g_spec_data.prev_target_pawn = GetObserverTarget(LocalEntity.Pawn.Address);
            g_spec_data.spectated_pawn = g_spec_data.prev_target_pawn;
        }

        for (const auto& entity : allEntities)
        {
            if (!entity.Controller.Address || entity.Controller.PlayerName.empty())
                continue;

            if (entity.Controller.Address == LocalEntity.Controller.Address)
                continue;

            if (LocalEntity.IsAlive() && entity.IsAlive())
                continue;

            uintptr_t pawn_handle = 0;
            if (!memoryManager.ReadMemory<uintptr_t>(entity.Controller.Address + Offset.PlayerController.m_hPawn, pawn_handle))
                continue;

            uintptr_t pawn_addr = 0;
            if (pawn_handle != 0)
                pawn_addr = CEntity::ResolveEntityHandle(pawn_handle);

            if (!pawn_addr)
                pawn_addr = entity.Pawn.Address;

            if (!pawn_addr)
                continue;

            uintptr_t spec_target = GetObserverTarget(pawn_addr);
            if (!spec_target)
                continue;

            if (spec_target == LocalEntity.Pawn.Address || (g_spec_data.spectated_pawn != 0 && spec_target == g_spec_data.spectated_pawn))
                g_spec_data.current_spectators.insert(entity.Controller.PlayerName);
        }

        g_spec_data.needs_refresh = (g_spec_data.current_spectators != prev_spectators);
    }

    void SpectatorWindowList(CEntity& LocalEntity)
    {
        if (!MiscCFG::SpecList || (LocalEntity.Pawn.TeamID == 0 && !MenuConfig::ShowMenu))//&& g_spec_data.current_spectators.empty()
            return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

        static float fontHeight = ImGui::GetFontSize();
        float requiredHeight = 0.0f;
        
        requiredHeight = g_spec_data.current_spectators.size() * (fontHeight + 5) + 20;

        ImGui::SetNextWindowPos(MenuConfig::SpecWinPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ 150.0f, requiredHeight }, ImGuiCond_Always);
        ImGui::GetStyle().WindowRounding = 8.0f;

        std::string title = "Spectators";
        ImGui::Begin(title.c_str(), NULL, flags);

        if (MenuConfig::SpecWinChengePos)
        {
            ImGui::SetWindowPos(title.c_str(), MenuConfig::SpecWinPos);
            MenuConfig::SpecWinChengePos = false;
        }

        if (!g_spec_data.current_spectators.empty())
        {
            for (const auto& spectator : g_spec_data.current_spectators)
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                ImGui::Text(spectator.c_str());
            }
        }

        MenuConfig::SpecWinPos = ImGui::GetWindowPos();
        ImGui::End();
    }
}
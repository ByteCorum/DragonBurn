#pragma once
#include "..\Core\Config.h"
#include "..\Game\Entity.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

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

    uintptr_t GetObserverTarget(uintptr_t entityPawnAddress);
    void GetSpectatorList(const std::vector<CEntity>& allEntities, CEntity& LocalEntity);
    void SpectatorWindowList(CEntity& LocalEntity);
}
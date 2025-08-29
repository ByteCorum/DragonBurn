#include "SoundESP.h"
#define NOMINMAX
#include "../Core/AnimationSystem.hpp"
#include "../Core/Config.h"

namespace SoundSystem {

static inline bool is_entity_index_alive(int idx) {
    if (idx < 0 || idx >= 64) return false;
    DWORD64 controllerAddress = 0;
    if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (static_cast<DWORD64>(idx) + 1) * 0x78, controllerAddress))
        return false;
    if (controllerAddress == 0) return false;
    int aliveStatus = 0;
    if (!memoryManager.ReadMemory<int>(controllerAddress + Offset.Entity.IsAlive, aliveStatus))
        return false;
    return aliveStatus != 0;
}
void render_filled_3d_circle( const Vec3& origin, float radius, ImColor color ) {
	static constexpr float pi = 3.14159265358979323846f;
	
	static constexpr float Step = pi * 2.0f / 60;
	std::vector<ImVec2> points;
	for ( float lat = 0.f; lat <= pi * 2.0f; lat += Step ) {
		const auto& point3d = Vec3( sin( lat ), cos( lat ), 0.f ) * radius;
		Vec2 point2d;
		if ( gGame.View.WorldToScreen( origin + point3d, point2d ) )
			points.push_back( ImVec2( point2d.x, point2d.y ) );
	}
	
	if (!points.empty()) {
		ImGui::GetBackgroundDrawList( )->AddConvexPolyFilled( points.data( ), points.size( ), color );
	}
}

void render_3d_circle( const Vec3& origin, float radius, ImColor color ) {
	static constexpr float pi = 3.14159265358979323846f;
	static constexpr float Step = pi * 2.0f / 60;

	std::vector<ImVec2> points;
	for ( float lat = 0.f; lat <= pi * 2.0f; lat += Step ) {
		const auto& point3d = Vec3( sin( lat ), cos( lat ), 0.f ) * radius;
		Vec2 point2d;
		if ( gGame.View.WorldToScreen( origin + point3d, point2d ) )
			points.push_back( ImVec2( point2d.x, point2d.y ) );
	}

    if ( !points.empty( ) ) {
        points.push_back( points.front( ) );
        ImGui::GetBackgroundDrawList( )->AddPolyline( points.data( ), points.size( ), color, false, 1.0f );
    }
}

void sound_info::push_sound( const CEntity& entity, int idx, const CEntity& localEntity ) { /* use that for enemy */
    if (!MiscCFG::EnemySound)
        return;

    // Check if the entity is within the specified distance
    if (entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) > 1000.0f) {
        return;
    }

    // Strong, fresh validation using direct memory reads (avoid stale cached data)
    if (entity.Pawn.Address == 0 || entity.Controller.Address == 0) {
        if (!s_info.empty()) {
            s_info.erase(
                std::remove_if(s_info.begin(), s_info.end(), [&](const sound_info_t& ev) { return ev.entity_index == idx && ev.controller_address == entity.Controller.Address; }),
                s_info.end());
        }
        return;
    }

    int currentHealth = 0;
    int aliveStatus = 0;
    memoryManager.ReadMemory<int>(entity.Pawn.Address + Offset.Pawn.CurrentHealth, currentHealth);
    memoryManager.ReadMemory<int>(entity.Controller.Address + Offset.Entity.IsAlive, aliveStatus);
    if (currentHealth <= 0 || aliveStatus == 0) {
        if (!s_info.empty()) {
            s_info.erase(
                std::remove_if(s_info.begin(), s_info.end(), [&](const sound_info_t& ev) { return ev.entity_index == idx && ev.controller_address == entity.Controller.Address; }),
                s_info.end());
        }
        return;
    }

    // game::m_flEmitSoundTime
    float emit_sound_time = 0.0f;
    if (!memoryManager.ReadMemory<float>(entity.Pawn.Address + Offset.Pawn.m_flEmitSoundTime, emit_sound_time)) {
        // If target died between checks, ensure cleanup
        if (!is_entity_index_alive(idx)) {
            s_info.erase(
                std::remove_if(s_info.begin(), s_info.end(), [&](const sound_info_t& ev) { return ev.entity_index == idx; }),
                s_info.end());
        }
        return; // if we can't read, skip
    }

    const int slot = (idx >= 0 && idx < 64) ? idx : 0;
    static float main[64]{ 0.0f };
    if (main[slot] == 0.0f) main[slot] = emit_sound_time;

    // Reduce how often rings are spawned/refreshed for the same entity (cooldown)
    static double lastSpawn[64]{ 0.0 };
    const double nowTime = ImGui::GetTime();
    const double minSpawnInterval = 0.85; // seconds between updates per entity (slightly rarer)

    if (main[slot] != emit_sound_time) {
        // Only treat as valid movement sound if the target is actually moving (filter out ragdoll/idle updates)
        Vec3 absVel{ 0,0,0 };
        if (!memoryManager.ReadMemory<Vec3>(entity.Pawn.Address + Offset.Pawn.AbsVelocity, absVel)) {
            return;
        }
        const float moveSpeed2D = std::sqrt(absVel.x * absVel.x + absVel.y * absVel.y);
        if (moveSpeed2D < 15.0f) { // threshold to ignore near-static events
            main[slot] = emit_sound_time; // acknowledge but don't spawn ripples
            goto RENDER_RIPPLES;
        }
        main[slot] = emit_sound_time; // acknowledge the sound change immediately

        if ((nowTime - lastSpawn[slot]) < minSpawnInterval)
            goto RENDER_RIPPLES; // skip spawning new ring too soon, but still render existing

        // Per-entity single ring: update existing or add new
        bool updated = false;
        for (auto& ev : this->s_info) {
            if (ev.entity_index == slot && ev.controller_address == entity.Controller.Address) {
                ev.player_origin = entity.Pawn.Pos;
                ev.has_changed = true;
                ev.spawn_time = nowTime;
                updated = true;
                break;
            }
        }
        if (!updated) {
            sound_info_t ev{ };
            ev.player_origin = entity.Pawn.Pos;
            ev.has_changed = true;
            ev.spawn_time = nowTime;
            ev.entity_index = slot;
            ev.controller_address = entity.Controller.Address;
            ev.pawn_address = entity.Pawn.Address;
            ev.steam_id = entity.Controller.SteamID;
            this->s_info.push_back(ev);
        }
        lastSpawn[slot] = nowTime;
    }

RENDER_RIPPLES:
    // ripple rendering
    const int numRings = 1;
    for (std::size_t i = 0; i < this->s_info.size();) {
        // If the owning entity is dead now, drop the ripple immediately
        // Also drop if the slot got reused by a different controller
        const auto& evref = this->s_info[i];
        if (!is_entity_index_alive(evref.entity_index)) {
            this->s_info.erase(this->s_info.begin() + static_cast<long long>(i));
            continue;
        }
        DWORD64 currentController = 0;
        if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (static_cast<DWORD64>(evref.entity_index) + 1) * 0x78, currentController) ||
            currentController == 0 || currentController != evref.controller_address) {
            this->s_info.erase(this->s_info.begin() + static_cast<long long>(i));
            continue;
        }
        const double elapsed = ImGui::GetTime() - this->s_info[i].spawn_time;

        // Smooth expanding rings; stop at boundary
        const float waveSpeed = 340.0f;   // faster expansion
        const float maxRadius = 150.0f;   // visual boundary
        const float duration = max(0.1f, maxRadius / max(1.0f, waveSpeed));
        const float ringDelay = duration / static_cast<float>(numRings);

        bool anyRingDrawn = false;
        for (int r = 0; r < numRings; ++r) {
            const float t = static_cast<float>(elapsed) - r * ringDelay;
            if (t < 0.0f || t > duration)
                continue;
            const float progress = std::clamp(t / duration, 0.0f, 1.0f);
            // Start moderately sized; expand smoothly
            const float startRadius = min(18.0f, maxRadius * 0.2f);
            const float radius = startRadius + (maxRadius - startRadius) * progress;

            ImColor col = MiscCFG::EnemySoundColor;
            col.Value.w = col.Value.w * (1.0f - progress);
            render_3d_circle(this->s_info[i].player_origin, radius, col);
            anyRingDrawn = true;
        }

        if (!anyRingDrawn) {
            this->s_info.erase(this->s_info.begin() + static_cast<long long>(i));
        } else {
            ++i;
        }
    }
}

// Prune stored ripples for entities that are no longer alive/valid this frame
void sound_info::prune_by_alive_indices(const std::vector<int>& alive_indices) {
    if (s_info.empty()) return;

    // Fast lookup for alive indices
    std::vector<int> sorted_indices = alive_indices;
    std::sort(sorted_indices.begin(), sorted_indices.end());
    sorted_indices.erase(std::unique(sorted_indices.begin(), sorted_indices.end()), sorted_indices.end());

    // Remove entries whose entity_index is not in the alive list
    s_info.erase(
        std::remove_if(s_info.begin(), s_info.end(), [&](const sound_info_t& ev) {
            if (ev.entity_index < 0) return false; // keep local sounds (if any)
            return !std::binary_search(sorted_indices.begin(), sorted_indices.end(), ev.entity_index);
        }),
        s_info.end());
}
}

#pragma once
#include "../Game/Entity.h"
#include "../Core/Config.h"

namespace SoundESP {
    struct SoundEffect {
        Vec3 origin;
        double spawnTime;
        CEntity entity;
    };

    inline float MaxDistance = 2000.0f;
    inline float EffectSpeed = 200.0f;
    inline float MaxRadius = 60.0f;
    inline float MinMovementSpeed = 15.0f;
    inline double MinSpawnInterval = 0.2f;

    static std::vector<SoundEffect> soundEffects;
    static std::unordered_map<int, float> lastSoundTimes;
    static std::unordered_map<int, double> lastSpawnTimes;

    inline void RenderSound(const Vec3& origin, float radius, const ImColor& color) {
        static constexpr float PI = 3.14159265358979323846f;
        static constexpr float STEP = PI * 2.0f / 60;
        
        std::vector<ImVec2> points;
        points.reserve(60);
        
        for (float angle = 0.0f; angle <= PI * 2.0f; angle += STEP) {
            Vec3 worldPoint = origin + Vec3(sin(angle), cos(angle), 0.0f) * radius;
            Vec2 screenPoint;
            if (gGame.View.WorldToScreen(worldPoint, screenPoint)) {
                points.emplace_back(screenPoint.x, screenPoint.y);
            }
        }

        if (!points.empty()) {
            points.push_back(points.front());
            ImGui::GetBackgroundDrawList()->AddPolyline(points.data(), points.size(), color, false, 1.0f);
        }
    }

    inline void ProcessSound(const CEntity& entity, const CEntity& localEntity) {
        if (entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) > MaxDistance)
            return;

        float currentSoundTime = 0.0f;
        bool Jumped = !entity.Pawn.HasFlag(PlayerPawn::Flags::ON_GROUND);

        if (!memoryManager.ReadMemory<float>(entity.Pawn.Address + Offset.Pawn.m_flEmitSoundTime, currentSoundTime))
            return;
        if (lastSoundTimes[entity.Controller.Address] == 0.0f) {
            lastSoundTimes[entity.Controller.Address] = currentSoundTime;
            return;
        }
        if (lastSoundTimes[entity.Controller.Address] == currentSoundTime)
            return;

        if (entity.Pawn.Speed < MinMovementSpeed && !Jumped)
        {
            lastSoundTimes[entity.Controller.Address] = currentSoundTime;
            return;
        }

        lastSoundTimes[entity.Controller.Address] = currentSoundTime;

        double currentTime = ImGui::GetTime();
        if (currentTime - lastSpawnTimes[entity.Controller.Address] < MinSpawnInterval)
            return;

        lastSpawnTimes[entity.Controller.Address] = currentTime;

        bool updated = false;
        for (auto& soundEffect : soundEffects) {
            if (soundEffect.entity.Controller.Address == entity.Controller.Address) {
                soundEffect.origin = entity.Pawn.Pos;
                soundEffect.spawnTime = currentTime;
                soundEffect.entity = entity;
                updated = true;
                break;
            }
        }

        if (!updated) {
            soundEffects.push_back({
                entity.Pawn.Pos,
                currentTime,
                entity
            });
        }
    }

    inline void Render() {
        if (soundEffects.empty())
            return;

        double currentTime = ImGui::GetTime();
        float duration = MaxRadius / EffectSpeed;

        for (auto it = soundEffects.begin(); it != soundEffects.end();) {
            auto& soundEffect = *it;

            if (!soundEffect.entity.IsAlive()) {
                it = soundEffects.erase(it);
                continue;
            }

            float elapsed = static_cast<float>(currentTime - soundEffect.spawnTime);
            
            if (elapsed > duration) {
                it = soundEffects.erase(it);
                continue;
            }

            float progress = std::clamp(elapsed / duration, 0.0f, 1.0f);
            float startRadius = MaxRadius * 0.1f;
            float radius = startRadius + (MaxRadius - startRadius) * progress;
            
            ImColor color = ESPConfig::EnemySoundColor;
            color.Value.w *= (1.0f - progress);

            RenderSound(soundEffect.origin, radius, color);
            ++it;
        }
    }
}
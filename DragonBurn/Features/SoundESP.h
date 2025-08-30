#pragma once
#include "../Game/Entity.h"

namespace SoundESP {
    struct SoundEffect {
        Vec3 origin;
        double spawnTime;
        CEntity entity;
    };

    struct AnimationContext {
        float value = 0.0f;
        unsigned int id = 0;
        
        void SetValue(float newValue, bool clamp = true) {
            value = clamp ? std::clamp(newValue, 0.0f, 1.0f) : newValue;
        }
    };

    inline float MaxDistance = 1000.0f;
    inline float EffectSpeed = 340.0f;
    inline float MaxRadius = 150.0f;
    inline float MinMovementSpeed = 15.0f;
    inline double MinSpawnInterval = 0.85;

    void ProcessSound(const CEntity& entity, int entityIndex, const CEntity& localEntity);
    void Render();

    void RenderSound(const Vec3& origin, float radius, const ImColor& color);
    float GetAnimationValue(const std::string& key, float deltaTime);
}
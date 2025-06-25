#pragma once
#define _USE_MATH_DEFINES
#include <chrono>
#include <vector>
#include <math.h>

#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Core/Config.h"

#undef min()
#undef max()

namespace TriggerBot
{


    struct Vec3Trigger {
        float x, y, z;

        Vec3Trigger() : x(0), y(0), z(0) {}
        Vec3Trigger(float x, float y, float z) : x(x), y(y), z(z) {}

        Vec3Trigger operator+(const Vec3Trigger& other) const {
            return Vec3Trigger(x + other.x, y + other.y, z + other.z);
        }

        Vec3Trigger operator-(const Vec3Trigger& other) const {
            return Vec3Trigger(x - other.x, y - other.y, z - other.z);
        }

        Vec3Trigger operator*(float scalar) const {
            return Vec3Trigger(x * scalar, y * scalar, z * scalar);
        }

        float dot(const Vec3Trigger& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3Trigger normalize() const {
            float len = length();
            if (len == 0) return Vec3Trigger(0, 0, 0);
            return Vec3Trigger(x / len, y / len, z / len);
        }

        Vec3Trigger cross(const Vec3Trigger& other) const {
            return Vec3Trigger(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }
    };



    // Configuration
    inline int TriggerDelay = 30; // ms
    inline int ShotDuration = 80; // ms
    inline bool ScopeOnly = true;
    inline bool IgnoreFlash = false;
    inline bool WorkWithAimbot = false;
    inline bool VisibleCheck = true;

    // Input configuration
    inline int HotKey = VK_XBUTTON2;

    // Timing variables
    inline std::chrono::time_point<std::chrono::system_clock> g_LastShotTime;
    inline std::chrono::time_point<std::chrono::system_clock> g_TargetFoundTime;
    inline bool g_HasValidTarget = false;
    inline bool g_CanShoot = false;

    // Main functions
    void Run(const CEntity& LocalEntity);

    // Core detection function
    bool CheckForValidHitbox(const CEntity& LocalEntity, const CEntity& Entity, bool IsVisible);

    // Hitbox radius calculation based on bone positions
    float GetHitboxRadius(const CEntity& Entity, int hitboxIndex);

    bool raySphereIntersect(const Vec3Trigger& rayOrigin, const Vec3Trigger& rayDirection, const Vec3Trigger& sphereCenter, float sphereRadius);


    bool rayAABBIntersect(const Vec3Trigger& rayOrigin, const Vec3Trigger& rayDirection, const Vec3Trigger& boxCenter, const Vec3Trigger& boxSize);

    // Ray casting from eye position to hitbox radius
    bool CastRayToHitbox(const CEntity& LocalEntity, const CEntity& Entity, int hitboxIndex, const Vec2& hitboxScreenPos, float radius);

    // Validation functions
    bool IsValidTarget(const CEntity& LocalEntity, const CEntity& TargetEntity, bool Spotted);
    bool CanTrigger(const CEntity& LocalEntity);

    // Execution functions
    void ExecuteShot();

    // Utility functions
    std::string GetWeapon(const CEntity& LocalEntity);
    bool CheckScopeWeapon(const std::string& WeaponName);
    bool CheckWeapon(const std::string& WeaponName);
}
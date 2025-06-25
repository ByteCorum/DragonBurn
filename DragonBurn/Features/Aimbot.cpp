#include "Aimbot.h"

#undef max()
#undef min()

void AimControl::switchToggle()
{
    LegitBotConfig::AimAlways = !LegitBotConfig::AimAlways;
}

bool AimControl::CheckAutoMode(const std::string& WeaponName)
{
    if (WeaponName == "deagle" || WeaponName == "elite" || WeaponName == "fiveseven" || WeaponName == "glock" || WeaponName == "awp" || WeaponName == "xm1014" || WeaponName == "mag7" || WeaponName == "sawedoff" || WeaponName == "tec9" || WeaponName == "zeus" || WeaponName == "p2000" || WeaponName == "nova" || WeaponName == "p250" || WeaponName == "ssg08" || WeaponName == "usp" || WeaponName == "revolver")
        return false;
    else
        return true;
}

std::pair<float, float> AimControl::CalculateTargetOffset(const Vec2& ScreenPos, int ScreenCenterX, int ScreenCenterY)
{
    float TargetX = 0.0f;
    float TargetY = 0.0f;

    // Calculate X offset
    if (ScreenPos.x != ScreenCenterX) {
        TargetX = (ScreenPos.x > ScreenCenterX) ?
            -(ScreenCenterX - ScreenPos.x) :
            ScreenPos.x - ScreenCenterX;

        // Clamp to screen boundaries
        if (TargetX + ScreenCenterX > ScreenCenterX * 2 || TargetX + ScreenCenterX < 0) {
            TargetX = 0.0f;
        }
    }

    // Calculate Y offset
    if (ScreenPos.y != 0 && ScreenPos.y != ScreenCenterY) {
        TargetY = (ScreenPos.y > ScreenCenterY) ?
            -(ScreenCenterY - ScreenPos.y) :
            ScreenPos.y - ScreenCenterY;

        // Clamp to screen boundaries
        if (TargetY + ScreenCenterY > ScreenCenterY * 2 || TargetY + ScreenCenterY < 0) {
            TargetY = 0.0f;
        }
    }

    return { TargetX, TargetY };
}

// Humanize function to add natural movement patterns
std::pair<float, float> AimControl::Humanize(float TargetX, float TargetY) {

    // Clamp strength to valid range
    HumanizationStrength = std::clamp(HumanizationStrength, 0.0f, 1.0f);
    
    // If no humanization requested, return original targets
    if (HumanizationStrength <= 0.0f) {
        PrevTargetX = TargetX;
        PrevTargetY = TargetY;
        return { TargetX, TargetY };
    }
    
    // Random distributions for different types of jitter (increased ranges)
    std::uniform_real_distribution<float> jitterDist(-2.5f, 2.5f);
    std::uniform_real_distribution<float> microDist(-1.2f, 1.2f);
    std::uniform_real_distribution<float> smoothnessDist(0.4f, 0.85f);
    
    // Calculate movement distance for dynamic adjustments
    float MovementDistance = std::sqrt(TargetX * TargetX + TargetY * TargetY);
    
    // Add micro-movements (scaled by strength) - increased base multiplier
    float MicroJitterX = microDist(gen) * std::min(MovementDistance * 0.25f, 8.0f) * HumanizationStrength;
    float MicroJitterY = microDist(gen) * std::min(MovementDistance * 0.25f, 8.0f) * HumanizationStrength;
    
    // Add larger jitter for longer movements (scaled by strength) - increased multiplier
    float JitterScale = std::min(MovementDistance * 0.15f, 12.0f) * HumanizationStrength;
    float JitterX = jitterDist(gen) * JitterScale;
    float JitterY = jitterDist(gen) * JitterScale;
    
    // Create slightly curved path (scaled by strength) - increased curve factor
    float PerpX = -TargetY * 0.35f * jitterDist(gen) * HumanizationStrength;
    float PerpY = TargetX * 0.35f * jitterDist(gen) * HumanizationStrength;
    
    // Apply smoothing with strength-controlled factor (more aggressive smoothing variation)
    // At strength=0, no smoothing (immediate response)
    // At strength=1, full smoothing range with more noticeable lag
    float baseSmoothFactor = smoothnessDist(gen);
    float SmoothFactor = 1.0f - ((1.0f - baseSmoothFactor) * HumanizationStrength);
    float SmoothedX = TargetX * SmoothFactor + PrevTargetX * (1.0f - SmoothFactor);
    float SmoothedY = TargetY * SmoothFactor + PrevTargetY * (1.0f - SmoothFactor);
    
    // Add reaction time simulation - occasional delayed response
    std::uniform_real_distribution<float> reactionDist(0.0f, 1.0f);
    if (reactionDist(gen) < 0.15f * HumanizationStrength) { // 15% chance at full strength
        SmoothedX = PrevTargetX; // Use previous target (simulates delayed reaction)
        SmoothedY = PrevTargetY;
    }
    
    // Combine all humanization elements
    float HumanizedX = SmoothedX + MicroJitterX + JitterX + PerpX;
    float HumanizedY = SmoothedY + MicroJitterY + JitterY + PerpY;
    
    // Store current targets for next frame smoothing
    PrevTargetX = TargetX;
    PrevTargetY = TargetY;
    
    return { HumanizedX, HumanizedY };
}

void AimControl::AimBot(const CEntity& Local, Vec3 LocalPos,std::vector<Vec3>& AimPosList)
{
    // Early returns for invalid states
    if (MenuConfig::ShowMenu) return;

    std::string curWeapon = TriggerBot::GetWeapon(Local);
    if (!TriggerBot::CheckWeapon(curWeapon)) return;

    if (onlyAuto && !CheckAutoMode(curWeapon)) return;

    if (Local.Pawn.ShotsFired <= AimBullet - 1 && AimBullet != 0) {
        HasTarget = false;
        return;
    }

    // Scope check
    if (AimControl::ScopeOnly) {
        bool isScoped;
        memoryManager.ReadMemory<bool>(Local.Pawn.Address + Offset.Pawn.isScoped, isScoped);
        if (!isScoped && TriggerBot::CheckScopeWeapon(curWeapon)) {
            HasTarget = false;
            return;
        }
    }

    if (!IgnoreFlash && Local.Pawn.FlashDuration > 0.f) return;

    // Initialize variables
    const int ListSize = AimPosList.size();
    if (ListSize == 0) {
        HasTarget = false;
        return;
    }

    float BestNorm = MAXV;
    int BestTargetIndex = -1;
    Vec2 Angles{ 0, 0 };

    const int ScreenCenterX = Gui.Window.Size.x / 2;
    const int ScreenCenterY = Gui.Window.Size.y / 2;

    // Find best target
    for (int i = 0; i < ListSize; i++) {
        Vec3 OppPos = AimPosList[i] - LocalPos;
        const float Distance = sqrt(OppPos.x * OppPos.x + OppPos.y * OppPos.y);

        // Apply RCS if enabled
        if (LegitBotConfig::RCS) {
            RCS::UpdateAngles(Local, Angles);

            // X-axis rotation
            const float radX = Angles.x * RCS::RCSScale.x / 360.f * M_PI;
            const float sinX = sinf(radX);
            const float cosX = cosf(radX);

            const float z = OppPos.z * cosX + Distance * sinX;
            const float d = (Distance * cosX - OppPos.z * sinX) / Distance;

            // Y-axis rotation
            const float radY = -Angles.y * RCS::RCSScale.y / 360.f * M_PI;
            const float sinY = sinf(radY);
            const float cosY = cosf(radY);

            const float x = (OppPos.x * cosY - OppPos.y * sinY) * d;
            const float y = (OppPos.x * sinY + OppPos.y * cosY) * d;

            OppPos = Vec3{ x, y, z };
            AimPosList[i] = LocalPos + OppPos;
        }

        // Calculate angles
        const float Yaw = atan2f(OppPos.y, OppPos.x) * 57.295779513f - Local.Pawn.ViewAngle.y;
        const float Pitch = -atan(OppPos.z / Distance) * 57.295779513f - Local.Pawn.ViewAngle.x;
        const float Norm = sqrt(Yaw * Yaw + Pitch * Pitch);

        // Track best target
        if (Norm < BestNorm) {
            BestNorm = Norm;
            BestTargetIndex = i;
        }
    }

    // Check if best target is within FOV
    if (BestNorm >= AimFov || BestNorm <= AimFovMin || BestTargetIndex == -1) {
        HasTarget = false;
        return;
    }

    // Get screen position of best target
    Vec2 ScreenPos;
    if (!gGame.View.WorldToScreen(AimPosList[BestTargetIndex], ScreenPos)) {
        HasTarget = false;
        return;
    }

    HasTarget = true;

    // Calculate target offsets
    auto [TargetX, TargetY] = CalculateTargetOffset(ScreenPos, ScreenCenterX, ScreenCenterY);

    // Apply smoothing if enabled
    if (Smooth > 0.0f) {
        // Dynamic smoothing based on distance
        const float DistanceRatio = BestNorm / AimFov;
        const float SpeedFactor = 1.0f + (1.0f - DistanceRatio);
        TargetX /= (Smooth * SpeedFactor);
        TargetY /= (Smooth * SpeedFactor);
    }

    if (HumanizeVar)
    {
        // Humanize the movement
        auto [HumanizedX, HumanizedY] = Humanize(TargetX, TargetY);
        TargetX = HumanizedX;
        TargetY = HumanizedY;
    }
    

    // Apply mouse movement
    mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
}

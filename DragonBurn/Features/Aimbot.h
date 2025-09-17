#pragma once
#define _USE_MATH_DEFINES
#define MAXV 10000e9
#include <math.h>
#include <thread>
#include <chrono>
#include "..\Game\Game.h"
#include "..\Game\Entity.h"
#include "..\Core\Config.h"
#include <iostream>
#include "..\Game\View.h"
#include "..\Features/RCS.h"
#include "TriggerBot.h"
#include <random>

extern "C" {
#include "..\Helpers\Mouse.h"
#include "..\Game\Entity.h"
}


namespace AimControl
{
    inline int HotKey = VK_LBUTTON;
    inline int AimBullet = 1;
    inline bool ScopeOnly = true;
    inline bool IgnoreFlash = false;
    inline bool HumanizeVar = true;
    inline int HumanizationStrength = 5;
    inline float AimFov = 10;
    inline float AimFovMin = 0.4f;
    inline float Smooth = 5.0f;
    inline std::vector<int> HitboxList{ BONEINDEX::head };
    inline bool HasTarget = false;
    inline bool onlyAuto = false;

    static float PrevTargetX = 0.0f;
    static float PrevTargetY = 0.0f;
    static std::random_device rd;
    static std::mt19937 gen(rd());
   

    std::pair<float, float> Humanize(float TargetX, float TargetY);

    void AimBot(const CEntity& Local, Vec3 LocalPos,std::vector<Vec3>& AimPosList);
    void switchToggle();
    std::pair<float, float> CalculateTargetOffset(const Vec2& ScreenPos, int ScreenCenterX, int ScreenCenterY);
    bool CheckAutoMode(const std::string& WeaponName);
}

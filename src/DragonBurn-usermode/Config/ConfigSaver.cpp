#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ConfigSaver.h"
#include "../Resources/Language.h"
#include "../Core/Config.h"
#include "../Features/TriggerBot.h"
#include "../Features/Aimbot.h"
#include "../Features/RCS.h"
#include "../Helpers/KeyManager.h"
#include <json.hpp>

using json = nlohmann::json;

namespace MyConfigSaver 
{
    void SaveConfig(const std::string& filename, const std::string& author) {
        std::ofstream configFile(MenuConfig::path + '\\' + filename);
        if (!configFile.is_open()) {
            return;
        }

        json ConfigData;

        ConfigData["0"]["Name"] = MenuConfig::name;
        ConfigData["0"]["Version"] = MenuConfig::version;
        ConfigData["0"]["Author"] = author;

        ConfigData["ESP"]["Hotkey"] =               ESPConfig::HotKey;
        ConfigData["ESP"]["Enable"]=                ESPConfig::ESPenabled;
        ConfigData["ESP"]["BoneESP"]=               ESPConfig::ShowBoneESP;
        ConfigData["ESP"]["BoxESP"]=                ESPConfig::ShowBoxESP;
        ConfigData["ESP"]["BoxType"]=               ESPConfig::BoxType;
        ConfigData["ESP"]["SnapLine"]=              ESPConfig::ShowLineToEnemy;
        ConfigData["ESP"]["LinePos"]=               ESPConfig::LinePos;
        ConfigData["ESP"]["HealthBar"]=             ESPConfig::ShowHealthBar;
        ConfigData["ESP"]["AmmoBar"]=               ESPConfig::AmmoBar;
        ConfigData["ESP"]["WeaponESP"]=             ESPConfig::ShowWeaponESP;
        ConfigData["ESP"]["EyeRay"]=                ESPConfig::ShowEyeRay;
        ConfigData["ESP"]["PlayerName"]=            ESPConfig::ShowPlayerName;
        ConfigData["ESP"]["DistanceESP"]=           ESPConfig::ShowDistance;
        ConfigData["ESP"]["HealthNum"]=             ESPConfig::ShowHealthNum;
        ConfigData["ESP"]["HeadBox"]=               ESPConfig::ShowHeadBox;
        ConfigData["ESP"]["Preview"]=               ESPConfig::ShowPreview;
        ConfigData["ESP"]["VisCheck"]=              ESPConfig::VisibleCheck;
        ConfigData["ESP"]["FilledBox"]=             ESPConfig::FilledBox;
        ConfigData["ESP"]["FilledVisBox"]=          ESPConfig::FilledVisBox;
        ConfigData["ESP"]["MultiColor"]=            ESPConfig::MultiColor;
        ConfigData["ESP"]["OutLine"]=               ESPConfig::OutLine;
        ConfigData["ESP"]["BoxRounding"]=           ESPConfig::BoxRounding;
        ConfigData["ESP"]["ShowScoped"]=            ESPConfig::ShowIsScoped;
        ConfigData["ESP"]["ShowBlind"]=             ESPConfig::ShowIsBlind;
        ConfigData["ESP"]["DisableFlashed"] =       ESPConfig::FlashCheck;
        ConfigData["ESP"]["ArmorBar"]=              ESPConfig::ArmorBar;
        ConfigData["ESP"]["ArmorNum"]=              ESPConfig::ShowArmorNum;

        ConfigData["ESP"]["BoneColor"]["r"]=        ESPConfig::BoneColor.Value.x;
        ConfigData["ESP"]["BoneColor"]["g"]=        ESPConfig::BoneColor.Value.y;
        ConfigData["ESP"]["BoneColor"]["b"]=        ESPConfig::BoneColor.Value.z;
        ConfigData["ESP"]["BoneColor"]["a"]=        ESPConfig::BoneColor.Value.w;

        ConfigData["ESP"]["BoxColor"]["r"]=         ESPConfig::BoxColor.Value.x;
        ConfigData["ESP"]["BoxColor"]["g"]=         ESPConfig::BoxColor.Value.y;
        ConfigData["ESP"]["BoxColor"]["b"]=         ESPConfig::BoxColor.Value.z;
        ConfigData["ESP"]["BoxColor"]["a"]=         ESPConfig::BoxColor.Value.w;

        ConfigData["ESP"]["SnapLineColor"]["r"]=    ESPConfig::LineToEnemyColor.Value.x;
        ConfigData["ESP"]["SnapLineColor"]["g"]=    ESPConfig::LineToEnemyColor.Value.y;
        ConfigData["ESP"]["SnapLineColor"]["b"]=    ESPConfig::LineToEnemyColor.Value.z;
        ConfigData["ESP"]["SnapLineColor"]["a"]=    ESPConfig::LineToEnemyColor.Value.w;

        ConfigData["ESP"]["EyeRayColor"]["r"]=      ESPConfig::EyeRayColor.Value.x;
        ConfigData["ESP"]["EyeRayColor"]["g"]=      ESPConfig::EyeRayColor.Value.y;
        ConfigData["ESP"]["EyeRayColor"]["b"]=      ESPConfig::EyeRayColor.Value.z;
        ConfigData["ESP"]["EyeRayColor"]["a"]=      ESPConfig::EyeRayColor.Value.w;

        ConfigData["ESP"]["HeadBoxColor"]["r"]=     ESPConfig::HeadBoxColor.Value.x;
        ConfigData["ESP"]["HeadBoxColor"]["g"]=     ESPConfig::HeadBoxColor.Value.y;
        ConfigData["ESP"]["HeadBoxColor"]["b"]=     ESPConfig::HeadBoxColor.Value.z;
        ConfigData["ESP"]["HeadBoxColor"]["a"]=     ESPConfig::HeadBoxColor.Value.w;
        
        ConfigData["ESP"]["VisibleColor"]["r"]=     ESPConfig::VisibleColor.Value.x;
        ConfigData["ESP"]["VisibleColor"]["g"]=     ESPConfig::VisibleColor.Value.y;
        ConfigData["ESP"]["VisibleColor"]["b"]=     ESPConfig::VisibleColor.Value.z;
        ConfigData["ESP"]["VisibleColor"]["a"]=     ESPConfig::VisibleColor.Value.w;

        ConfigData["ESP"]["FilledColor"]["r"]=      ESPConfig::FilledColor.Value.x;
        ConfigData["ESP"]["FilledColor"]["g"]=      ESPConfig::FilledColor.Value.y;
        ConfigData["ESP"]["FilledColor"]["b"]=      ESPConfig::FilledColor.Value.z;
        ConfigData["ESP"]["FilledColor"]["a"]=      ESPConfig::FilledColor.Value.w;

        ConfigData["ESP"]["FilledColor2"]["r"]=     ESPConfig::FilledColor2.Value.x;
        ConfigData["ESP"]["FilledColor2"]["g"]=     ESPConfig::FilledColor2.Value.y;
        ConfigData["ESP"]["FilledColor2"]["b"]=     ESPConfig::FilledColor2.Value.z;
        ConfigData["ESP"]["FilledColor2"]["a"]=     ESPConfig::FilledColor2.Value.w;

        ConfigData["ESP"]["FilledVisColor"]["r"]=   ESPConfig::BoxFilledVisColor.Value.x;
        ConfigData["ESP"]["FilledVisColor"]["g"]=   ESPConfig::BoxFilledVisColor.Value.y;
        ConfigData["ESP"]["FilledVisColor"]["b"]=   ESPConfig::BoxFilledVisColor.Value.z;
        ConfigData["ESP"]["FilledVisColor"]["a"]=   ESPConfig::BoxFilledVisColor.Value.w;

        ConfigData["ESP"]["OutOfFOVArrow"]=         ESPConfig::ShowOutOfFOVArrow;
        ConfigData["ESP"]["OutOfFOVRadius"]=        ESPConfig::OutOfFOVRadiusFactor;
        ConfigData["ESP"]["OutOfFOVColor"]["r"]=    ESPConfig::OutOfFOVArrowColor.Value.x;
        ConfigData["ESP"]["OutOfFOVColor"]["g"]=    ESPConfig::OutOfFOVArrowColor.Value.y;
        ConfigData["ESP"]["OutOfFOVColor"]["b"]=    ESPConfig::OutOfFOVArrowColor.Value.z;
        ConfigData["ESP"]["OutOfFOVColor"]["a"]=    ESPConfig::OutOfFOVArrowColor.Value.w;

        ConfigData["ESP"]["EnemySound"] = ESPConfig::EnemySound;
        ConfigData["ESP"]["EnemySoundColor"]["r"] = ESPConfig::EnemySoundColor.Value.x;
        ConfigData["ESP"]["EnemySoundColor"]["g"] = ESPConfig::EnemySoundColor.Value.y;
        ConfigData["ESP"]["EnemySoundColor"]["b"] = ESPConfig::EnemySoundColor.Value.z;
        ConfigData["ESP"]["EnemySoundColor"]["a"] = ESPConfig::EnemySoundColor.Value.w;


        //ConfigData["Crosshairs"]["Enable"]=        CrosshairsCFG::ShowCrossHair;
        //ConfigData["Crosshairs"]["Size"]=          CrosshairsCFG::CrossHairSize;
        //ConfigData["Crosshairs"]["Dot"]=           CrosshairsCFG::drawDot;
        //ConfigData["Crosshairs"]["Crossline"]=     CrosshairsCFG::drawCrossline;
        //ConfigData["Crosshairs"]["tStyle"]=        CrosshairsCFG::tStyle;
        //ConfigData["Crosshairs"]["Circle"]=        CrosshairsCFG::drawCircle;
        //ConfigData["Crosshairs"]["Outline"]=       CrosshairsCFG::drawOutLine;
        //ConfigData["Crosshairs"]["DynamicGap"]=    CrosshairsCFG::DynamicGap;
        //ConfigData["Crosshairs"]["TeamCheck"]=     CrosshairsCFG::TeamCheck;
        //ConfigData["Crosshairs"]["Preset"]=        CrosshairsCFG::crosshairPreset;
        //ConfigData["Crosshairs"]["Gap"]=           CrosshairsCFG::Gap;
        //ConfigData["Crosshairs"]["H_Length"]=      CrosshairsCFG::HorizontalLength;
        //ConfigData["Crosshairs"]["V_Length"]=      CrosshairsCFG::VerticalLength;
        //ConfigData["Crosshairs"]["Thickness"]=     CrosshairsCFG::Thickness;
        //ConfigData["Crosshairs"]["DotSize"]=       CrosshairsCFG::DotSize;
        //ConfigData["Crosshairs"]["CircleRadius"]=  CrosshairsCFG::CircleRadius;
        //ConfigData["Crosshairs"]["TargetCheck"]=   MenuConfig::TargetingCrosshairs;

        //ConfigData["Crosshairs"]["CrosshairsColor"]["r"]=   CrosshairsCFG::CrossHairColor.Value.x;
        //ConfigData["Crosshairs"]["CrosshairsColor"]["g"]=   CrosshairsCFG::CrossHairColor.Value.y;
        //ConfigData["Crosshairs"]["CrosshairsColor"]["b"]=   CrosshairsCFG::CrossHairColor.Value.z;
        //ConfigData["Crosshairs"]["CrosshairsColor"]["a"]=   CrosshairsCFG::CrossHairColor.Value.w;

        //ConfigData["Crosshairs"]["TargetedColor"]["r"]=     CrosshairsCFG::TargetedColor.Value.x;
        //ConfigData["Crosshairs"]["TargetedColor"]["g"]=     CrosshairsCFG::TargetedColor.Value.y;
        //ConfigData["Crosshairs"]["TargetedColor"]["b"]=     CrosshairsCFG::TargetedColor.Value.z;
        //ConfigData["Crosshairs"]["TargetedColor"]["a"]=     CrosshairsCFG::TargetedColor.Value.w;



        ConfigData["Radar"]["Enable"] =            RadarCFG::ShowRadar;
        ConfigData["Radar"]["Type"]=               RadarCFG::RadarType;
        ConfigData["Radar"]["Range"]=              RadarCFG::RadarRange;
        ConfigData["Radar"]["Proportion"]=         RadarCFG::Proportion;
        ConfigData["Radar"]["PointProportion"]=    RadarCFG::RadarPointSizeProportion;
        ConfigData["Radar"]["Alpha"]=              RadarCFG::RadarBgAlpha;
        ConfigData["Radar"]["Custom"]=             RadarCFG::customRadar;
        ConfigData["Radar"]["Crossline"]=          RadarCFG::ShowRadarCrossLine;

        ConfigData["Radar"]["CrosslineColor"]["r"]=         RadarCFG::RadarCrossLineColor.Value.x;
        ConfigData["Radar"]["CrosslineColor"]["g"]=         RadarCFG::RadarCrossLineColor.Value.y;
        ConfigData["Radar"]["CrosslineColor"]["b"]=         RadarCFG::RadarCrossLineColor.Value.z;
        ConfigData["Radar"]["CrosslineColor"]["a"]=         RadarCFG::RadarCrossLineColor.Value.w;



        ConfigData["Aimbot"]["Enable"]=           LegitBotConfig::AimBot;
        ConfigData["Aimbot"]["ToggleMode"]=       LegitBotConfig::AimToggleMode;
        ConfigData["Aimbot"]["Hotkey"]=           AimControl::HotKey;
        ConfigData["Aimbot"]["AimBullet"]=        AimControl::AimBullet;
        ConfigData["Aimbot"]["Fov"]=              AimControl::AimFov;
        ConfigData["Aimbot"]["HumanizationStrength"]=              AimControl::HumanizationStrength;
        ConfigData["Aimbot"]["FovMin"]=           AimControl::AimFovMin;
        ConfigData["Aimbot"]["FovCircle"]=        ESPConfig::DrawFov;

        ConfigData["Aimbot"]["Smooth"]=           AimControl::Smooth;
        ConfigData["Aimbot"]["Hitboxes"]=         AimControl::HitboxList;
        ConfigData["Aimbot"]["VisibleCheck"]=     LegitBotConfig::VisibleCheck;
        ConfigData["Aimbot"]["IgnoreFlash"]=      AimControl::IgnoreFlash;
        ConfigData["Aimbot"]["HumanizeVar"]=      AimControl::HumanizeVar;
        ConfigData["Aimbot"]["ScopeOnly"]=        AimControl::ScopeOnly;
        ConfigData["Aimbot"]["OnlyAuto"] =        AimControl::onlyAuto;

        ConfigData["Aimbot"]["CircleColor"]["r"]= LegitBotConfig::FovCircleColor.Value.x;
        ConfigData["Aimbot"]["CircleColor"]["g"]= LegitBotConfig::FovCircleColor.Value.y;
        ConfigData["Aimbot"]["CircleColor"]["b"]= LegitBotConfig::FovCircleColor.Value.z;
        ConfigData["Aimbot"]["CircleColor"]["a"]= LegitBotConfig::FovCircleColor.Value.w;



        ConfigData["RCS"]["Enable"]=           LegitBotConfig::RCS;
        ConfigData["RCS"]["RCSBullet"]=        RCS::RCSBullet;
        ConfigData["RCS"]["Yaw"]=              RCS::RCSScale.x;
        ConfigData["RCS"]["Pitch"]=            RCS::RCSScale.y;



        ConfigData["Triggerbot"]["Enable"]=       LegitBotConfig::TriggerBot;
        ConfigData["Triggerbot"]["Hotkey"]=       TriggerBot::HotKey;
        ConfigData["Triggerbot"]["Delay"]=        TriggerBot::TriggerDelay;
        ConfigData["Triggerbot"]["FakeShot"]=     TriggerBot::ShotDuration;
        ConfigData["Triggerbot"]["ScopeOnly"]=    TriggerBot::ScopeOnly;
        ConfigData["Triggerbot"]["IgnoreFlash"]=  TriggerBot::IgnoreFlash;
        ConfigData["Triggerbot"]["StopedOnly"] = TriggerBot::StopedOnly;
        ConfigData["Triggerbot"]["TTDtimeout"] = TriggerBot::TTDtimeout;
        ConfigData["Triggerbot"]["AutoMode"]=     LegitBotConfig::TriggerAlways;



        ConfigData["Misc"]["HeadShootLine"] = MiscCFG::ShowHeadShootLine;
        ConfigData["Misc"]["HeadShootLineColor"]["r"] = MiscCFG::HeadShootLineColor.Value.x;
        ConfigData["Misc"]["HeadShootLineColor"]["g"] = MiscCFG::HeadShootLineColor.Value.y;
        ConfigData["Misc"]["HeadShootLineColor"]["b"] = MiscCFG::HeadShootLineColor.Value.z;
        ConfigData["Misc"]["HeadShootLineColor"]["a"] = MiscCFG::HeadShootLineColor.Value.w;

        ConfigData["Misc"]["WorkInSpec"]=       MenuConfig::WorkInSpec;
        ConfigData["Misc"]["Watermark"]=        MiscCFG::WaterMark;
        ConfigData["Misc"]["HitSounds"]=        MiscCFG::HitSound;
        ConfigData["Misc"]["HitMarker"]=        MiscCFG::HitMarker;

        ConfigData["Misc"]["BombTimer"]=        MiscCFG::bmbTimer;
        ConfigData["Misc"]["TimerColor"]["r"]=  MiscCFG::BombTimerCol.Value.x;
        ConfigData["Misc"]["TimerColor"]["g"]=  MiscCFG::BombTimerCol.Value.y;
        ConfigData["Misc"]["TimerColor"]["b"]=  MiscCFG::BombTimerCol.Value.z;
        ConfigData["Misc"]["TimerColor"]["a"]=  MiscCFG::BombTimerCol.Value.w;

        ConfigData["Misc"]["Bhop"]=             MiscCFG::BunnyHop;
        ConfigData["Misc"]["FastStop"] =        MiscCFG::FastStop;
        ConfigData["Misc"]["SpecList"]=         MiscCFG::SpecList;

        ConfigData["Misc"]["SniperCrosshair"] = MiscCFG::SniperCrosshair;
        ConfigData["Misc"]["SniperCrosshairColor"]["r"] = MiscCFG::SniperCrosshairColor.Value.x;
        ConfigData["Misc"]["SniperCrosshairColor"]["g"] = MiscCFG::SniperCrosshairColor.Value.y;
        ConfigData["Misc"]["SniperCrosshairColor"]["b"] = MiscCFG::SniperCrosshairColor.Value.z;
        ConfigData["Misc"]["SniperCrosshairColor"]["a"] = MiscCFG::SniperCrosshairColor.Value.w;

        ConfigData["Misc"]["AutoAccept"] = MiscCFG::AutoAccept;

        ConfigData["Misc"]["AutoKnife"] = MiscCFG::AutoKnife;
        ConfigData["Misc"]["AutoKnifeDistance"] = MiscCFG::AutoKnifeDistance;
        ConfigData["Misc"]["AutoZeus"] = MiscCFG::AutoZeus;
        ConfigData["Misc"]["AutoZeusDistance"] = MiscCFG::AutoZeusDistance;
        ConfigData["Misc"]["AntiAFKKick"] = MiscCFG::AntiAFKKick;

        ConfigData["Misc"]["TeamCheck"]=        MenuConfig::TeamCheck;
        ConfigData["Misc"]["AntiRecord"]=       MenuConfig::BypassOBS;
        ConfigData["Misc"]["MenuKey"] =         MenuConfig::HotKey;

        ConfigData["MenuConfig"]["MarkWinPos"]["x"] = MenuConfig::MarkWinPos.x;
        ConfigData["MenuConfig"]["MarkWinPos"]["y"] = MenuConfig::MarkWinPos.y;

        ConfigData["MenuConfig"]["BombWinPos"]["x"] = MenuConfig::BombWinPos.x;
        ConfigData["MenuConfig"]["BombWinPos"]["y"] = MenuConfig::BombWinPos.y;

        ConfigData["MenuConfig"]["RadarWinPos"]["x"] = MenuConfig::RadarWinPos.x;
        ConfigData["MenuConfig"]["RadarWinPos"]["y"] = MenuConfig::RadarWinPos.y;

        ConfigData["MenuConfig"]["SpecWinPos"]["x"] = MenuConfig::SpecWinPos.x;
        ConfigData["MenuConfig"]["SpecWinPos"]["y"] = MenuConfig::SpecWinPos.y;

        configFile << ConfigData.dump(4);
        configFile.close();
    }

    // Function to load the configuration from a file
    void LoadConfig(const std::string& filename) {

        json ConfigData;
        std::ifstream configFile(MenuConfig::path + '\\' + filename);
        if (configFile)
        {
            configFile >> ConfigData;
        }
        else
        {
            return;
        }

        if (ConfigData.contains("ESP"))
        {
            ESPConfig::HotKey = ReadData(ConfigData["ESP"], { "Hotkey" }, VK_F6);
            ESPConfig::ESPenabled = ReadData(ConfigData["ESP"], { "Enable" }, false);
            ESPConfig::ShowBoneESP = ReadData(ConfigData["ESP"], { "BoneESP" }, false);
            ESPConfig::ShowBoxESP = ReadData(ConfigData["ESP"],{"BoxESP"}, false);
            ESPConfig::BoxType = ReadData(ConfigData["ESP"], { "BoxType" }, 0);
            ESPConfig::ShowLineToEnemy = ReadData(ConfigData["ESP"], { "SnapLine" }, false);
            ESPConfig::LinePos = ReadData(ConfigData["ESP"], { "LinePos" }, 0);
            ESPConfig::ShowHealthBar = ReadData(ConfigData["ESP"], { "HealthBar" }, false);
            ESPConfig::AmmoBar = ReadData(ConfigData["ESP"], { "AmmoBar" }, false);
            ESPConfig::ShowWeaponESP = ReadData(ConfigData["ESP"], { "WeaponESP" }, false);
            ESPConfig::ShowEyeRay = ReadData(ConfigData["ESP"], { "EyeRay" }, false);
            ESPConfig::ShowPlayerName = ReadData(ConfigData["ESP"], { "PlayerName" }, false);
            ESPConfig::ShowDistance = ReadData(ConfigData["ESP"], { "DistanceESP" }, false);
            ESPConfig::ShowHealthNum = ReadData(ConfigData["ESP"], { "HealthNum" }, false);
            ESPConfig::ShowHeadBox = ReadData(ConfigData["ESP"], { "HeadBox" }, false);
            ESPConfig::ShowPreview = ReadData(ConfigData["ESP"], { "Preview" }, false);
            ESPConfig::VisibleCheck = ReadData(ConfigData["ESP"], { "VisCheck" }, false);
            ESPConfig::FilledBox = ReadData(ConfigData["ESP"], { "FilledBox" }, false);
            ESPConfig::FilledVisBox = ReadData(ConfigData["ESP"], { "FilledVisBox" }, false);
            ESPConfig::MultiColor = ReadData(ConfigData["ESP"], { "MultiColor" }, false);
            ESPConfig::OutLine = ReadData(ConfigData["ESP"], { "OutLine" }, false);
            ESPConfig::BoxRounding = ReadData(ConfigData["ESP"], { "BoxRounding" }, 0.f);
            ESPConfig::ShowIsScoped = ReadData(ConfigData["ESP"], { "ShowScoped" }, false);
            ESPConfig::ShowIsBlind = ReadData(ConfigData["ESP"], { "ShowBlind" }, false);
            ESPConfig::FlashCheck = ReadData(ConfigData["ESP"], { "DisableFlashed" }, false);
            ESPConfig::ArmorBar = ReadData(ConfigData["ESP"], { "ArmorBar" }, false);
            ESPConfig::ShowArmorNum = ReadData(ConfigData["ESP"], { "ArmorNum" }, false);

            ESPConfig::ShowOutOfFOVArrow = ReadData(ConfigData["ESP"], { "OutOfFOVArrow" }, false);
            ESPConfig::OutOfFOVRadiusFactor = ReadData(ConfigData["ESP"], { "OutOfFOVRadius" }, 0.45f);
            ESPConfig::OutOfFOVArrowColor.Value.x = ReadData(ConfigData["ESP"], { "OutOfFOVColor","r" }, 0.f);
            ESPConfig::OutOfFOVArrowColor.Value.y = ReadData(ConfigData["ESP"], { "OutOfFOVColor","g" }, 0.f);
            ESPConfig::OutOfFOVArrowColor.Value.z = ReadData(ConfigData["ESP"], { "OutOfFOVColor","b" }, 0.f);
            ESPConfig::OutOfFOVArrowColor.Value.w = ReadData(ConfigData["ESP"], { "OutOfFOVColor","a" }, 255.f);

            ESPConfig::BoneColor.Value.x = ReadData(ConfigData["ESP"], { "BoneColor","r" }, 0.f);
            ESPConfig::BoneColor.Value.y = ReadData(ConfigData["ESP"], { "BoneColor","g" }, 0.f);
            ESPConfig::BoneColor.Value.z = ReadData(ConfigData["ESP"], { "BoneColor","b" }, 0.f);
            ESPConfig::BoneColor.Value.w = ReadData(ConfigData["ESP"], { "BoneColor","a" }, 255.f);

            ESPConfig::BoxColor.Value.x = ReadData(ConfigData["ESP"], { "BoxColor","r" }, 0.f);
            ESPConfig::BoxColor.Value.y = ReadData(ConfigData["ESP"], { "BoxColor","g" }, 0.f);
            ESPConfig::BoxColor.Value.z = ReadData(ConfigData["ESP"], { "BoxColor","b" }, 0.f);
            ESPConfig::BoxColor.Value.w = ReadData(ConfigData["ESP"], { "BoxColor","a" }, 255.f);

            ESPConfig::LineToEnemyColor.Value.x = ReadData(ConfigData["ESP"], { "SnapLineColor","r" }, 0.f);
            ESPConfig::LineToEnemyColor.Value.y = ReadData(ConfigData["ESP"], { "SnapLineColor","g" }, 0.f);
            ESPConfig::LineToEnemyColor.Value.z = ReadData(ConfigData["ESP"], { "SnapLineColor","b" }, 0.f);
            ESPConfig::LineToEnemyColor.Value.w = ReadData(ConfigData["ESP"], { "SnapLineColor","a" }, 255.f);

            ESPConfig::HeadBoxColor.Value.x = ReadData(ConfigData["ESP"], { "HeadBoxColor","r" }, 0.f);
            ESPConfig::HeadBoxColor.Value.y = ReadData(ConfigData["ESP"], { "HeadBoxColor","g" }, 0.f);
            ESPConfig::HeadBoxColor.Value.z = ReadData(ConfigData["ESP"], { "HeadBoxColor","b" }, 0.f);
            ESPConfig::HeadBoxColor.Value.w = ReadData(ConfigData["ESP"], { "HeadBoxColor","a" }, 255.f);

            ESPConfig::VisibleColor.Value.x = ReadData(ConfigData["ESP"], { "VisibleColor","r" }, 0.f);
            ESPConfig::VisibleColor.Value.y = ReadData(ConfigData["ESP"], { "VisibleColor","g" }, 0.f);
            ESPConfig::VisibleColor.Value.z = ReadData(ConfigData["ESP"], { "VisibleColor","b" }, 0.f);
            ESPConfig::VisibleColor.Value.w = ReadData(ConfigData["ESP"], { "VisibleColor","a" }, 255.f);

            ESPConfig::FilledColor.Value.x = ReadData(ConfigData["ESP"], { "FilledColor","r" }, 0.f);
            ESPConfig::FilledColor.Value.y = ReadData(ConfigData["ESP"], { "FilledColor","g" }, 0.f);
            ESPConfig::FilledColor.Value.z = ReadData(ConfigData["ESP"], { "FilledColor","b" }, 0.f);
            ESPConfig::FilledColor.Value.w = ReadData(ConfigData["ESP"], { "FilledColor","a" }, 255.f);

            ESPConfig::FilledColor2.Value.x = ReadData(ConfigData["ESP"], { "FilledColor2","r" }, 0.f);
            ESPConfig::FilledColor2.Value.y = ReadData(ConfigData["ESP"], { "FilledColor2","g" }, 0.f);
            ESPConfig::FilledColor2.Value.z = ReadData(ConfigData["ESP"], { "FilledColor2","b" }, 0.f);
            ESPConfig::FilledColor2.Value.w = ReadData(ConfigData["ESP"], { "FilledColor2","a" }, 255.f);

            ESPConfig::BoxFilledVisColor.Value.x = ReadData(ConfigData["ESP"], { "FilledVisColor","r" }, 0.f);
            ESPConfig::BoxFilledVisColor.Value.y = ReadData(ConfigData["ESP"], { "FilledVisColor","g" }, 0.f);
            ESPConfig::BoxFilledVisColor.Value.z = ReadData(ConfigData["ESP"], { "FilledVisColor","b" }, 0.f);
            ESPConfig::BoxFilledVisColor.Value.w = ReadData(ConfigData["ESP"], { "FilledVisColor","a" }, 255.f);

            ESPConfig::EyeRayColor.Value.x = ReadData(ConfigData["ESP"], { "EyeRayColor","r" }, 0.f);
            ESPConfig::EyeRayColor.Value.y = ReadData(ConfigData["ESP"], { "EyeRayColor","g" }, 0.f);
            ESPConfig::EyeRayColor.Value.z = ReadData(ConfigData["ESP"], { "EyeRayColor","b" }, 0.f);
            ESPConfig::EyeRayColor.Value.w = ReadData(ConfigData["ESP"], { "EyeRayColor","a" }, 255.f);

            ESPConfig::EnemySound = ReadData(ConfigData["ESP"], { "EnemySound" }, false);
            ESPConfig::EnemySoundColor.Value.x = ReadData(ConfigData["ESP"], { "EnemySoundColor","r" }, 0.f);
            ESPConfig::EnemySoundColor.Value.y = ReadData(ConfigData["ESP"], { "EnemySoundColor","g" }, 0.f);
            ESPConfig::EnemySoundColor.Value.z = ReadData(ConfigData["ESP"], { "EnemySoundColor","b" }, 0.f);
            ESPConfig::EnemySoundColor.Value.w = ReadData(ConfigData["ESP"], { "EnemySoundColor","a" }, 255.f);

        }

        //if (ConfigData.contains("Crosshairs")) 
        //{
        //    CrosshairsCFG::ShowCrossHair = ReadData(ConfigData["Crosshairs"], { "Enable" }, false);
        //    CrosshairsCFG::CrossHairSize = ReadData(ConfigData["Crosshairs"], { "Size" }, 0.f);
        //    CrosshairsCFG::drawDot = ReadData(ConfigData["Crosshairs"], { "Dot" }, false);
        //    CrosshairsCFG::drawCrossline = ReadData(ConfigData["Crosshairs"], { "Crossline" }, false);
        //    CrosshairsCFG::tStyle = ReadData(ConfigData["Crosshairs"], { "tStyle" }, false);
        //    CrosshairsCFG::drawCircle = ReadData(ConfigData["Crosshairs"], { "Circle" }, false);
        //    CrosshairsCFG::drawOutLine = ReadData(ConfigData["Crosshairs"], { "Outline" }, false);
        //    CrosshairsCFG::DynamicGap = ReadData(ConfigData["Crosshairs"], { "DynamicGap" }, false);
        //    CrosshairsCFG::TeamCheck = ReadData(ConfigData["Crosshairs"], { "TeamCheck" }, false);
        //    CrosshairsCFG::crosshairPreset = ReadData(ConfigData["Crosshairs"], { "Preset" }, 0);
        //    CrosshairsCFG::Gap = ReadData(ConfigData["Crosshairs"], { "Gap" }, 0);
        //    CrosshairsCFG::HorizontalLength = ReadData(ConfigData["Crosshairs"], { "H_Length" }, 0);
        //    CrosshairsCFG::VerticalLength = ReadData(ConfigData["Crosshairs"], { "V_Length" }, 0);
        //    CrosshairsCFG::Thickness = ReadData(ConfigData["Crosshairs"], { "Thickness" }, 0);
        //    CrosshairsCFG::DotSize = ReadData(ConfigData["Crosshairs"], { "DotSize" }, 0.f);
        //    CrosshairsCFG::CircleRadius = ReadData(ConfigData["Crosshairs"], { "CircleRadius" }, 0.f);
        //    MenuConfig::TargetingCrosshairs = ReadData(ConfigData["Crosshairs"], { "TargetCheck" }, false);

        //    CrosshairsCFG::CrossHairColor.Value.x = ReadData(ConfigData["Crosshairs"], { "CrosshairsColor","r" }, 0.f);
        //    CrosshairsCFG::CrossHairColor.Value.y = ReadData(ConfigData["Crosshairs"], { "CrosshairsColor","g" }, 0.f);
        //    CrosshairsCFG::CrossHairColor.Value.z = ReadData(ConfigData["Crosshairs"], { "CrosshairsColor","b" }, 0.f);
        //    CrosshairsCFG::CrossHairColor.Value.w = ReadData(ConfigData["Crosshairs"], { "CrosshairsColor","a" }, 0.f);

        //    CrosshairsCFG::TargetedColor.Value.x = ReadData(ConfigData["Crosshairs"], { "TargetedColor","r" }, 0.f);
        //    CrosshairsCFG::TargetedColor.Value.y = ReadData(ConfigData["Crosshairs"], { "TargetedColor","g" }, 0.f);
        //    CrosshairsCFG::TargetedColor.Value.z = ReadData(ConfigData["Crosshairs"], { "TargetedColor","b" }, 0.f);
        //    CrosshairsCFG::TargetedColor.Value.w = ReadData(ConfigData["Crosshairs"], { "TargetedColor","a" }, 0.f);

        //}

        if (ConfigData.contains("Radar"))
        {
            RadarCFG::ShowRadar = ReadData(ConfigData["Radar"],{"Enable"}, false);
            RadarCFG::RadarType = ReadData(ConfigData["Radar"],{"Type"}, 2);
            RadarCFG::RadarRange = ReadData(ConfigData["Radar"],{"Range"}, 150.f);
            RadarCFG::Proportion = ReadData(ConfigData["Radar"],{"Proportion"}, 3300.f);
            RadarCFG::RadarPointSizeProportion = ReadData(ConfigData["Radar"],{"PointProportion"}, 1.f);
            RadarCFG::RadarBgAlpha = ReadData(ConfigData["Radar"],{"Alpha"}, 0.1f);
            RadarCFG::customRadar = ReadData(ConfigData["Radar"],{"Custom"}, false);
            RadarCFG::ShowRadarCrossLine = ReadData(ConfigData["Radar"],{"Crossline"}, false);
            RadarCFG::RadarCrossLineColor.Value.x = ReadData(ConfigData["Radar"],{"CrosslineColor","r"}, 0.f);
            RadarCFG::RadarCrossLineColor.Value.y = ReadData(ConfigData["Radar"],{"CrosslineColor","g"}, 0.f);
            RadarCFG::RadarCrossLineColor.Value.z = ReadData(ConfigData["Radar"],{"CrosslineColor","b"}, 0.f);
            RadarCFG::RadarCrossLineColor.Value.w = ReadData(ConfigData["Radar"],{"CrosslineColor","a"}, 255.f);

        }

        if (ConfigData.contains("Aimbot"))
        {
            LegitBotConfig::AimBot = ReadData(ConfigData["Aimbot"],{"Enable"}, false);
            LegitBotConfig::AimToggleMode = ReadData(ConfigData["Aimbot"],{"ToggleMode"}, false);
            AimControl::HotKey = ReadData(ConfigData["Aimbot"],{"Hotkey"}, 0);
            AimControl::AimBullet = ReadData(ConfigData["Aimbot"],{"AimBullet"}, 0);
            AimControl::AimFov = ReadData(ConfigData["Aimbot"],{"Fov"}, 5.f);
            AimControl::HumanizationStrength = ReadData(ConfigData["Aimbot"],{"HumanizationStrength"}, 5);
            AimControl::AimFovMin = ReadData(ConfigData["Aimbot"],{"FovMin"}, .5f);
            ESPConfig::DrawFov = ReadData(ConfigData["Aimbot"],{"FovCircle"}, false);
            LegitBotConfig::FovCircleColor.Value.x = ReadData(ConfigData["Aimbot"],{"CircleColor","r"}, 0.f);
            LegitBotConfig::FovCircleColor.Value.y = ReadData(ConfigData["Aimbot"],{"CircleColor","g"}, 0.f);
            LegitBotConfig::FovCircleColor.Value.z = ReadData(ConfigData["Aimbot"],{"CircleColor","b"}, 0.f);
            LegitBotConfig::FovCircleColor.Value.w = ReadData(ConfigData["Aimbot"],{"CircleColor","a"}, 255.f);
            AimControl::Smooth = ReadData(ConfigData["Aimbot"],{"Smooth"}, 2.f);
            AimControl::HitboxList = LoadVector(ConfigData["Aimbot"], "Hitboxes", { BONEINDEX::head });
            LegitBotConfig::VisibleCheck = ReadData(ConfigData["Aimbot"],{"VisibleCheck"}, true);
            AimControl::IgnoreFlash = ReadData(ConfigData["Aimbot"],{"IgnoreFlash"}, false);
            AimControl::HumanizeVar = ReadData(ConfigData["Aimbot"],{"HumanizeVar"}, true);
            AimControl::ScopeOnly = ReadData(ConfigData["Aimbot"],{"ScopeOnly"}, false);
            AimControl::onlyAuto = ReadData(ConfigData["Aimbot"], { "OnlyAuto" }, false);
            Text::Aimbot::HotKey = KeyMgr::GetKeyName(AimControl::HotKey);
            LegitBotConfig::HitboxUpdated = false;
        }

        if (ConfigData.contains("RCS"))
        {
            LegitBotConfig::RCS = ReadData(ConfigData["RCS"],{"Enable"}, true);
            RCS::RCSBullet = ReadData(ConfigData["RCS"], { "RCSBullet" }, 1);
            RCS::RCSScale.x = ReadData(ConfigData["RCS"],{"Yaw"}, 1.f);
            RCS::RCSScale.y = ReadData(ConfigData["RCS"],{"Pitch"}, 1.f);
        }

        if (ConfigData.contains("Triggerbot"))
        {
            LegitBotConfig::TriggerBot = ReadData(ConfigData["Triggerbot"],{"Enable"}, false);
            TriggerBot::HotKey = ReadData(ConfigData["Triggerbot"],{"Hotkey"}, 6);
            TriggerBot::TriggerDelay = ReadData(ConfigData["Triggerbot"],{"Delay"}, 20);
            TriggerBot::ShotDuration = ReadData(ConfigData["Triggerbot"],{"FakeShot"}, 200);
            TriggerBot::ScopeOnly = ReadData(ConfigData["Triggerbot"],{"ScopeOnly"}, false);
            TriggerBot::IgnoreFlash = ReadData(ConfigData["Triggerbot"],{"IgnoreFlash"}, false);
            TriggerBot::StopedOnly = ReadData(ConfigData["Triggerbot"], { "StopedOnly" }, false);
            TriggerBot::TTDtimeout = ReadData(ConfigData["Triggerbot"], { "TTDtimeout" }, false);
            LegitBotConfig::TriggerAlways = ReadData(ConfigData["Triggerbot"],{"AutoMode"}, false);
            Text::Trigger::HotKey = KeyMgr::GetKeyName(TriggerBot::HotKey);
        }

        if (ConfigData.contains("Misc"))
        {
            MiscCFG::ShowHeadShootLine = ReadData(ConfigData["Misc"],{"HeadShootLine"}, false);
            MiscCFG::HeadShootLineColor.Value.x = ReadData(ConfigData["Misc"],{"HeadShootLineColor","r"}, 0.f);
            MiscCFG::HeadShootLineColor.Value.y = ReadData(ConfigData["Misc"],{"HeadShootLineColor","g"}, 0.f);
            MiscCFG::HeadShootLineColor.Value.z = ReadData(ConfigData["Misc"],{"HeadShootLineColor","b"}, 0.f);
            MiscCFG::HeadShootLineColor.Value.w = ReadData(ConfigData["Misc"],{"HeadShootLineColor","a"}, 255.f);
            MenuConfig::WorkInSpec = ReadData(ConfigData["Misc"],{"WorkInSpec"}, false);
            MiscCFG::WaterMark = ReadData(ConfigData["Misc"],{"Watermark"}, false);
            MiscCFG::HitSound = ReadData(ConfigData["Misc"],{"HitSounds"}, 0);
            MiscCFG::HitMarker = ReadData(ConfigData["Misc"],{"HitMarker"}, false);
            MiscCFG::bmbTimer = ReadData(ConfigData["Misc"],{"BombTimer"}, false);
            MiscCFG::BombTimerCol.Value.x = ReadData(ConfigData["Misc"],{"TimerColor","r"}, 0.f);
            MiscCFG::BombTimerCol.Value.y = ReadData(ConfigData["Misc"],{"TimerColor","g"}, 0.f);
            MiscCFG::BombTimerCol.Value.z = ReadData(ConfigData["Misc"],{"TimerColor","b"}, 0.f);
            MiscCFG::BombTimerCol.Value.w = ReadData(ConfigData["Misc"],{"TimerColor","a"}, 255.f);
            MiscCFG::BunnyHop = ReadData(ConfigData["Misc"],{"Bhop"}, false);
            MiscCFG::FastStop = ReadData(ConfigData["Misc"],{"FastStop"}, false);
            MiscCFG::SpecList = ReadData(ConfigData["Misc"],{"SpecList"}, false);
            MiscCFG::SniperCrosshair = ReadData(ConfigData["Misc"], { "SniperCrosshair" }, false);
            MiscCFG::SniperCrosshairColor.Value.x = ReadData(ConfigData["Misc"], { "SniperCrosshairColor","r" }, 0.f);
            MiscCFG::SniperCrosshairColor.Value.y = ReadData(ConfigData["Misc"], { "SniperCrosshairColor","g" }, 0.f);
            MiscCFG::SniperCrosshairColor.Value.z = ReadData(ConfigData["Misc"], { "SniperCrosshairColor","b" }, 0.f);
            MiscCFG::SniperCrosshairColor.Value.w = ReadData(ConfigData["Misc"], { "SniperCrosshairColor","a" }, 255.f);

            MiscCFG::AutoAccept = ReadData(ConfigData["Misc"], { "AutoAccept" }, false);
            MiscCFG::AutoKnife = ReadData(ConfigData["Misc"], { "AutoKnife" }, false);
            MiscCFG::AutoKnifeDistance = ReadData(ConfigData["Misc"], { "AutoKnifeDistance" }, 70.0f);
            MiscCFG::AutoZeus = ReadData(ConfigData["Misc"], { "AutoZeus" }, false);
            MiscCFG::AutoZeusDistance = ReadData(ConfigData["Misc"], { "AutoZeusDistance" }, 130.0f);
            MiscCFG::AntiAFKKick = ReadData(ConfigData["Misc"], { "AntiAFKKick" }, false);

            MiscCFG::AutoKnife = ReadData(ConfigData["Misc"], { "AutoKnife" }, false);
            MiscCFG::AutoKnifeDistance = ReadData(ConfigData["Misc"], { "AutoKnifeDistance" }, 70.0f);
            MiscCFG::AutoZeus = ReadData(ConfigData["Misc"], { "AutoZeus" }, false);
            MiscCFG::AutoZeusDistance = ReadData(ConfigData["Misc"], { "AutoZeusDistance" }, 130.0f);
            MiscCFG::AntiAFKKick = ReadData(ConfigData["Misc"], { "AntiAFKKick" }, false);

            MenuConfig::TeamCheck = ReadData(ConfigData["Misc"],{"TeamCheck"}, true);
            MenuConfig::BypassOBS = ReadData(ConfigData["Misc"],{"AntiRecord"}, false);
            MenuConfig::HotKey = ReadData(ConfigData["Misc"], { "MenuKey" }, VK_END);
            Text::Misc::HotKey = KeyMgr::GetKeyName(MenuConfig::HotKey);
        }

        if (ConfigData.contains("MenuConfig"))
        {
            MenuConfig::MarkWinPos.x = ReadData(ConfigData["MenuConfig"], { "MarkWinPos","x" }, ImGui::GetIO().DisplaySize.x - 300.0f);
            MenuConfig::MarkWinPos.y = ReadData(ConfigData["MenuConfig"], { "MarkWinPos","y" }, 100.f);

            MenuConfig::BombWinPos.x = ReadData(ConfigData["MenuConfig"], { "BombWinPos","x" }, (ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f);
            MenuConfig::BombWinPos.y = ReadData(ConfigData["MenuConfig"], { "BombWinPos","y" }, 80.0f);

            MenuConfig::RadarWinPos.x = ReadData(ConfigData["MenuConfig"], { "RadarWinPos","x" }, 0.f);
            MenuConfig::RadarWinPos.y = ReadData(ConfigData["MenuConfig"], { "RadarWinPos","y" }, 0.f);

            MenuConfig::SpecWinPos.x = ReadData(ConfigData["MenuConfig"], { "SpecWinPos","x" }, 10.0f);
            MenuConfig::SpecWinPos.y = ReadData(ConfigData["MenuConfig"], { "SpecWinPos","y" }, ImGui::GetIO().DisplaySize.y / 2 - 200);

            MenuConfig::MarkWinChengePos = true;
            MenuConfig::BombWinChengePos = true;
            MenuConfig::RadarWinChengePos = true;
            MenuConfig::SpecWinChengePos = true;
        }
    }
}
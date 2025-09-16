#pragma once
#include <iostream>
#include <string>
#include <windows.h>

namespace Text {
    namespace Global 
    {
        inline std::string SwitchButton = "Enable";
        inline std::string FeatureSettings = "Settings";
    }

    namespace ESP
    {
        inline std::string Enable = "Enable ESP";
        inline std::string HotKeyList = "Hotkey";
        inline std::string HotKey = "F6";
        inline std::string FeatureName = "ESP";
        inline std::string Box = "Box";
        inline std::string BoxRounding = "Box Rounding: ";
        inline std::string FilledBox = "Filled Box";
        inline std::string FilledAlpha = "Filled Box Alpha";
        inline std::string Skeleton = "Skeleton";
        inline std::string HeadBox = "Head ESP";
        inline std::string EyeRay = "Eye Ray";
        inline std::string HealthBar = "Health Bar";
        inline std::string Weapon = "Weapon Icon";
        inline std::string Distance = "Distance";
        inline std::string PlayerName = "Player Name";
        inline std::string SnapLine = "Snap Line";
        inline std::string LinePosList = "   Line Position";
        inline std::string VisCheck = "Visible Color";
        inline std::string Preview = "ESP Preview";
        inline std::string MultiColor = "Multicoloured";
        inline std::string MultiColTip = "Only work when the box has non-rounded corners.";
        inline std::string Outline = "Outline";
        inline std::string BoxType = "   Box Type";
        inline std::string HealthNum = "Health Number";
        inline std::string Ammo = "Ammo Bar";
        inline std::string ScopedESP = "Show Scoped";
        inline std::string FlashedESP = "Show Flashed";
        inline std::string FlashCheck = "Disable When Flashed";
        inline std::string ShowArmorBar = "Armor Bar";
        inline std::string ArmorNum = "Armor Number";
        inline std::string OutOfFOVArrow = "Offscreen";
        inline std::string OutOfFOVRadius = "Arrow Radius Factor";
        inline std::string SoundEsp = "Sound esp";
    }

    namespace Aimbot
    {
        inline std::string Enable = "Enable Aimbot";
        inline std::string FeatureName = "Aimbot";
        inline std::string HotKeyList = "Hotkey";
        inline std::string HotKey = "LMB";
        inline std::string Toggle = "Toggle Mode";
        inline std::string DrawFov = "Show Fov";
        inline std::string VisCheck = "Visible Check";
        inline std::string JumpCheck = "On Ground Check";
        inline std::string FovSlider = "FOV: ";
        inline std::string HumanizationStrength = "Humanization Strength: ";
        inline std::string FovMinSlider = "MiniFOV: ";
        inline std::string SmoothSlider = "Smooth: ";
        inline std::string BoneList = "Hitboxes";
        inline std::string Tip = "Aimbot will not work while the menu is opened";
        inline std::string ScopeOnly = "Scope Check";
        inline std::string BulletSlider = "Start Bullet: ";
        inline std::string IgnoreFlash = "Ignore Flash";
        inline std::string HumanizeVar = "Humanize Aimbot";
        inline std::string OnlyAuto = "Only Auto";
        inline std::string OnlyAutoTip = "Aim will only work in automatic shooting mode";
        inline std::string SmoothTip = "It's better to keep this above 6 to avoid VAC Live";
        inline std::string StartBulletTip = "It's better to keep this above 1 to avoid VAC Live";
        inline std::string OnTip = "It's better to keep this on to avoid VAC Live";
        inline std::string OffTip = "It's better to keep this off to avoid VAC Live";
    }

    namespace RCS
    {
        inline std::string BulletSlider = "Start Bullet:";
        inline std::string Toggle = "Recoil Control";
        inline std::string Yaw = "Yaw: ";
        inline std::string Pitch = "Pitch: ";
    }

    namespace Radar
    {
        inline std::string Toggle = "Show Radar";
        inline std::string FeatureName = "Radar";
        inline std::string StyleList = "Style";
        inline std::string CustomCheck = "Custom";
        inline std::string CrossLine = "Cross Line";
        inline std::string SizeSlider = "Point Size:";
        inline std::string ProportionSlider = "Proportion:";
        inline std::string RangeSlider = "Range:";
        inline std::string AlphaSlider = "Background Alpha:";
    }

    namespace Trigger
    {
        inline std::string Enable = "Enable Triggerbot";
        inline std::string FeatureName = "Triggerbot";
        inline std::string HotKeyList = "Hotkey";
        inline std::string HotKey = "X2MB";
        inline std::string Toggle = "Always Active";
        inline std::string ScopeOnly = "Scope Only";
        inline std::string WorkWithAimbot = "Work With Aimbot";
        inline std::string DelaySlider = "Shot Delay:";
        inline std::string FakeShotSlider = "Shot Duration:";
        inline std::string IgnoreFlash = "Ignore Flash";
        inline std::string StopOnly = "Stop Check";
        inline std::string VisCheck = "Visible Check";
        inline std::string DelayTip = "Using it try keep your TTD above 350ms";
        inline std::string TTDtimeout = "Wait min TTD";
    }

    namespace Misc
    {
        inline std::string FeatureName = "Misc";
        inline std::string MenuKey = " Menu Key";
        inline std::string HotKey = "End";
        inline std::string StyleList = "Style";
        inline std::string HeadshotLine = "Headshot Line";
        inline std::string SpecCheck = "Work in Spec";
        inline std::string BunnyHop = "Bunny Hop";
        inline std::string HitSound = "Hit Sound";
        inline std::string HitMerker = "Hit Markers";
        inline std::string bmbTimer = "Bomb Timer";
        inline std::string SpecList = "Spec List";
        inline std::string Watermark = "Watermark";
        inline std::string TeamCheck = "Team Check";
        inline std::string AntiRecord = "Anti Record";
        inline std::string SniperCrosshair = "Sniper Crosshair";
        inline std::string FastStop = "Fast Stop";
        inline std::string AutoKnifeBot = "Auto Knife Bot";
        inline std::string AutoZeusBot = "Auto Zeus Bot";
        inline std::string AutoBotDistance = "Distance";
        inline std::string AntiAFK = "Anti AFK";
        inline std::string AntiAFKInterval = "Interval";
        inline std::string InsecureTip = "This option may trigger VAC Live";
    }

    namespace Config
    {
        inline std::string FeatureName = "Config Loader";
        inline std::string MyConfigs = "Config List";
        inline std::string Load = "Load";
        inline std::string Save = "Save";
        inline std::string Delete = "Delete";
        inline std::string Reset = "Reset";
        inline std::string Create = "Create";
        inline std::string OpenFolder = "Open Folder";
        inline std::string SeparateLine = "Create Config";
        inline std::string AuthorName = "Author Name";
        inline std::string ConfigName = "Config Name";
    }
}


// Crosshairs
//CrosshairsText.Toggle = "Show Crosshairs";
//CrosshairsText.FeatureName = " Crosshairs";
//CrosshairsText.PresetList = "Presets";
//CrosshairsText.ColorEditor = "Crosshairs Color";
//CrosshairsText.Dot = "Center Dot";
//CrosshairsText.DotSizeSlider = "Dot Size:";
//CrosshairsText.Outline = "Crosshairs Outline";
//CrosshairsText.Crossline = "Crossline";
//CrosshairsText.hLengthSlider = "Horizontal Length:";
//CrosshairsText.vLengthSilder = "Vertical Length:";
//CrosshairsText.GapSlider = "Gap:";
//CrosshairsText.ThicknessSlider = "Thickness:";
//CrosshairsText.tStyle = "T Style";
//CrosshairsText.Circle = "Circle";
//CrosshairsText.RadiusSlider = "Circle Radius:";
//CrosshairsText.TargetCheck = "Targeting Crosshairs";
//CrosshairsText.TeamCheck = "TeamCheck";

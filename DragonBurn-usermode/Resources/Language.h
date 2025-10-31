#pragma once
#include <iostream>
#include <string>
#include <windows.h>

namespace Text {
    namespace detail {
        inline std::string to_utf8(const wchar_t* str) {
            if (!str)
                return {};
            int required = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
            if (required <= 1)
                return {};
            std::string result(static_cast<size_t>(required - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, str, -1, result.data(), required, nullptr, nullptr);
            return result;
        }

        inline std::string to_utf8(const char* str) {
            return str ? std::string(str) : std::string{};
        }

#if defined(__cpp_char8_t)
        inline std::string to_utf8(const char8_t* str) {
            return str ? std::string(reinterpret_cast<const char*>(str)) : std::string{};
        }
#endif
    }

    namespace Global 
    {
        inline std::string SwitchButton = detail::to_utf8(L"启用");
        inline std::string FeatureSettings = detail::to_utf8(L"设置");
        inline std::string Confirm = detail::to_utf8(L"确认");
        inline std::string Cancel = detail::to_utf8(L"取消");
        inline std::string Yes = detail::to_utf8(L"是");
        inline std::string No = detail::to_utf8(L"否");
    }

    namespace ESP
    {
        inline std::string Enable = detail::to_utf8(L"启用透视");
        inline std::string HotKeyList = detail::to_utf8(L"热键");
        inline std::string HotKey = detail::to_utf8(L"F6");
        inline std::string FeatureName = detail::to_utf8(L"透视");
        inline std::string Box = detail::to_utf8(L"方框");
        inline std::string BoxRounding = detail::to_utf8(L"方框圆角: ");
        inline std::string FilledBox = detail::to_utf8(L"填充方框");
        inline std::string FilledAlpha = detail::to_utf8(L"填充透明度");
        inline std::string Skeleton = detail::to_utf8(L"骨骼");
        inline std::string HeadBox = detail::to_utf8(L"头部框");
        inline std::string EyeRay = detail::to_utf8(L"视线射线");
        inline std::string HealthBar = detail::to_utf8(L"血量条");
        inline std::string Weapon = detail::to_utf8(L"武器图标");
        inline std::string Distance = detail::to_utf8(L"距离");
        inline std::string PlayerName = detail::to_utf8(L"玩家名称");
        inline std::string SnapLine = detail::to_utf8(L"连线");
        inline std::string LinePosList = detail::to_utf8(L"   连线位置");
        inline std::string VisCheck = detail::to_utf8(L"可见颜色");
        inline std::string Preview = detail::to_utf8(L"透视预览");
        inline std::string MultiColor = detail::to_utf8(L"渐变填充");
        inline std::string MultiColTip = detail::to_utf8(L"仅在方框无圆角时生效。");
        inline std::string Outline = detail::to_utf8(L"描边");
        inline std::string BoxType = detail::to_utf8(L"   方框类型");
        inline std::string HealthNum = detail::to_utf8(L"血量数字");
        inline std::string Ammo = detail::to_utf8(L"弹药条");
        inline std::string ScopedESP = detail::to_utf8(L"瞄准提示");
        inline std::string FlashedESP = detail::to_utf8(L"致盲提示");
        inline std::string FlashCheck = detail::to_utf8(L"致盲时禁用");
        inline std::string ShowArmorBar = detail::to_utf8(L"护甲条");
        inline std::string ArmorNum = detail::to_utf8(L"护甲数字");
        inline std::string OutOfFOVArrow = detail::to_utf8(L"视野外箭头");
        inline std::string OutOfFOVRadius = detail::to_utf8(L"箭头半径比例");
        inline std::string SoundEsp = detail::to_utf8(L"声音提示");
    }

    namespace Aimbot
    {
        inline std::string Enable = detail::to_utf8(L"启用自瞄");
        inline std::string FeatureName = detail::to_utf8(L"自瞄");
        inline std::string HotKeyList = detail::to_utf8(L"热键");
        inline std::string HotKey = detail::to_utf8(L"鼠标左键");
        inline std::string Toggle = detail::to_utf8(L"切换模式");
        inline std::string DrawFov = detail::to_utf8(L"显示 FOV");
        inline std::string VisCheck = detail::to_utf8(L"可见校验");
        inline std::string JumpCheck = detail::to_utf8(L"在地检测");
        inline std::string FovSlider = detail::to_utf8(L"FOV: ");
        inline std::string HumanizationStrength = detail::to_utf8(L"人性化强度: ");
        inline std::string FovMinSlider = detail::to_utf8(L"最小 FOV: ");
        inline std::string SmoothSlider = detail::to_utf8(L"平滑: ");
        inline std::string BoneList = detail::to_utf8(L"瞄准骨骼");
        inline std::string Tip = detail::to_utf8(L"打开菜单时自瞄不会工作");
        inline std::string ScopeOnly = detail::to_utf8(L"仅开镜生效");
        inline std::string BulletSlider = detail::to_utf8(L"起始子弹: ");
        inline std::string IgnoreFlash = detail::to_utf8(L"忽略致盲");
        inline std::string HumanizeVar = detail::to_utf8(L"启用人性化");
        inline std::string OnlyAuto = detail::to_utf8(L"仅自动武器");
        inline std::string OnlyAutoTip = detail::to_utf8(L"仅在自动开火模式下生效");
        inline std::string SmoothTip = detail::to_utf8(L"建议保持大于 6 以降低 VAC Live 风险");
        inline std::string StartBulletTip = detail::to_utf8(L"建议设置为 1 以上以降低 VAC Live 风险");
        inline std::string OnTip = detail::to_utf8(L"建议开启以降低 VAC Live 风险");
        inline std::string OffTip = detail::to_utf8(L"建议关闭以降低 VAC Live 风险");
    }

    namespace RCS
    {
        inline std::string BulletSlider = detail::to_utf8(L"起始子弹:");
        inline std::string Toggle = detail::to_utf8(L"后坐力控制");
        inline std::string Yaw = detail::to_utf8(L"水平: ");
        inline std::string Pitch = detail::to_utf8(L"垂直: ");
    }

    namespace Radar
    {
        inline std::string Toggle = detail::to_utf8(L"显示雷达");
        inline std::string FeatureName = detail::to_utf8(L"雷达");
        inline std::string StyleList = detail::to_utf8(L"样式");
        inline std::string CustomCheck = detail::to_utf8(L"自定义");
        inline std::string CrossLine = detail::to_utf8(L"十字线");
        inline std::string SizeSlider = detail::to_utf8(L"点大小:");
        inline std::string ProportionSlider = detail::to_utf8(L"缩放:");
        inline std::string RangeSlider = detail::to_utf8(L"范围:");
        inline std::string AlphaSlider = detail::to_utf8(L"背景透明度:");
    }

    namespace Trigger
    {
        inline std::string Enable = detail::to_utf8(L"启用扳机");
        inline std::string FeatureName = detail::to_utf8(L"扳机");
        inline std::string HotKeyList = detail::to_utf8(L"热键");
        inline std::string HotKey = detail::to_utf8(L"鼠标侧键2");
        inline std::string Toggle = detail::to_utf8(L"始终激活");
        inline std::string ScopeOnly = detail::to_utf8(L"仅开镜");
        inline std::string WorkWithAimbot = detail::to_utf8(L"配合自瞄");
        inline std::string DelaySlider = detail::to_utf8(L"延迟:");
        inline std::string FakeShotSlider = detail::to_utf8(L"射击时长:");
        inline std::string IgnoreFlash = detail::to_utf8(L"忽略致盲");
        inline std::string StopOnly = detail::to_utf8(L"静止检测");
        inline std::string VisCheck = detail::to_utf8(L"可见校验");
        inline std::string DelayTip = detail::to_utf8(L"尽量保持 TTD 大于 350 毫秒");
        inline std::string TTDtimeout = detail::to_utf8(L"等待最小 TTD");
    }

    namespace Misc
    {
        inline std::string FeatureName = detail::to_utf8(L"杂项");
        inline std::string MenuKey = detail::to_utf8(L" 菜单键");
        inline std::string HotKey = detail::to_utf8(L"End");
        inline std::string StyleList = detail::to_utf8(L"样式");
        inline std::string HeadshotLine = detail::to_utf8(L"爆头提示线");
        inline std::string SpecCheck = detail::to_utf8(L"观战时生效");
        inline std::string BunnyHop = detail::to_utf8(L"连跳");
        inline std::string HitSound = detail::to_utf8(L"命中音效");
        inline std::string HitMerker = detail::to_utf8(L"命中提示");
        inline std::string bmbTimer = detail::to_utf8(L"炸弹计时");
        inline std::string SpecList = detail::to_utf8(L"观战列表");
        inline std::string Watermark = detail::to_utf8(L"水印");
        inline std::string TeamCheck = detail::to_utf8(L"队友过滤");
        inline std::string AntiRecord = detail::to_utf8(L"录制屏蔽");
        inline std::string SniperCrosshair = detail::to_utf8(L"狙击准星");
        inline std::string FastStop = detail::to_utf8(L"急停");
        inline std::string AutoKnifeBot = detail::to_utf8(L"自动匕首");
        inline std::string AutoZeusBot = detail::to_utf8(L"自动电击");
        inline std::string AutoBotDistance = detail::to_utf8(L"距离");
        inline std::string AntiAFK = detail::to_utf8(L"防挂机");
        inline std::string AntiAFKInterval = detail::to_utf8(L"间隔");
        inline std::string InsecureTip = detail::to_utf8(L"该选项可能触发 VAC Live");
        inline std::string AutoAccept = detail::to_utf8(L"自动接受");
        inline std::string SourceCode = detail::to_utf8(L"源代码");
        inline std::string ContactAuthor = detail::to_utf8(L"联系作者");
        inline std::string Uninstall = detail::to_utf8(L"卸载");
        inline std::string CleanTraces = detail::to_utf8(L"清理痕迹");
        inline std::string GlobalSettings = detail::to_utf8(L"全局设置");
    }

    namespace Config
    {
        inline std::string FeatureName = detail::to_utf8(L"配置管理");
        inline std::string MyConfigs = detail::to_utf8(L"配置列表");
        inline std::string Load = detail::to_utf8(L"加载");
        inline std::string Save = detail::to_utf8(L"保存");
        inline std::string Delete = detail::to_utf8(L"删除");
        inline std::string Reset = detail::to_utf8(L"重置");
        inline std::string Create = detail::to_utf8(L"创建");
        inline std::string OpenFolder = detail::to_utf8(L"打开文件夹");
        inline std::string SeparateLine = detail::to_utf8(L"新建配置");
        inline std::string AuthorName = detail::to_utf8(L"作者");
        inline std::string ConfigName = detail::to_utf8(L"配置名称");
        inline std::string ConfirmDelete = detail::to_utf8(L"   确认删除？   ");
        inline std::string ConfirmReset = detail::to_utf8(L"   确认重置？   ");
        inline std::string DefaultConfigName = detail::to_utf8(L"新配置");
        inline std::string DefaultAuthorName = detail::to_utf8(L"作者");
    }

    namespace Menu
    {
        inline std::string Title = detail::to_utf8(L"   DragonBurn 菜单");
        inline std::string ExternalRadar = detail::to_utf8(L"外部雷达");
        inline std::string Recoil = detail::to_utf8(L"压枪");
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

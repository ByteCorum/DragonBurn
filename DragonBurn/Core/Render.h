#pragma once
#define _USE_MATH_DEFINES
#define NOMINMAX
#include <cmath>
#include <chrono>
#include <map>
#include <unordered_map>
#include <Windows.h>
#include "../Game/Entity.h"
#include "../Helpers/Format.h"
#include "../OS-ImGui/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../OS-ImGui/imgui/imgui_internal.h"

namespace Render
{
	//--------------------------------------------------------------------------------------
	// Small per-frame cache to avoid repeated calls into ImGui and window size math
	//--------------------------------------------------------------------------------------
	namespace detail {
		inline ImDrawList* sDrawList = nullptr;
		inline ImVec2             sWindowSize{};
		inline ImVec2             sWindowCenter{};

		inline ImDrawList* GetDrawList() noexcept {
			// Fallback if user didn't call BeginFrame()
			return sDrawList ? sDrawList : ImGui::GetBackgroundDrawList();
		}

		inline void UpdateWindowCache() noexcept {
			// Rely on Gui.Window.Size instead of GetSystemMetrics (syscall)
			sWindowSize.x = Gui.Window.Size.x;
			sWindowSize.y = Gui.Window.Size.y;
			sWindowCenter = { sWindowSize.x * 0.5f, sWindowSize.y * 0.5f };
		}
	}

	// Call once at the start of your frame rendering (optional but recommended).
	inline void BeginFrame() noexcept {
		detail::sDrawList = ImGui::GetBackgroundDrawList();
		detail::UpdateWindowCache();
	}

	//--------------------------------------------------------------------------------------
	// Helpers
	//--------------------------------------------------------------------------------------
	inline ImVec2 GetScreenCenterImVec2() noexcept {
		// Uses cached window size; no system metrics calls every frame
		if (detail::sWindowSize.x == 0.f) detail::UpdateWindowCache();
		return detail::sWindowCenter;
	}

	inline ImVec4 rgba2ImVec(int r, int g, int b, int a) noexcept {
		const float inv = 1.0f / 255.0f;
		return { r * inv, g * inv, b * inv, a * inv };
	}

	// Faster linear color mix (keeps alpha of Col_1)
	inline ImColor Mix(const ImColor& a, const ImColor& b, float t) noexcept {
		// Clamp t
		if (t < 0.f) t = 0.f; else if (t > 1.f) t = 1.f;
		ImVec4 av = a.Value, bv = b.Value;
		return ImColor(
			t * av.x + (1.0f - t) * bv.x,
			t * av.y + (1.0f - t) * bv.y,
			t * av.z + (1.0f - t) * bv.z,
			av.w
		);
	}

	//--------------------------------------------------------------------------------------
	// ESP / Aim visuals
	//--------------------------------------------------------------------------------------
	inline void DrawHeadCircle(const CEntity& Entity, ImColor Color) noexcept
	{
		if (!ESPConfig::ShowHeadBox) return;

		const auto& bones = Entity.GetBone().BonePosList;
		if (bones.empty()) return;

		const BoneJointPos& Head = bones[BONEINDEX::head];
		const BoneJointPos& Neck = bones[BONEINDEX::neck_0];

		const float radius = std::fabs(Head.ScreenPos.y - Neck.ScreenPos.y) + 2.0f;
		Gui.Circle(Head.ScreenPos, radius, Color, 1.2f);
	}

	inline void DrawHealth(int Health, const ImVec2& Pos) noexcept
	{
		if (!ESPConfig::ShowHealthNum) return;

		char buf[8];
		_snprintf_s(buf, _TRUNCATE, "%d", Health);
		Gui.StrokeText(buf, Pos, ImColor(0, 220, 0, 255), 12, false);
	}

	inline void DrawDistance(const CEntity& LocalEntity, CEntity& Entity, ImVec4 Rect) noexcept
	{
		if (!ESPConfig::ShowDistance) return;

		const int distance = static_cast<int>(Entity.Pawn.Pos.DistanceTo(LocalEntity.Pawn.Pos)) / 100;
		char buf[16];
		_snprintf_s(buf, _TRUNCATE, "%dm", distance);
		Gui.StrokeText(buf, { Rect.x + Rect.z + 4.0f, Rect.y }, ImColor(0, 98, 98, 255), 14, false);
	}

	inline void DrawFovCircle(const CEntity& LocalEntity) noexcept
	{
		if (!ESPConfig::DrawFov) return;

		constexpr float DEG_TO_RAD = (float)M_PI / 180.0f;
		if (detail::sWindowSize.x == 0.f) detail::UpdateWindowCache();

		const ImVec2 center = detail::sWindowCenter;
		const float halfWidth = detail::sWindowSize.x * 0.5f;

		const float localFovTan = std::tanf(LocalEntity.Pawn.Fov * DEG_TO_RAD * 0.5f);
		const float aimFovTan = std::tanf(AimControl::AimFov * DEG_TO_RAD * 0.5f);
		const float minFovTan = std::tanf(AimControl::AimFovMin * DEG_TO_RAD * 0.5f);

		const float radius = (aimFovTan / (localFovTan > 1e-6f ? localFovTan : 1e-6f)) * halfWidth;
		Gui.Circle(center, radius, LegitBotConfig::FovCircleColor, 1.0f);

		if (AimControl::AimFovMin > 0.0f) {
			const float minRadius = (minFovTan / (localFovTan > 1e-6f ? localFovTan : 1e-6f)) * halfWidth;
			Gui.Circle(center, minRadius, LegitBotConfig::FovCircleColor, 1.0f);
		}
	}

	inline void DrawCrossHair(ImDrawList* drawList, const ImVec2& pos, ImColor color) noexcept
	{
		// Expect caller to pass foreground or background list already.
		// No trig, all precomputed scalars.
		constexpr float gap = 0.1f;
		constexpr float size = 6.0f;
		constexpr float thickness = 1.1f;

		const float left = pos.x - (gap + size);
		const float right = pos.x + (gap + 1.0f + size);
		const float top = pos.y - (gap + size);
		const float bottom = pos.y + (gap + 1.0f + size);

		const float xMin = pos.x - thickness + 1.0f;
		const float xMax = pos.x + thickness;
		const float yMin = pos.y - thickness + 1.0f;
		const float yMax = pos.y + thickness;

		drawList->AddRectFilled({ left, yMin }, { pos.x - gap, yMax }, color); // Left
		drawList->AddRectFilled({ pos.x + gap + 1, yMin }, { right, yMax }, color); // Right
		drawList->AddRectFilled({ xMin, top }, { xMax, pos.y - gap }, color); // Top
		drawList->AddRectFilled({ xMin, pos.y + gap + 1 }, { xMax, bottom }, color); // Bottom
	}

	inline void LineToEnemy(ImVec4 Rect, ImColor Color, float Thickness) noexcept
	{
		if (!ESPConfig::ShowLineToEnemy) return;
		if (detail::sWindowSize.x == 0.f) detail::UpdateWindowCache();

		const float centerX = detail::sWindowCenter.x;
		const float centerY = detail::sWindowCenter.y;

		const float startX = Rect.x + Rect.z * 0.5f;
		float startY = Rect.y;
		float endX = centerX, endY = 0.0f;

		switch (ESPConfig::LinePos) {
		case 1: endY = centerY; break;
		case 2: startY += Rect.w; endY = detail::sWindowSize.y; break;
		default: break;
		}

		Gui.Line({ startX, startY }, { endX, endY }, Color, Thickness);
	}

	inline void DrawFov(const CEntity& LocalEntity, float Size, ImColor Color, float Thickness) noexcept
	{
		if (!LegitBotConfig::ShowFovLine || MenuConfig::ShowMenu) return;
		if (detail::sWindowSize.x == 0.f) detail::UpdateWindowCache();

		constexpr float DEG_TO_RAD = (float)M_PI / 180.0f;
		const ImVec2 pos = detail::sWindowCenter;

		const float halfFovRad = (LocalEntity.Pawn.Fov * 0.5f) * DEG_TO_RAD;
		const float length = Size * std::tanf(halfFovRad);

		const Vec2 leftEnd = { pos.x - length, pos.y - Size };
		const Vec2 rightEnd = { pos.x + length, pos.y - Size };

		Gui.Line(pos, leftEnd, Color, Thickness);
		Gui.Line(pos, rightEnd, Color, Thickness);
	}

	inline void HeadShootLine(const CEntity& LocalEntity, ImColor Color) noexcept
	{
		if (!MiscCFG::ShowHeadShootLine || MenuConfig::ShowMenu) return;
		if (detail::sWindowSize.x == 0.f) detail::UpdateWindowCache();

		const float halfX = detail::sWindowCenter.x;
		const float halfY = detail::sWindowCenter.y;

		constexpr float DEG_TO_RAD = (float)M_PI / 180.0f;
		const float fovRad = LocalEntity.Pawn.Fov * DEG_TO_RAD;
		const float viewRad = LocalEntity.Pawn.ViewAngle.x * DEG_TO_RAD;

		const float fovSin = std::sinf(fovRad);
		const float viewSin = std::sinf(viewRad);

		// Pre-compute scale factor once (sin(90°) == 1)
		const float scaleFactor = detail::sWindowSize.y / (2.0f * (fovSin > 1e-6f ? fovSin : 1e-6f));

		Vec2 pos{ halfX, halfY - scaleFactor * viewSin };

		// Left shadow and body
		Gui.RectangleFilled({ pos.x - 21.0f, pos.y - 1.0f }, { 17.0f, 3.0f }, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled({ pos.x - 20.0f, pos.y + 0.0f }, { 17.0f, 3.0f }, Color);

		// Right shadow and body
		Gui.RectangleFilled({ pos.x + 5.0f,  pos.y - 1.0f }, { 17.0f, 3.0f }, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled({ pos.x + 6.0f,  pos.y + 0.0f }, { 17.0f, 3.0f }, Color);
	}

	inline ImVec4 Get2DBox(const CEntity& Entity) noexcept
	{
		const auto& bones = Entity.GetBone().BonePosList;
		if (bones.empty()) return { 0,0,0,0 };

		const BoneJointPos& head = bones[BONEINDEX::head];

		const float diffY = Entity.Pawn.ScreenPos.y - head.ScreenPos.y;
		const float sizeY = diffY * 1.09f;
		const float sizeX = sizeY * 0.6f;

		const float posX = Entity.Pawn.ScreenPos.x - sizeX * 0.5f;
		const float posY = head.ScreenPos.y - sizeY * 0.08f;

		return { posX, posY, sizeX, sizeY };
	}

	inline void DrawBone(const CEntity& Entity, ImColor Color, float Thickness) noexcept
	{
		if (!ESPConfig::ShowBoneESP) return;

		const auto& bonePosList = Entity.GetBone().BonePosList;
		if (bonePosList.empty()) return;

		BoneJointPos previous{}, current{};

		for (const auto& boneChain : BoneJointList::List)
		{
			previous.Pos = Vec3(0, 0, 0);
			for (const auto& index : boneChain)
			{
				current = bonePosList[index];
				if (previous.Pos == Vec3(0, 0, 0)) {
					previous = current;
					continue;
				}
				if (previous.IsVisible && current.IsVisible) {
					Gui.Line(previous.ScreenPos, current.ScreenPos, Color, Thickness);
				}
				previous = current;
			}
		}
	}

	inline void ShowLosLine(const CEntity& Entity, const float Length, ImColor Color, float Thickness) noexcept
	{
		if (!ESPConfig::ShowEyeRay) return;

		const auto& bonePosList = Entity.GetBone().BonePosList;
		if (bonePosList.empty()) return;

		const BoneJointPos& head = bonePosList[BONEINDEX::head];
		const Vec2 startPoint = head.ScreenPos;

		constexpr float DEG_TO_RAD = (float)M_PI / 180.0f;
		const float viewX = Entity.Pawn.ViewAngle.x * DEG_TO_RAD;
		const float viewY = Entity.Pawn.ViewAngle.y * DEG_TO_RAD;

		const float cosX = std::cosf(viewX);
		const float sinX = std::sinf(viewX);
		const float cosY = std::cosf(viewY);
		const float sinY = std::sinf(viewY);

		const float lineLength = cosX * Length;

		Vec3 temp{
			head.Pos.x + cosY * lineLength,
			head.Pos.y + sinY * lineLength,
			head.Pos.z - sinX * Length
		};

		Vec2 endPoint;
		if (!gGame.View.WorldToScreen(temp, endPoint)) return;

		Gui.Line(startPoint, endPoint, Color, Thickness);
	}

	inline ImVec4 Get2DBoneRect(const CEntity& Entity) noexcept
	{
		const auto& bones = Entity.GetBone().BonePosList;
		if (bones.empty()) return { 0,0,0,0 };

		bool anyVisible = false;
		Vec2 minPos = bones[0].ScreenPos;
		Vec2 maxPos = bones[0].ScreenPos;

		for (const auto& j : bones)
		{
			if (!j.IsVisible) continue;
			anyVisible = true;
			minPos.x = (std::min)(j.ScreenPos.x, minPos.x);
			minPos.y = (std::min)(j.ScreenPos.y, minPos.y);
			maxPos.x = (std::max)(j.ScreenPos.x, maxPos.x);
			maxPos.y = (std::max)(j.ScreenPos.y, maxPos.y);
		}

		if (!anyVisible) return { 0,0,0,0 };

		const Vec2 size{ maxPos.x - minPos.x, maxPos.y - minPos.y };
		return { minPos.x, minPos.y, size.x, size.y };
	}

	//--------------------------------------------------------------------------------------
	// Health / Armor / Ammo Bars
	//--------------------------------------------------------------------------------------
	class HealthBar
	{
	private:
		using TimePoint_ = std::chrono::steady_clock::time_point;
		static constexpr int ShowBackUpHealthDuration = 500;

		float   MaxHealth = 0.f;
		float   CurrentHealth = 0.f;
		float   LastestBackupHealth = 0.f; // reserved (not used in current draw path)
		ImVec2  RectPos{};
		ImVec2  RectSize{};
		bool    InShowBackupHealth = false; // reserved
		TimePoint_ BackupHealthTimePoint{}; // reserved
		int     MaxAmmo = 0;
		int     CurrentAmmo = 0;
		int     MaxArmor = 0;
		int     CurrentArmor = 0;

		// Colors
		ImColor FirstStageColor = ImColor(0, 255, 0, 255);
		ImColor SecondStageColor = ImColor(255, 232, 0, 255);
		ImColor ThirdStageColor = ImColor(255, 39, 0, 255);
		ImColor BackupHealthColor = ImColor(255, 255, 255, 220);
		ImColor FrameColor = ImColor(45, 45, 45, 220);
		ImColor BackGroundColor = ImColor(0, 0, 0, 255);
		ImColor AmmoColor = ImColor(255, 255, 0, 255);
		ImColor ArmorColor = ImColor(0, 128, 255, 255);
		ImColor ArmorWithHelmetColor = ImColor(255, 0, 255, 255);

	public:
		HealthBar() = default;

		void HealthBarV(float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size, bool ShowNum) noexcept
		{
			ImDrawList* dl = detail::GetDrawList();

			this->MaxHealth = MaxHealth;
			this->CurrentHealth = CurrentHealth;
			this->RectPos = Pos;
			this->RectSize = Size;

			float proportion = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
			if (proportion < 0.f) proportion = 0.f; else if (proportion > 1.f) proportion = 1.f;

			const float height = RectSize.y * proportion;
			const ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

			dl->AddRectFilled(RectPos, rectBR, BackGroundColor, 5.0f, ImDrawFlags_RoundCornersAll);

			const float t = std::pow(proportion, 2.5f);
			const ImColor color = (proportion > 0.5f)
				? Mix(FirstStageColor, SecondStageColor, t * 3.f - 1.f)
				: Mix(SecondStageColor, ThirdStageColor, t * 4.f);

			const ImVec2 healthTL = { RectPos.x, RectPos.y + RectSize.y - height };
			dl->AddRectFilled(healthTL, rectBR, color, 0.0f);

			dl->AddRect(RectPos, rectBR, FrameColor, 0.0f, ImDrawFlags_RoundCornersAll, 1.0f);

			if (ShowNum && CurrentHealth < MaxHealth) {
				char buf[16];
				_snprintf_s(buf, _TRUNCATE, "%.0f", CurrentHealth);
				Gui.StrokeText(buf, healthTL, ImColor(255, 255, 255), 13.0f, true);
			}
		}

		void ArmorBarV(bool HasHelmet, float MaxArmor, float CurrentArmor, const ImVec2& Pos, const ImVec2& Size, bool ShowNum) noexcept
		{
			ImDrawList* dl = detail::GetDrawList();

			this->MaxArmor = static_cast<int>(MaxArmor);
			this->CurrentArmor = static_cast<int>(CurrentArmor);
			this->RectPos = Pos;
			this->RectSize = Size;

			float proportion = (MaxArmor > 0.f) ? (CurrentArmor / MaxArmor) : 0.f;
			if (proportion < 0.f) proportion = 0.f; else if (proportion > 1.f) proportion = 1.f;

			const float height = RectSize.y * proportion;
			const ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

			dl->AddRectFilled(RectPos, rectBR, BackGroundColor, 5.0f, ImDrawFlags_RoundCornersAll);

			const ImColor color = HasHelmet ? ArmorWithHelmetColor : ArmorColor;
			const ImVec2 armorTL = { RectPos.x, RectPos.y + RectSize.y - height };
			dl->AddRectFilled(armorTL, rectBR, color, 0.0f);

			dl->AddRect(RectPos, rectBR, FrameColor, 0.0f, ImDrawFlags_RoundCornersAll, 1.0f);

			if (ShowNum && CurrentArmor < MaxArmor) {
				char buf[16];
				_snprintf_s(buf, _TRUNCATE, "%.0f", CurrentArmor);
				Gui.StrokeText(buf, armorTL, ImColor(255, 255, 255), 13.0f, true);
			}
		}

		void AmmoBarH(float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size) noexcept
		{
			ImDrawList* dl = detail::GetDrawList();

			this->MaxAmmo = static_cast<int>(MaxAmmo);
			this->CurrentAmmo = static_cast<int>(CurrentAmmo);
			this->RectPos = Pos;
			this->RectSize = Size;

			float proportion = (MaxAmmo > 0.f) ? (CurrentAmmo / MaxAmmo) : 0.f;
			if (proportion < 0.f) proportion = 0.f; else if (proportion > 1.f) proportion = 1.f;

			const float width = RectSize.x * proportion;
			const ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

			dl->AddRectFilled(RectPos, rectBR, BackGroundColor, 5.0f, ImDrawFlags_RoundCornersAll);

			const ImVec2 ammoBR = { RectPos.x + width, RectPos.y + RectSize.y };
			dl->AddRectFilled(RectPos, ammoBR, AmmoColor, 0.0f);

			dl->AddRect(RectPos, rectBR, FrameColor, 0.0f, ImDrawFlags_RoundCornersAll, 1.0f);
		}
	};

	//--------------------------------------------------------------------------------------
	// Bar API (kept identical)
	//--------------------------------------------------------------------------------------
	inline void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size) noexcept
	{
		static std::unordered_map<DWORD, HealthBar> sHealthBars;
		static bool reserved = false;
		if (!reserved) { sHealthBars.reserve(64); reserved = true; } // typical players count

		HealthBar& hb = sHealthBars[Sign];
		hb.HealthBarV(MaxHealth, CurrentHealth, Pos, Size, ESPConfig::ShowHealthNum);
	}

	inline void DrawAmmoBar(DWORD Sign, float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size) noexcept
	{
		static std::unordered_map<DWORD, HealthBar> sAmmoBars;
		static bool reserved = false;
		if (!reserved) { sAmmoBars.reserve(64); reserved = true; }

		HealthBar& hb = sAmmoBars[Sign];
		hb.AmmoBarH(MaxAmmo, CurrentAmmo, Pos, Size);
	}

	inline void DrawArmorBar(DWORD Sign, float MaxArmor, float CurrentArmor, bool HasHelmet, const ImVec2& Pos, const ImVec2& Size) noexcept
	{
		static std::unordered_map<DWORD, HealthBar> sArmorBars;
		static bool reserved = false;
		if (!reserved) { sArmorBars.reserve(64); reserved = true; }

		HealthBar& hb = sArmorBars[Sign];
		hb.ArmorBarV(HasHelmet, MaxArmor, CurrentArmor, Pos, Size, ESPConfig::ShowArmorNum);
	}
}

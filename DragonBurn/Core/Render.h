#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <map>
#include <Windows.h>
#include <unordered_map>
#include "../Game/Entity.h"
#include "../Helpers/Format.h"
#include "../OS-ImGui/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../OS-ImGui/imgui/imgui_internal.h"
#include "../Features/TriggerBot.h"
#include "../Features/Aimbot.h"
#undef max()
#undef min()

namespace Render
{
	// Drawing functions
	void DrawHeadCircle(const CEntity& Entity, ImColor Color);
	void DrawHealth(int Health, const ImVec2& Pos);
	void DrawDistance(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect);
	void DrawFovCircle(const CEntity& LocalEntity);
	void DrawCrossHair(ImDrawList* drawList, const ImVec2& pos, ImColor color) noexcept;
	void LineToEnemy(ImVec4 Rect, ImColor Color, float Thickness);
	void DrawFov(const CEntity& LocalEntity, float Size, ImColor Color, float Thickness);
	void HeadShootLine(const CEntity& LocalEntity, ImColor Color);
	void DrawBone(const CEntity& Entity, ImColor Color, float Thickness);
	void ShowLosLine(const CEntity& Entity, const float Length, ImColor Color, float Thickness);

	// Utility functions
	ImVec4 Get2DBox(const CEntity& Entity);
	ImVec4 Get2DBoneRect(const CEntity& Entity);
	ImVec2 GetScreenCenterImVec2();
	ImVec4 rgba2ImVec(int r, int g, int b, int a);

	// Bar drawing functions
	void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size);
	void DrawAmmoBar(DWORD Sign, float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size);
	void DrawArmorBar(DWORD Sign, float MaxArmor, float CurrentArmor, bool HasHelmet, const ImVec2& Pos, const ImVec2& Size);

	class HealthBar
	{
	private:
		using TimePoint_ = std::chrono::steady_clock::time_point;
		const int ShowBackUpHealthDuration = 500;
		float MaxHealth = 0.f;
		float CurrentHealth = 0.f;
		float LastestBackupHealth = 0.f;
		ImVec2 RectPos{};
		ImVec2 RectSize{};
		bool InShowBackupHealth = false;
		TimePoint_ BackupHealthTimePoint{};
		int MaxAmmo = 0;
		int CurrentAmmo = 0;
		int MaxArmor = 0;
		int CurrentArmor = 0;

		// Color mixing function
		ImColor Mix(const ImColor& Col_1, const ImColor& Col_2, float t);

		// Color constants
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

		void HealthBarV(float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size, bool ShowNum);
		void ArmorBarV(bool HasHelmet, float MaxArmor, float CurrentArmor, const ImVec2& Pos, const ImVec2& Size, bool ShowNum);
		void AmmoBarH(float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size);
	};
}
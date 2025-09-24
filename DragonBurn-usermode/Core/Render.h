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

namespace Render
{

	inline void DrawHeadCircle(const CEntity& Entity, ImColor Color)
	{
		if (!ESPConfig::ShowHeadBox)
			return;

		const auto& BoneList = Entity.GetBone().BonePosList;
		if (BoneList.empty()) return;

		const BoneJointPos& Head = BoneList[BONEINDEX::head];
		const BoneJointPos& Neck = BoneList[BONEINDEX::neck_0];

		Gui.Circle(Head.ScreenPos, std::abs(Head.ScreenPos.y - Neck.ScreenPos.y) + 2.0f, Color, 1.2f);
	}


	inline void DrawHealth(int Health, const ImVec2& Pos)
	{
		if (!ESPConfig::ShowHealthNum)
			return;

		char health_str[4];
		snprintf(health_str, sizeof(health_str), "%d", Health);

		Gui.StrokeText(health_str, Pos, ImColor(0, 220, 0, 255), 12, false);
	}


	inline void DrawDistance(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect)
	{
		if (!ESPConfig::ShowDistance)
			return;

		int distance = static_cast<int>(Entity.Pawn.Pos.DistanceTo(LocalEntity.Pawn.Pos)) / 100;
		std::string dis_str = std::to_string(distance) + "m";

		Gui.StrokeText(dis_str, { Rect.x + Rect.z + 4, Rect.y }, ImColor(0, 98, 98, 255), 14, false);

	}

	inline void DrawFovCircle(ImDrawList* drawList, const CEntity& LocalEntity) noexcept
	{
		if (!ESPConfig::DrawFov)
			return;

		constexpr float DEG_TO_RAD = M_PI / 180.f;
		constexpr float STATIC_FOV = 90.0f;
		
		ImVec2 center = ImVec2(Gui.Window.Size.x / 2.0f, Gui.Window.Size.y / 2.0f);
		float halfWindowSize = Gui.Window.Size.x / 2.0f;

		float staticFovTan = tan(STATIC_FOV * DEG_TO_RAD / 2.0f);
		float aimFovTan = tan(AimControl::AimFov * DEG_TO_RAD / 2.0f);

		float radius = (aimFovTan / staticFovTan) * halfWindowSize;

		drawList->AddCircle(center, radius, LegitBotConfig::FovCircleColor, 0, 1.5f);

		if (AimControl::AimFovMin > 0)
		{
			float aimFovMinTan = tan(AimControl::AimFovMin * DEG_TO_RAD / 2.0f);
			float minRadius = (aimFovMinTan / staticFovTan) * halfWindowSize;
			drawList->AddCircle(center, minRadius, LegitBotConfig::FovCircleColor, 0, 1.5f);
		}
	}

	inline void DrawCrossHair(ImDrawList* drawList, const ImVec2& pos, ImColor color) noexcept
	{
		constexpr float gap = 0.1f;
		constexpr int size = 6;
		constexpr float thickness = 1.1f;

		float left = pos.x - (gap + size);
		float right = pos.x + (gap + 1 + size);
		float top = pos.y - (gap + size);
		float bottom = pos.y + (gap + 1 + size);

		float xMin = pos.x - thickness + 1;
		float xMax = pos.x + thickness;
		float yMin = pos.y - thickness + 1;
		float yMax = pos.y + thickness;

		drawList->AddRectFilled(ImVec2(left, yMin), ImVec2(pos.x - gap, yMax), color);  // Left
		drawList->AddRectFilled(ImVec2(pos.x + gap + 1, yMin), ImVec2(right, yMax), color); // Right
		drawList->AddRectFilled(ImVec2(xMin, top), ImVec2(xMax, pos.y - gap), color);  // Top
		drawList->AddRectFilled(ImVec2(xMin, pos.y + gap + 1), ImVec2(xMax, bottom), color); // Bottom
	}

	inline void LineToEnemy(ImVec4 Rect, ImColor Color, float Thickness)
	{
		if (!ESPConfig::ShowLineToEnemy)
			return;

		const float CenterX = Gui.Window.Size.x * 0.5f;
		const float CenterY = Gui.Window.Size.y * 0.5f;
		const float StartX = Rect.x + Rect.z * 0.5f;
		float StartY = Rect.y;
		float EndX = CenterX, EndY = 0;

		switch (ESPConfig::LinePos)
		{
		case 1: EndY = CenterY; break;
		case 2: StartY += Rect.w; EndY = Gui.Window.Size.y; break;
		default: break;
		}

		Gui.Line({ StartX, StartY }, { EndX, EndY }, Color, Thickness);
	}

	inline void DrawFov(const CEntity& LocalEntity, float Size, ImColor Color, float Thickness)
	{
		if (!LegitBotConfig::ShowFovLine || MenuConfig::ShowMenu)
			return;

		constexpr float DEG_TO_RAD = M_PI / 180.0f;
		const Vec2 Pos = Gui.Window.Size * 0.5f;
		const float radian = (LocalEntity.Pawn.Fov * 0.5f) * DEG_TO_RAD;
		const float Length = Size * tan(radian);

		std::array<Vec2, 2> LineEndPoints = {
			Vec2(Pos.x - Length, Pos.y - Size),
			Vec2(Pos.x + Length, Pos.y - Size)
		};

		Gui.Line(Pos, LineEndPoints[0], Color, 1.5f);
		Gui.Line(Pos, LineEndPoints[1], Color, 1.5f);
	}

	inline void HeadShootLine(const CEntity& LocalEntity, ImColor Color)
	{
		if (!MiscCFG::ShowHeadShootLine || MenuConfig::ShowMenu)
			return;

		// Pre-compute half dimensions
		const float halfWindowX = Gui.Window.Size.x * 0.5f;
		const float halfWindowY = Gui.Window.Size.y * 0.5f;

		// Convert angles to radians
		const float fovRadians = LocalEntity.Pawn.Fov * (M_PI / 180.0f);
		const float viewAngleXRadians = LocalEntity.Pawn.ViewAngle.x * (M_PI / 180.0f);

		// Compute sine values
		const float fovSin = std::sin(fovRadians);
		const float viewSin = std::sin(viewAngleXRadians);

		// Pre-compute scale factor once (note: sin(90°) is 1, so it is removed)
		const float scaleFactor = Gui.Window.Size.y / (2.0f * fovSin);

		Vec2 Pos;
		Pos.x = halfWindowX;
		Pos.y = halfWindowY - scaleFactor * viewSin;

		// Left rectangles
		Gui.RectangleFilled(Vec2{ Pos.x - 21, Pos.y - 1 }, Vec2{ 17, 3 }, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled(Vec2{ Pos.x - 20, Pos.y }, Vec2{ 17, 3 }, Color);

		// Right rectangles
		Gui.RectangleFilled(Vec2{ Pos.x + 5, Pos.y - 1 }, Vec2{ 17, 3 }, Color & IM_COL32_A_MASK);
		Gui.RectangleFilled(Vec2{ Pos.x + 6, Pos.y }, Vec2{ 17, 3 }, Color);
	}

	inline ImVec4 Get2DBox(const CEntity& Entity)
	{
		const auto& bonePosList = Entity.GetBone().BonePosList;
		if (bonePosList.empty())
			return ImVec4(0, 0, 0, 0);

		Vec2 minPos = bonePosList[0].ScreenPos;
		Vec2 maxPos = bonePosList[0].ScreenPos;

		for (const auto& boneJoint : bonePosList)
		{
			if (!boneJoint.IsVisible)
				continue;
			minPos.x = min(boneJoint.ScreenPos.x, minPos.x);
			minPos.y = min(boneJoint.ScreenPos.y, minPos.y);
			maxPos.x = max(boneJoint.ScreenPos.x, maxPos.x);
			maxPos.y = max(boneJoint.ScreenPos.y, maxPos.y);
		}

		BoneJointPos headBone = Entity.GetBone().BonePosList[BONEINDEX::head];
		const float diffY = maxPos.y - headBone.ScreenPos.y;
		const float height = diffY * 1.09f;
		const float width = height * 0.6f;

		const float posX = headBone.ScreenPos.x - width * 0.5f;
		const float posY = headBone.ScreenPos.y - height * 0.08f;

		minPos.x = min(minPos.x, posX);
		minPos.y = min(minPos.y, posY);
		maxPos.x = max(maxPos.x, posX + width);
		maxPos.y = max(maxPos.y, posY + height);

		const Vec2 size{ maxPos.x - minPos.x, maxPos.y - minPos.y };
		return ImVec4(minPos.x, minPos.y, size.x, size.y);
	}

	inline void DrawBone(const CEntity& Entity, ImColor Color, float Thickness)
	{
		if (!ESPConfig::ShowBoneESP)
			return;

		const auto& bonePosList = Entity.GetBone().BonePosList;
		BoneJointPos previous, current;

		for (const auto& boneChain : BoneJointList::List)
		{
			previous.Pos = Vec3(0, 0, 0);
			for (const auto& index : boneChain)
			{
				current = bonePosList[index];
				if (previous.Pos == Vec3(0, 0, 0))
				{
					previous = current;
					continue;
				}
				if (previous.IsVisible && current.IsVisible)
				{
					Gui.Line(previous.ScreenPos, current.ScreenPos, Color, Thickness);
				}
				previous = current;
			}
		}
	}

	inline void ShowLosLine(const CEntity& Entity, const float Length, ImColor Color, float Thickness)
	{
		if (!ESPConfig::ShowEyeRay)
			return;

		const auto& bonePosList = Entity.GetBone().BonePosList;
		const BoneJointPos& head = bonePosList[BONEINDEX::head];
		const Vec2 startPoint = head.ScreenPos;

		const float degToRad = M_PI / 180.0f;
		const float viewAngleX = Entity.Pawn.ViewAngle.x * degToRad;
		const float viewAngleY = Entity.Pawn.ViewAngle.y * degToRad;

		const float lineLength = cos(viewAngleX) * Length;

		Vec3 temp;
		temp.x = head.Pos.x + cos(viewAngleY) * lineLength;
		temp.y = head.Pos.y + sin(viewAngleY) * lineLength;
		temp.z = head.Pos.z - sin(viewAngleX) * Length;

		Vec2 endPoint;
		if (!gGame.View.WorldToScreen(temp, endPoint))
			return;

		Gui.Line(startPoint, endPoint, Color, Thickness);
	}

	inline ImVec4 Get2DBoneRect(const CEntity& Entity)
	{
		const auto& bonePosList = Entity.GetBone().BonePosList;
		if (bonePosList.empty())
			return ImVec4(0, 0, 0, 0);

		Vec2 minPos = bonePosList[0].ScreenPos;
		Vec2 maxPos = bonePosList[0].ScreenPos;

		for (const auto& boneJoint : bonePosList)
		{
			if (!boneJoint.IsVisible)
				continue;
			minPos.x = min(boneJoint.ScreenPos.x, minPos.x);
			minPos.y = min(boneJoint.ScreenPos.y, minPos.y);
			maxPos.x = max(boneJoint.ScreenPos.x, maxPos.x);
			maxPos.y = max(boneJoint.ScreenPos.y, maxPos.y);
		}

		const Vec2 size{ maxPos.x - minPos.x, maxPos.y - minPos.y };
		return ImVec4(minPos.x, minPos.y, size.x, size.y);
	}


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

	public:
		HealthBar() {}

		void HealthBarV(float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size, bool ShowNum);
		void ArmorBarV(bool HasHelmet, float MaxArmor, float CurrentArmor, const ImVec2& Pos, const ImVec2& Size, bool ShowNum);
		void AmmoBarH(float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size);

	private:
		inline ImColor Mix(const ImColor& Col_1, const ImColor& Col_2, float t)
		{
			ImColor col;
			col.Value.x = t * Col_1.Value.x + (1.0f - t) * Col_2.Value.x;
			col.Value.y = t * Col_1.Value.y + (1.0f - t) * Col_2.Value.y;
			col.Value.z = t * Col_1.Value.z + (1.0f - t) * Col_2.Value.z;
			col.Value.w = Col_1.Value.w;
			return col;
		}

		ImColor FirstStageColor = ImColor(0, 255, 0, 255);
		ImColor SecondStageColor = ImColor(255, 232, 0, 255);
		ImColor ThirdStageColor = ImColor(255, 39, 0, 255);
		ImColor BackupHealthColor = ImColor(255, 255, 255, 220);
		ImColor FrameColor = ImColor(45, 45, 45, 220);
		ImColor BackGroundColor = ImColor(0, 0, 0, 255);
		ImColor AmmoColor = ImColor(255, 255, 0, 255);
		ImColor ArmorColor = ImColor(0, 128, 255, 255);
		ImColor ArmorWithHelmetColor = ImColor(255, 0, 255, 255);
	};

	//////////////////////////////////////////////////////////////////////////
	// Vertical Health Bar
	//////////////////////////////////////////////////////////////////////////
	void HealthBar::HealthBarV(float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size, bool ShowNum)
	{
		ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

		this->MaxHealth = MaxHealth;
		this->CurrentHealth = CurrentHealth;
		this->RectPos = Pos;
		this->RectSize = Size;

		float proportion = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;
		proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

		float height = RectSize.y * proportion;
		ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

		DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

		float colorLerpT = powf(proportion, 2.5f);
		ImColor color = (proportion > 0.5f && proportion <= 1.f) ?
			Mix(FirstStageColor, SecondStageColor, colorLerpT * 3.f - 1.f) :
			Mix(SecondStageColor, ThirdStageColor, colorLerpT * 4.f);

		ImVec2 healthRectTL = { RectPos.x, RectPos.y + RectSize.y - height };
		DrawList->AddRectFilled(healthRectTL, rectBR, color, 0);

		DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);

		if (ShowNum && CurrentHealth < MaxHealth)
		{
			char healthStr[16];
			snprintf(healthStr, sizeof(healthStr), "%.f", CurrentHealth);
			Gui.StrokeText(healthStr, healthRectTL, ImColor(255, 255, 255), 13.f, true);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Vertical Armor Bar
	//////////////////////////////////////////////////////////////////////////
	void HealthBar::ArmorBarV(bool HasHelmet, float MaxArmor, float CurrentArmor, const ImVec2& Pos, const ImVec2& Size, bool ShowNum)
	{
		ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

		this->MaxArmor = MaxArmor;
		this->CurrentArmor = CurrentArmor;
		this->RectPos = Pos;
		this->RectSize = Size;

		float proportion = (MaxArmor > 0.f) ? CurrentArmor / MaxArmor : 0.f;
		proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

		float height = RectSize.y * proportion;
		ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

		DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

		ImColor color = HasHelmet ? ArmorWithHelmetColor : ArmorColor;
		ImVec2 armorRectTL = { RectPos.x, RectPos.y + RectSize.y - height };
		DrawList->AddRectFilled(armorRectTL, rectBR, color, 0);

		DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);

		if (ShowNum && CurrentArmor < MaxArmor)
		{
			char armorStr[16];
			snprintf(armorStr, sizeof(armorStr), "%.f", CurrentArmor);
			Gui.StrokeText(armorStr, armorRectTL, ImColor(255, 255, 255), 13.f, true);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Horizontal Ammo Bar
	//////////////////////////////////////////////////////////////////////////
	void HealthBar::AmmoBarH(float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size)
	{
		ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

		this->MaxAmmo = MaxAmmo;
		this->CurrentAmmo = CurrentAmmo;
		this->RectPos = Pos;
		this->RectSize = Size;

		float proportion = (MaxAmmo > 0) ? CurrentAmmo / static_cast<float>(MaxAmmo) : 0.f;
		proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

		float width = RectSize.x * proportion;
		ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

		DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

		ImVec2 ammoRectBR = { RectPos.x + width, RectPos.y + RectSize.y };
		DrawList->AddRectFilled(RectPos, ammoRectBR, AmmoColor, 0);

		DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);
	}

	void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size)
	{
		static std::unordered_map<DWORD, HealthBar> HealthBarMap;
		HealthBar& hb = HealthBarMap[Sign];
		hb.HealthBarV(MaxHealth, CurrentHealth, Pos, Size, ESPConfig::ShowHealthNum);
	}

	void DrawAmmoBar(DWORD Sign, float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size)
	{
		static std::unordered_map<DWORD, HealthBar> HealthBarMap;
		HealthBar& hb = HealthBarMap[Sign];
		hb.AmmoBarH(MaxAmmo, CurrentAmmo, Pos, Size);
	}
	void DrawArmorBar(DWORD Sign, float MaxArmor, float CurrentArmor, bool HasHelmet, const ImVec2& Pos, const ImVec2& Size)
	{
		static std::unordered_map<DWORD, HealthBar> HealthBarMap;
		HealthBar& hb = HealthBarMap[Sign];
		hb.ArmorBarV(HasHelmet, MaxArmor, CurrentArmor, Pos, Size, ESPConfig::ShowArmorNum);
	}

	ImVec2 GetScreenCenterImVec2()
	{
		int W = GetSystemMetrics(SM_CXSCREEN);
		int H = GetSystemMetrics(SM_CYSCREEN);
		return ImVec2(W / 2.0f, H / 2.0f);
	}

	inline ImVec4 rgba2ImVec(int r, int g, int b, int a)
	{
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
}
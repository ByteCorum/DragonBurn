#pragma once
#include "..\Core\Render.h"
#include "..\Core\Config.h"
#include "..\Core\Cheats.h"
#include "..\Helpers\GetWeaponIcon.h"

namespace ESP
{
	struct WeaponIconSize
	{
		float width;
		float height;
		float offsetX;
		float offsetY;
	};

	// Use constexpr for compile-time initialization and better performance
	static constexpr WeaponIconSize DEFAULT_ICON_SIZE = { 13.0f, 13.0f, 0.0f, 0.0f };
	static constexpr WeaponIconSize KNIFE_SIZE = { 13.0f, 13.0f, -5.0f, 0.0f };
	static constexpr WeaponIconSize RIFLE_SIZE = { 13.0f, 13.0f, -10.0f, 0.0f };
	static constexpr WeaponIconSize SMG_SIZE = { 13.0f, 13.0f, -7.0f, 0.0f };
	static constexpr WeaponIconSize GRENADE_SIZE = { 13.0f, 13.0f, 3.0f, 0.0f };
	static constexpr WeaponIconSize REVOLVER_SIZE = { 13.0f, 13.0f, -3.0f, 0.0f };

	// Optimized weapon icon size lookup using string_view and unordered_map with reserve
	static std::unordered_map<std::string_view, WeaponIconSize> weaponIconSizes = []()
		{
			std::unordered_map<std::string_view, WeaponIconSize> map;
			map.reserve(44); // Reserve space to avoid rehashing

			// Knives
			map.emplace("t_knife", KNIFE_SIZE);
			map.emplace("ct_knife", KNIFE_SIZE);

			// Pistols (most use default)
			map.emplace("deagle", KNIFE_SIZE);
			map.emplace("elite", DEFAULT_ICON_SIZE);
			map.emplace("fiveseven", DEFAULT_ICON_SIZE);
			map.emplace("glock", DEFAULT_ICON_SIZE);
			map.emplace("revolver", REVOLVER_SIZE);
			map.emplace("hkp2000", DEFAULT_ICON_SIZE);
			map.emplace("p250", DEFAULT_ICON_SIZE);
			map.emplace("usp_silencer", DEFAULT_ICON_SIZE);
			map.emplace("tec9", DEFAULT_ICON_SIZE);
			map.emplace("cz75a", DEFAULT_ICON_SIZE);

			// SMGs
			map.emplace("mac10", DEFAULT_ICON_SIZE);
			map.emplace("ump45", SMG_SIZE);
			map.emplace("bizon", SMG_SIZE);
			map.emplace("mp7", REVOLVER_SIZE);
			map.emplace("mp9", SMG_SIZE);
			map.emplace("p90", SMG_SIZE);

			// Rifles
			map.emplace("galilar", RIFLE_SIZE);
			map.emplace("famas", RIFLE_SIZE);
			map.emplace("m4a1_silencer", RIFLE_SIZE);
			map.emplace("m4a1", RIFLE_SIZE);
			map.emplace("aug", SMG_SIZE);
			map.emplace("sg556", RIFLE_SIZE);
			map.emplace("ak47", RIFLE_SIZE);

			// Snipers
			map.emplace("g3sg1", RIFLE_SIZE);
			map.emplace("scar20", RIFLE_SIZE);
			map.emplace("awp", RIFLE_SIZE);
			map.emplace("ssg08", RIFLE_SIZE);

			// Shotguns
			map.emplace("xm1014", RIFLE_SIZE);
			map.emplace("sawedoff", RIFLE_SIZE);
			map.emplace("mag7", RIFLE_SIZE);
			map.emplace("nova", RIFLE_SIZE);

			// Machine guns
			map.emplace("negev", RIFLE_SIZE);
			map.emplace("m249", RIFLE_SIZE);

			// Equipment
			map.emplace("taser", DEFAULT_ICON_SIZE);
			map.emplace("flashbang", GRENADE_SIZE);
			map.emplace("hegrenade", GRENADE_SIZE);
			map.emplace("smokegrenade", GRENADE_SIZE);
			map.emplace("molotov", GRENADE_SIZE);
			map.emplace("decoy", GRENADE_SIZE);
			map.emplace("incgrenade", GRENADE_SIZE);
			map.emplace("c4", DEFAULT_ICON_SIZE);

			return map;
		}();

	// Cache for frequently accessed values
	struct RenderCache
	{
		ImFont* font0 = nullptr;
		ImFont* font1 = nullptr;
		ImDrawList* drawList = nullptr;
		bool fontsValid = false;

		void UpdateFonts()
		{
			if (!fontsValid)
			{
				const auto& fonts = ImGui::GetIO().Fonts->Fonts;
				if (fonts.Size >= 2)
				{
					font0 = fonts[0];
					font1 = fonts[1];
					fontsValid = true;
				}
			}
			drawList = ImGui::GetBackgroundDrawList();
		}
	};
	static RenderCache s_renderCache;

	// Optimized weapon icon size getter with fallback
	inline const WeaponIconSize& GetWeaponIconSize(std::string_view weaponName) noexcept
	{
		const auto it = weaponIconSizes.find(weaponName);
		return (it != weaponIconSizes.end()) ? it->second : DEFAULT_ICON_SIZE;
	}

	constexpr ImVec4 GetBoxRect(const CEntity& Entity, int BoxType) noexcept
	{
		// Both cases do the same thing, so simplify
		return Render::Get2DBox(Entity);
	}

	// Optimized text outline drawing
	inline void DrawTextWithOutline(ImDrawList* drawList, ImFont* font, float fontSize,
		const ImVec2& pos, ImU32 textColor, ImU32 outlineColor, const char* text) noexcept
	{
		// Single loop for all outline positions
		constexpr ImVec2 outlineOffsets[] = { {-1, -1}, {-1, 1}, {1, 1}, {1, -1} };

		for (const auto& offset : outlineOffsets)
		{
			const ImVec2 outlinePos = { pos.x + offset.x, pos.y + offset.y };
			drawList->AddText(font, fontSize, outlinePos, outlineColor, text);
		}

		drawList->AddText(font, fontSize, pos, textColor, text);
	}

	// Optimized corner box drawing
	inline void DrawCornerBox(const ImVec4& rect, ImU32 color, float thickness) noexcept
	{
		const float quarterWidth = rect.z * 0.25f;
		const float quarterHeight = rect.w * 0.25f;

		// Precompute corner positions
		const ImVec2 corners[4] = {
			{rect.x, rect.y},                          // Top-left
			{rect.x + rect.z, rect.y},                 // Top-right  
			{rect.x, rect.y + rect.w},                 // Bottom-left
			{rect.x + rect.z, rect.y + rect.w}        // Bottom-right
		};

		// Precompute all line segments
		struct LineSegment { ImVec2 p1, p2; };
		const LineSegment segments[8] = {
			// Top-left corner
			{corners[0], {corners[0].x + quarterWidth, corners[0].y}},
			{corners[0], {corners[0].x, corners[0].y + quarterHeight}},
			// Top-right corner  
			{corners[1], {corners[1].x - quarterWidth, corners[1].y}},
			{corners[1], {corners[1].x, corners[1].y + quarterHeight}},
			// Bottom-left corner
			{corners[2], {corners[2].x + quarterWidth, corners[2].y}},
			{corners[2], {corners[2].x, corners[2].y - quarterHeight}},
			// Bottom-right corner
			{corners[3], {corners[3].x - quarterWidth, corners[3].y}},
			{corners[3], {corners[3].x, corners[3].y - quarterHeight}}
		};

		// Draw all segments in one loop
		for (const auto& seg : segments)
		{
			Gui.Line(seg.p1, seg.p2, color, thickness);
		}
	}

	void RenderPlayerESP(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect,
		int LocalPlayerControllerIndex, int Index)
	{
		s_renderCache.UpdateFonts();

		// Cache frequently used values
		const DWORD64 localPlayerMask = DWORD64(1) << LocalPlayerControllerIndex;
		const DWORD64 indexMask = DWORD64(1) << Index;
		const bool bIsVisible = (Entity.Pawn.bSpottedByMask & localPlayerMask) ||
			(LocalEntity.Pawn.bSpottedByMask & localPlayerMask);
		const bool bIsVisibleIndex = (Entity.Pawn.bSpottedByMask & localPlayerMask) ||
			(LocalEntity.Pawn.bSpottedByMask & indexMask);

		// Early exit optimizations - check cheapest conditions first
		const bool shouldRenderBones = ESPConfig::ShowBoneESP;
		const bool shouldRenderLOS = ESPConfig::ShowEyeRay;
		const bool shouldRenderHeadCircle = ESPConfig::ShowHeadBox;

		// Batch render expensive operations
		if (shouldRenderBones || shouldRenderLOS || shouldRenderHeadCircle)
		{
			if (shouldRenderBones)
				Render::DrawBone(Entity, ESPConfig::BoneColor, 1.3f);
			if (shouldRenderLOS)
				Render::ShowLosLine(Entity, 50.0f, ESPConfig::EyeRayColor, 1.3f);
			if (shouldRenderHeadCircle)
				Render::DrawHeadCircle(Entity, ESPConfig::HeadBoxColor);
		}

		// Optimized filled box rendering
		if (ESPConfig::FilledBox)
		{
			const float rounding = ESPConfig::BoxRounding;
			const ImVec2 rectPos = { Rect.x, Rect.y };
			const ImVec2 rectSize = { Rect.z, Rect.w };

			if (ESPConfig::FilledVisBox)
			{
				const ImColor color = bIsVisible ? ESPConfig::BoxFilledVisColor : ESPConfig::FilledColor;
				Gui.RectangleFilled(rectPos, rectSize, color, rounding);
			}
			else if (ESPConfig::MultiColor)
			{
				Gui.RectangleFilledGraident(rectPos, rectSize, ESPConfig::BoxColor,
					ESPConfig::FilledColor, ESPConfig::FilledColor2, rounding);
			}
			else
			{
				Gui.RectangleFilled(rectPos, rectSize, ESPConfig::FilledColor, rounding);
			}
		}

		// Optimized box ESP rendering
		if (ESPConfig::ShowBoxESP)
		{
			const ImVec2 rectPos = { Rect.x, Rect.y };
			const ImVec2 rectSize = { Rect.z, Rect.w };
			const ImU32 boxColor = (bIsVisibleIndex && ESPConfig::VisibleCheck) ?
				ESPConfig::VisibleColor : ESPConfig::BoxColor;

			if (ESPConfig::BoxType == 0) // Regular box
			{
				if (ESPConfig::OutLine)
				{
					Gui.Rectangle(rectPos, rectSize, ESPConfig::BoxColor & IM_COL32_A_MASK,
						3, ESPConfig::BoxRounding);
				}
				Gui.Rectangle(rectPos, rectSize, boxColor, 1.3f, ESPConfig::BoxRounding);
			}
			else if (ESPConfig::BoxType == 1) // Corner box
			{
				if (ESPConfig::OutLine)
				{
					DrawCornerBox(Rect, ESPConfig::BoxColor & IM_COL32_A_MASK, 3.0f);
				}
				DrawCornerBox(Rect, boxColor, 1.3f);
			}
		}

		// Line to enemy
		if (ESPConfig::ShowLineToEnemy)
		{
			Render::LineToEnemy(Rect, ESPConfig::LineToEnemyColor, 1.2f);
		}

		// Weapon ESP with optimized icon lookup
		if (ESPConfig::ShowWeaponESP)
		{
			const std::string weaponIcon = GunIcon(Entity.Pawn.WeaponName);
			const WeaponIconSize& iconSize = GetWeaponIconSize(Entity.Pawn.WeaponName);

			ImVec2 textPosition = {
				Rect.x + (Rect.z - iconSize.width) * 0.5f + iconSize.offsetX,
				Rect.y + Rect.w + 1 + iconSize.offsetY
			};

			if (ESPConfig::AmmoBar)
				textPosition.y += 6;

			DrawTextWithOutline(s_renderCache.drawList, s_renderCache.font1, 10.0f,
				textPosition, IM_COL32(255, 255, 255, 255), IM_COL32(0, 0, 0, 255),
				weaponIcon.c_str());
		}

		// Scoped indicator
		if (ESPConfig::ShowIsScoped)
		{
			bool isScoped;
			if (memoryManager.ReadMemory<bool>(Entity.Pawn.Address + Offset.Pawn.isScoped, isScoped) && isScoped)
			{
				const ImVec2 iconPos = { Rect.x, Rect.y };
				DrawTextWithOutline(s_renderCache.drawList, s_renderCache.font1, 12.0f,
					iconPos, IM_COL32(131, 137, 150, 255), IM_COL32(0, 0, 0, 255), "s");
			}
		}

		// Flash indicator
		if (ESPConfig::ShowIsBlind && Entity.Pawn.FlashDuration > 0.f)
		{
			const ImVec2 iconPos = { Rect.x, Rect.y + 12 };
			DrawTextWithOutline(s_renderCache.drawList, s_renderCache.font1, 12.0f,
				iconPos, IM_COL32(131, 137, 150, 255), IM_COL32(0, 0, 0, 255), "i");
		}

		// Player name
		if (ESPConfig::ShowPlayerName)
		{
			const ImVec2 namePos = { Rect.x + Rect.z * 0.5f, Rect.y - 10 };
			Gui.StrokeText(Entity.Controller.PlayerName, namePos, ImColor(255, 255, 255, 255), 10, true);
		}
	}

	constexpr void DrawPreviewBox(const ImVec2& startPos, const ImVec2& endPos,
		ImColor boxColor, float rounding, float thickness, bool filled) noexcept
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (filled)
			drawList->AddRectFilled(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All);
		else
			drawList->AddRect(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All, thickness);
	}

	void RenderPreview(ImVec2 windowSize)
	{
		if (!ESPConfig::ShowPreview) return;

		s_renderCache.UpdateFonts();
		auto drawList = s_renderCache.drawList;

		constexpr ImVec2 rectSize(100, 150);
		const ImVec2 rectPos(windowSize.x * 0.45f - rectSize.x * 0.5f, windowSize.y * 0.3f - rectSize.y * 0.5f);

		ImVec2 centerPos = ImGui::GetCursorScreenPos();
		centerPos.x += rectPos.x;
		centerPos.y += rectPos.y * -1.20f;

		// Batch drawing operations by type for better performance

		// Lines and rays
		if (ESPConfig::ShowEyeRay)
		{
			const ImVec2 lineStart(centerPos.x + 44, centerPos.y + 15);
			const ImVec2 lineEnd(centerPos.x - 10, centerPos.y + 20);
			drawList->AddLine(lineStart, lineEnd, ESPConfig::EyeRayColor, 2.0f);
		}

		// Bone structure
		if (ESPConfig::ShowBoneESP)
		{
			struct BoneSegment { ImVec2 startOffset, endOffset; };
			constexpr BoneSegment segments[] = {
				{{50, 25}, {60, 55}},   // Neck to Spine
				{{60, 55}, {62, 65}},   // Spine to Pelvis
				{{62, 65}, {65, 70}},   // Left Leg Up
				{{65, 70}, {60, 100}},  // Left Leg Mid
				{{60, 100}, {68, 145}}, // Left Leg Down
				{{62, 65}, {35, 100}},  // Right Leg Up
				{{35, 100}, {47, 130}}, // Right Leg Down
				{{50, 25}, {60, 30}},   // Left Scapula
				{{60, 30}, {45, 55}},   // Left Arm Up
				{{45, 55}, {25, 45}},   // Left Arm Down
				{{50, 25}, {40, 30}},   // Right Scapula
				{{40, 30}, {27, 53}},   // Right Arm Up
				{{27, 53}, {20, 45}}    // Right Arm Down
			};

			for (const auto& seg : segments)
			{
				const ImVec2 start(centerPos.x + seg.startOffset.x, centerPos.y + seg.startOffset.y);
				const ImVec2 end(centerPos.x + seg.endOffset.x, centerPos.y + seg.endOffset.y);
				drawList->AddLine(start, end, ESPConfig::BoneColor, 1.8f);
			}
		}

		// Head circle
		if (ESPConfig::ShowHeadBox)
		{
			drawList->AddCircle({ centerPos.x + 44, centerPos.y + 17 }, 12.0f, ESPConfig::HeadBoxColor, 0, 1.8f);
		}

		// Filled box
		if (ESPConfig::FilledBox)
		{
			const ImVec2 rectStart = centerPos;
			const ImVec2 rectEnd(centerPos.x + rectSize.x, centerPos.y + rectSize.y);

			if (ESPConfig::MultiColor)
			{
				drawList->AddRectFilledMultiColorRounded(rectStart, rectEnd,
					ImGui::GetColorU32(ImGuiCol_ChildBg), ESPConfig::FilledColor,
					ESPConfig::FilledColor, ESPConfig::FilledColor2, ESPConfig::FilledColor2,
					ESPConfig::BoxRounding, ImDrawCornerFlags_All);
			}
		}

		// Box ESP
		if (ESPConfig::ShowBoxESP)
		{
			const ImVec2 rectStart = centerPos;
			const ImVec2 rectEnd(centerPos.x + rectSize.x, centerPos.y + rectSize.y);

			switch (ESPConfig::BoxType)
			{
			case 0:
				DrawPreviewBox(rectStart, rectEnd, ESPConfig::BoxColor, ESPConfig::BoxRounding, 1.3f, false);
				break;
			case 1:
			{
				const float quarterX = rectSize.x * 0.25f;
				const float quarterY = rectSize.y * 0.25f;

				struct LineSeg { ImVec2 p1, p2; };
				const LineSeg segs[] = {
					{rectStart, {rectStart.x + quarterX, rectStart.y}},
					{rectStart, {rectStart.x, rectStart.y + quarterY}},
					{rectEnd, {rectEnd.x - quarterX, rectEnd.y}},
					{rectEnd, {rectEnd.x, rectEnd.y - quarterY}},
					{{rectStart.x, rectStart.y + rectSize.y}, {rectStart.x + quarterX, rectStart.y + rectSize.y}},
					{{rectStart.x, rectStart.y + rectSize.y}, {rectStart.x, rectStart.y + rectSize.y - quarterY}},
					{{rectStart.x + rectSize.x, rectStart.y}, {rectStart.x + rectSize.x - quarterX, rectStart.y}},
					{{rectStart.x + rectSize.x, rectStart.y}, {rectStart.x + rectSize.x, rectStart.y + quarterY}}
				};

				for (const auto& seg : segs)
					drawList->AddLine(seg.p1, seg.p2, ESPConfig::BoxColor, 1.3f);
			}
			break;
			}
		}

		// Health bars (batch all bar drawing)
		if (ESPConfig::ShowHealthBar)
		{
			const ImVec2 healthBarStart(centerPos.x - 6, centerPos.y);
			const ImVec2 healthBarEnd(centerPos.x - 3, centerPos.y + rectSize.y);
			drawList->AddRectFilled(healthBarStart, healthBarEnd, IM_COL32(0, 255, 0, 255));
		}

		if (ESPConfig::ArmorBar)
		{
			const ImVec2 armorBarStart(centerPos.x - 9, centerPos.y);
			const ImVec2 armorBarEnd(centerPos.x - 6, centerPos.y + rectSize.y);
			drawList->AddRectFilled(armorBarStart, armorBarEnd, IM_COL32(0, 128, 255, 255));
		}

		if (ESPConfig::AmmoBar)
		{
			const ImVec2 ammoBarStart(centerPos.x, centerPos.y + rectSize.y + 2);
			const ImVec2 ammoBarEnd(centerPos.x + rectSize.x, centerPos.y + rectSize.y + 5);
			drawList->AddRectFilled(ammoBarStart, ammoBarEnd, IM_COL32(255, 255, 0, 255));
		}

		// Line to enemy
		if (ESPConfig::ShowLineToEnemy)
		{
			ImVec2 lineStart(centerPos.x + rectSize.x * 0.5f, centerPos.y);
			ImVec2 lineEnd;

			switch (ESPConfig::LinePos)
			{
			case 0: lineEnd = ImVec2(lineStart.x, lineStart.y - 50); break;
			case 1: lineEnd = ImVec2(Gui.Window.Size.x * 0.5f, Gui.Window.Size.y * 0.5f); break;
			case 2: lineStart = ImVec2(centerPos.x + rectSize.x * 0.5f, centerPos.y + rectSize.y);
				lineEnd = ImVec2(lineStart.x, lineStart.y + 20); break;
			}

			drawList->AddLine(lineStart, lineEnd, ESPConfig::LineToEnemyColor, 1.8f);
		}

		// Text elements (batch all text drawing)
		if (ESPConfig::ShowPlayerName)
		{
			const ImVec2 textPos(centerPos.x + 36, centerPos.y - 12);
			drawList->AddText(s_renderCache.font0, 12.0f, textPos, IM_COL32(255, 255, 255, 255), "Player");
		}

		if (ESPConfig::ShowDistance)
		{
			const ImVec2 textPos(centerPos.x + 105, centerPos.y);
			drawList->AddText(textPos, IM_COL32(0, 98, 98, 255), "108m");
		}

		if (ESPConfig::ShowWeaponESP)
		{
			ImVec2 weaponPos(centerPos.x + 35, centerPos.y + 150);
			if (ESPConfig::AmmoBar) weaponPos.y += 5;
			drawList->AddText(s_renderCache.font1, 12.0f, weaponPos, IM_COL32(255, 255, 255, 255), "W");
		}

		if (ESPConfig::ShowIsScoped)
		{
			drawList->AddText(s_renderCache.font1, 15.0f, centerPos, IM_COL32(131, 137, 150, 255), "s");
		}

		if (ESPConfig::ShowIsBlind)
		{
			const ImVec2 flashPos(centerPos.x, centerPos.y + 15);
			drawList->AddText(s_renderCache.font1, 15.0f, flashPos, IM_COL32(131, 137, 150, 255), "i");
		}
	}
}
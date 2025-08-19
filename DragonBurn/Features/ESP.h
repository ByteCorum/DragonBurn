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
	static std::unordered_map<std::string, WeaponIconSize> weaponIconSizes = 
	{
{"t_knife", {13.0f, 13.0f, -5.0f, 0.0f}},
{"ct_knife", {13.0f, 13.0f, -5.0f, 0.0f}},
{"deagle", {13.0f, 13.0f, -5.0f, 0.0f}},
{"elite", {13.0f, 13.0f, 0.0f, 0.0f}},
{"fiveseven", {13.0f, 13.0f, 0.0f, 0.0f}},
{"glock", {13.0f, 13.0f, 0.0f, 0.0f}},
{"revolver", {13.0f, 13.0f, -3.0f, 0.0f}},
{"p2000", {13.0f, 13.0f, 0.0f, 0.0f}},
{"p250", {13.0f, 13.0f, 0.0f, 0.0f}},
{"usp", {13.0f, 13.0f, 0.0f, 0.0f}},
{"tec9", {13.0f, 13.0f, 0.0f, 0.0f}},
{"cz75a", {13.0f, 13.0f, 0.0f, 0.0f}},
{"mac10", {13.0f, 13.0f, 0.0f, 0.0f}},
{"ump45", {13.0f, 13.0f, -7.0f, 0.0f}},
{"bizon", {13.0f, 13.0f, -7.0f, 0.0f}},
{"mp7", {13.0f, 13.0f, -3.0f, 0.0f}},
{"mp9", {13.0f, 13.0f, -7.0f, 0.0f}},
{"p90", {13.0f, 13.0f, -7.0f, 0.0f}},
{"galilar", {13.0f, 13.0f, -10.0f, 0.0f}},
{"famas", {13.0f, 13.0f, -10.0f, 0.0f}},
{"m4a4", {13.0f, 13.0f, -10.0f, 0.0f}},
{"m4a1", {13.0f, 13.0f, -10.0f, 0.0f}},
{"aug", {13.0f, 13.0f, -7.0f, 0.0f}},
{"sg556", {13.0f, 13.0f, -10.0f, 0.0f}},
{"ak47", {13.0f, 13.0f, -10.0f, 0.0f}},
{"g3sg1", {13.0f, 13.0f, -10.0f, 0.0f}},
{"scar20", {13.0f, 13.0f, -10.0f, 0.0f}},
{"awp", {13.0f, 13.0f, -10.0f, 0.0f}},
{"ssg08", {13.0f, 13.0f, -10.0f, 0.0f}},
{"xm1014", {13.0f, 13.0f, -10.0f, 0.0f}},
{"sawedoff", {13.0f, 13.0f, -10.0f, 0.0f}},
{"mag7", {13.0f, 13.0f, -10.0f, 0.0f}},
{"nova", {13.0f, 13.0f, -10.0f, 0.0f}},
{"negev", {13.0f, 13.0f, -10.0f, 0.0f}},
{"m249", {13.0f, 13.0f, -10.0f, 0.0f}},
{"taser", {13.0f, 13.0f, 0.0f, 0.0f}},
{"flashbang", {13.0f, 13.0f, 3.0f, 0.0f}},
{"hegrenade", {13.0f, 13.0f, 3.0f, 0.0f}},
{"smokegrenade", {13.0f, 13.0f, 3.0f, 0.0f}},
{"molotov", {13.0f, 13.0f, 3.0f, 0.0f}},
{"decoy", {13.0f, 13.0f, 3.0f, 0.0f}},
{"incgrenade", {13.0f, 13.0f, 3.0f, 0.0f}},
{"c4", {13.0f, 13.0f, 0.0f, 0.0f}},
	};

	ImVec4 GetBoxRect(const CEntity& Entity, int BoxType)
	{
		ImVec4 Rect;
		switch (BoxType)
		{
		case 0:
			Rect = Render::Get2DBox(Entity);
			break;
		case 1:
			Rect = Render::Get2DBox(Entity);
			break;
		default:
			break;
		}

		return Rect;
	}

	void RenderPlayerESP(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect, int LocalPlayerControllerIndex, int Index)
	{
		if (Entity.Pawn.Pos == Vec3(0, 0, 0))
			return;

		// Cache frequently used values
		std::string weaponIcon = GunIcon(Entity.Pawn.WeaponName);
		const auto ioFonts = ImGui::GetIO().Fonts->Fonts[1];
		DWORD64 playerMask = (DWORD64(1) << LocalPlayerControllerIndex);
		bool bIsVisible = (Entity.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
		bool bIsVisibleIndex = (Entity.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & (DWORD64(1) << Index));

		// Render bones, LOS, and head circle
		Render::DrawBone(Entity, ESPConfig::BoneColor, 1.3f);
		Render::ShowLosLine(Entity, 50.0f, ESPConfig::EyeRayColor, 1.3f);
		Render::DrawHeadCircle(Entity, ESPConfig::HeadBoxColor);

		// Draw filled box if enabled
		if (ESPConfig::FilledBox) {
			float rounding = ESPConfig::BoxRounding;
			ImColor flatBoxCol = ESPConfig::FilledColor;
			ImColor flatBoxCol2 = ESPConfig::FilledColor2;
			ImColor flatBoxVisCol = ESPConfig::BoxFilledVisColor;
			if (ESPConfig::FilledVisBox) {
				Gui.RectangleFilled({ Rect.x, Rect.y }, { Rect.z, Rect.w }, bIsVisible ? flatBoxVisCol : flatBoxCol, rounding);
			}
			else {
				if (ESPConfig::MultiColor) {
					Gui.RectangleFilledGraident({ Rect.x, Rect.y }, { Rect.z, Rect.w }, ESPConfig::BoxColor, flatBoxCol, flatBoxCol2, rounding);
				}
				else {
					Gui.RectangleFilled({ Rect.x, Rect.y }, { Rect.z, Rect.w }, flatBoxCol, rounding);
				}
			}
		}

		if (ESPConfig::ShowBoxESP) {
			if (ESPConfig::BoxType == 0) {
				if (ESPConfig::OutLine)
					Gui.Rectangle({ Rect.x, Rect.y }, { Rect.z, Rect.w }, ESPConfig::BoxColor & IM_COL32_A_MASK, 3, ESPConfig::BoxRounding);

				if (bIsVisibleIndex && ESPConfig::VisibleCheck)
					Gui.Rectangle({ Rect.x, Rect.y }, { Rect.z, Rect.w }, ESPConfig::VisibleColor, 1.3f, ESPConfig::BoxRounding);
				else
					Gui.Rectangle({ Rect.x, Rect.y }, { Rect.z, Rect.w }, ESPConfig::BoxColor, 1.3f, ESPConfig::BoxRounding);
			}
			else if (ESPConfig::BoxType == 1) {
				const int outlineThickness = 3;
				const float quarterWidth = Rect.z * 0.25f;
				const float quarterHeight = Rect.w * 0.25f;
				ImVec2 topLeft = { Rect.x, Rect.y };
				ImVec2 topRight = { Rect.x + Rect.z, Rect.y };
				ImVec2 bottomLeft = { Rect.x, Rect.y + Rect.w };
				ImVec2 bottomRight = { Rect.x + Rect.z, Rect.y + Rect.w };

				struct LineSegment { ImVec2 p1, p2; };
				LineSegment outlineSegments[8] = {
					{ topLeft, { topLeft.x + quarterWidth, topLeft.y } },
					{ topLeft, { topLeft.x, topLeft.y + quarterHeight } },
					{ topRight, { topRight.x - quarterWidth, topRight.y } },
					{ topRight, { topRight.x, topRight.y + quarterHeight } },
					{ bottomLeft, { bottomLeft.x + quarterWidth, bottomLeft.y } },
					{ bottomLeft, { bottomLeft.x, bottomLeft.y - quarterHeight } },
					{ bottomRight, { bottomRight.x - quarterWidth, bottomRight.y } },
					{ bottomRight, { bottomRight.x, bottomRight.y - quarterHeight } }
				};

				for (const auto& seg : outlineSegments)
					Gui.Line(seg.p1, seg.p2, ESPConfig::BoxColor & IM_COL32_A_MASK, outlineThickness);

				const float lineThickness = 1.3f;
				LineSegment mainSegments[8] = {
					{ topLeft, { topLeft.x + quarterWidth, topLeft.y } },
					{ topLeft, { topLeft.x, topLeft.y + quarterHeight } },
					{ topRight, { topRight.x - quarterWidth, topRight.y } },
					{ topRight, { topRight.x, topRight.y + quarterHeight } },
					{ bottomLeft, { bottomLeft.x + quarterWidth, bottomLeft.y } },
					{ bottomLeft, { bottomLeft.x, bottomLeft.y - quarterHeight } },
					{ bottomRight, { bottomRight.x - quarterWidth, bottomRight.y } },
					{ bottomRight, { bottomRight.x, bottomRight.y - quarterHeight } }
				};

				ImU32 lineColor = (bIsVisible && ESPConfig::VisibleCheck) ? ESPConfig::VisibleColor : ESPConfig::BoxColor;
				for (const auto& seg : mainSegments)
					Gui.Line(seg.p1, seg.p2, lineColor, lineThickness);
			}
		}

		Render::LineToEnemy(Rect, ESPConfig::LineToEnemyColor, 1.2f);

		if (ESPConfig::ShowWeaponESP) {
			WeaponIconSize iconSize = weaponIconSizes[Entity.Pawn.WeaponName];
			ImVec2 textPosition = { Rect.x + (Rect.z - iconSize.width) / 2 + iconSize.offsetX,
									  Rect.y + Rect.w + 1 + iconSize.offsetY };
			if (ESPConfig::AmmoBar)
				textPosition.y += 6;

			const ImVec2 offsets[4] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };
			for (const auto& off : offsets) {
				ImVec2 pos = { textPosition.x + off.x, textPosition.y + off.y };
				ImGui::GetBackgroundDrawList()->AddText(ioFonts, 10.0f, pos, ImColor(0, 0, 0, 255), weaponIcon.c_str());
			}
			ImGui::GetBackgroundDrawList()->AddText(ioFonts, 10.0f, textPosition, ImColor(255, 255, 255, 255), weaponIcon.c_str());
		}

        if (ESPConfig::ShowIsScoped) {
            bool isScoped;
            memoryManager.ReadMemory<bool>(Entity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
            if (isScoped) {
                ImVec2 iconPos = { Rect.x, Rect.y };
                const ImVec2 scopeOffsets[4] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };
                for (const auto& off : scopeOffsets) {
                    ImVec2 pos = { iconPos.x + off.x, iconPos.y + off.y };
                    ImGui::GetBackgroundDrawList()->AddText(ioFonts, 12.0f, pos, ImColor(0, 0, 0, 255), "s");
                }
                ImGui::GetBackgroundDrawList()->AddText(ioFonts, 12.0f, iconPos, ImColor(131, 137, 150, 255), "s");
            }
        }

        if (ESPConfig::ShowIsBlind) {
            if (Entity.Pawn.FlashDuration > 0.f)
            {
                ImVec2 iconPos = { Rect.x+3, Rect.y + 12 };
                const ImVec2 flashOffsets[4] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };
                for (const auto& off : flashOffsets)
                {
                    ImVec2 pos = { iconPos.x + off.x, iconPos.y + off.y };
                    ImGui::GetBackgroundDrawList()->AddText(ioFonts, 18.f, pos, ImColor(0, 0, 0, 255), "i");
                }
                ImGui::GetBackgroundDrawList()->AddText(ioFonts, 18.f, iconPos, ImColor(131, 137, 150, 255), "i");
            }
        }

		if (ESPConfig::ShowPlayerName) {
			Gui.StrokeText(Entity.Controller.PlayerName, { Rect.x + Rect.z / 2, Rect.y - 10 }, ImColor(255, 255, 255, 255), 10, true);
		}
	}

	void DrawPreviewBox(const ImVec2& startPos, const ImVec2& endPos, ImColor boxColor, float rounding, float thickness, bool filled) {
		if (filled)
			ImGui::GetWindowDrawList()->AddRectFilled(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All);
		else
			ImGui::GetWindowDrawList()->AddRect(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All, thickness);
	}

    void RenderPreview(ImVec2 windowSize)
    {
        if (!ESPConfig::ShowPreview)
            return;

        auto drawList = ImGui::GetWindowDrawList();
        auto& io = ImGui::GetIO();
        auto font0 = io.Fonts->Fonts[0];
        auto font1 = io.Fonts->Fonts[1];

        const ImVec2 rectSize(100, 150);
        const ImVec2 rectPos((windowSize.x - rectSize.x) * 0.45f, (windowSize.y - rectSize.y) * 0.3f);
        ImVec2 centerPos = ImGui::GetCursorScreenPos();
        centerPos.x += rectPos.x;
        centerPos.y += rectPos.y * -1.20f;

        if (ESPConfig::ShowEyeRay) {
            const ImU32 eyeColor = ESPConfig::EyeRayColor;
            const ImVec2 lineStart(centerPos.x + 44, centerPos.y + 15);
            const ImVec2 lineEnd(centerPos.x - 10, centerPos.y + 20);
            drawList->AddLine(lineStart, lineEnd, eyeColor, 2.0f);
        }

        if (ESPConfig::ShowBoneESP) {
            const ImU32 boneColor = ESPConfig::BoneColor;
            struct BoneSegment { ImVec2 startOffset, endOffset; };
            const BoneSegment segments[] = {
                { {50, 25}, {60, 55} },   // Neck to Spine
                { {60, 55}, {62, 65} },   // Spine to Pelvis
                { {62, 65}, {65, 70} },   // Left Leg Up
                { {65, 70}, {60, 100} },  // Left Leg Mid
                { {60, 100}, {68, 145} }, // Left Leg Down
                { {62, 65}, {35, 100} },  // Right Leg Up
                { {35, 100}, {47, 130} }, // Right Leg Down
                { {50, 25}, {60, 30} },   // Left Scapula
                { {60, 30}, {45, 55} },   // Left Arm Up
                { {45, 55}, {25, 45} },   // Left Arm Down
                { {50, 25}, {40, 30} },   // Right Scapula
                { {40, 30}, {27, 53} },   // Right Arm Up
                { {27, 53}, {20, 45} }    // Right Arm Down
            };
            for (const auto& seg : segments) {
                ImVec2 start(centerPos.x + seg.startOffset.x, centerPos.y + seg.startOffset.y);
                ImVec2 end(centerPos.x + seg.endOffset.x, centerPos.y + seg.endOffset.y);
                drawList->AddLine(start, end, boneColor, 1.8f);
            }
        }

        if (ESPConfig::ShowHeadBox) {
            drawList->AddCircle({ centerPos.x + 44, centerPos.y + 17 }, 12.0f, ESPConfig::HeadBoxColor, 0, 1.8f);
        }

        if (ESPConfig::FilledBox) {
            const ImColor filledColor(ESPConfig::FilledColor.Value.x, ESPConfig::FilledColor.Value.y,
                ESPConfig::FilledColor.Value.z, ESPConfig::FilledColor.Value.w);
            const ImColor filledColor2(ESPConfig::FilledColor2.Value.x, ESPConfig::FilledColor2.Value.y,
                ESPConfig::FilledColor2.Value.z, ESPConfig::FilledColor2.Value.w);
            const ImVec2 rectStart = centerPos;
            const ImVec2 rectEnd = { rectStart.x + rectSize.x, rectStart.y + rectSize.y };

            if (ESPConfig::MultiColor)
            {
                drawList->AddRectFilledMultiColorRounded(rectStart, rectEnd,
                    ImGui::GetColorU32(ImGuiCol_ChildBg),
                    filledColor, filledColor,
                    filledColor2, filledColor2,
                    ESPConfig::BoxRounding, ImDrawCornerFlags_All);
            }
        }

        if (ESPConfig::ShowBoxESP) {
            const ImColor boxColor = ESPConfig::BoxColor;
            const ImVec2 rectStart = centerPos;
            const ImVec2 rectEnd = { rectStart.x + rectSize.x, rectStart.y + rectSize.y };

            switch (ESPConfig::BoxType)
            {
            case 0:
                DrawPreviewBox(rectStart, rectEnd, boxColor, ESPConfig::BoxRounding, 1.3f, false);
                break;
            case 1: {
                const float quarterX = rectSize.x * 0.25f;
                const float quarterY = rectSize.y * 0.25f;
                struct LineSeg { ImVec2 p1, p2; };
                const LineSeg segs[] = {
                    { rectStart, { rectStart.x + quarterX, rectStart.y } },
                    { rectStart, { rectStart.x, rectStart.y + quarterY } },
                    { { rectStart.x + rectSize.x, rectStart.y + rectSize.y },
                      { rectStart.x + rectSize.x * 0.75f, rectStart.y + rectSize.y } },
                    { { rectStart.x + rectSize.x, rectStart.y + rectSize.y },
                      { rectStart.x + rectSize.x, rectStart.y + rectSize.y * 0.75f } },
                    { { rectStart.x, rectStart.y + rectSize.y },
                      { rectStart.x + quarterX, rectStart.y + rectSize.y } },
                    { { rectStart.x, rectStart.y + rectSize.y },
                      { rectStart.x, rectStart.y + rectSize.y * 0.75f } },
                    { { rectStart.x + rectSize.x, rectStart.y },
                      { rectStart.x + rectSize.x * 0.75f, rectStart.y } },
                    { { rectStart.x + rectSize.x, rectStart.y },
                      { rectStart.x + rectSize.x, rectStart.y + quarterY } }
                };
                for (const auto& seg : segs)
                    drawList->AddLine(seg.p1, seg.p2, boxColor, 1.3f);
                break;
            }
            }
        }

        if (ESPConfig::ShowHealthBar) {
            const ImU32 greenColor = IM_COL32(0, 255, 0, 255);
            const ImVec2 HBPos = centerPos;
            const ImVec2 HBSize = rectSize;
            const ImVec2 HBS(HBPos.x - 6, HBPos.y);
            const ImVec2 HBE(HBPos.x - 3, HBPos.y + HBSize.y);
            drawList->AddRectFilled(HBS, HBE, greenColor, 0.0f, ImDrawCornerFlags_All);
        }
        if (ESPConfig::ArmorBar) {
            const ImU32 blueColor = IM_COL32(0, 128, 255, 255);
            const ImVec2 ABPos = centerPos;
            const ImVec2 ABSize = rectSize;
            const ImVec2 ABS(ABPos.x - 9, ABPos.y);
            const ImVec2 ABE(ABPos.x - 6, ABPos.y + ABSize.y);
            drawList->AddRectFilled(ABS, ABE, blueColor, 0.0f, ImDrawCornerFlags_All);
        }
        if (ESPConfig::AmmoBar) {
            const ImU32 yellowColor = IM_COL32(255, 255, 0, 255);
            const ImVec2 ABS(centerPos.x, centerPos.y + rectSize.y + 2);
            const ImVec2 ABE(centerPos.x + rectSize.x, centerPos.y + rectSize.y + 5);
            drawList->AddRectFilled(ABS, ABE, yellowColor, 0.0f, ImDrawCornerFlags_All);
        }

        if (ESPConfig::ShowLineToEnemy) {
            ImVec2 lineStart = { centerPos.x + rectSize.x / 2, centerPos.y };
            ImVec2 lineEnd;
            switch (ESPConfig::LinePos)
            {
            case 0:
                lineEnd = { lineStart.x, lineStart.y - 50 };
                break;
            case 1:
                lineEnd = { Gui.Window.Size.x / 2, Gui.Window.Size.y / 2 };
                break;
            case 2:
                lineStart = { centerPos.x + rectSize.x / 2, centerPos.y + rectSize.y };
                lineEnd = { lineStart.x, lineStart.y + 20.f };
                break;
            }
            drawList->AddLine(lineStart, lineEnd, ESPConfig::LineToEnemyColor, 1.8f);
        }

        if (ESPConfig::ShowPlayerName) {
            const ImVec2 textPos(centerPos.x + 36, centerPos.y - 12);
            drawList->AddText(font0, 12.0f, textPos, IM_COL32(255, 255, 255, 255), "Player");
        }
        if (ESPConfig::ShowDistance) {
            const ImVec2 textPos(centerPos.x + 105, centerPos.y);
            drawList->AddText(textPos, IM_COL32(0, 98, 98, 255), "108m");
        }
        if (ESPConfig::ShowWeaponESP) {
            if (ESPConfig::AmmoBar)
                centerPos.y += 5;
            const ImVec2 textPos(centerPos.x + 35, centerPos.y + 150);
            drawList->AddText(font1, 12.0f, textPos, IM_COL32(255, 255, 255, 255), "W");
        }
        if (ESPConfig::ShowIsScoped) {
            drawList->AddText(font1, 15.0f, centerPos, IM_COL32(131, 137, 150, 255), "s");
        }
        if (ESPConfig::ShowIsBlind) {
            const ImVec2 flashed(centerPos.x+3, centerPos.y + 12);
            drawList->AddText(font1, 20.0f, flashed, IM_COL32(131, 137, 150, 255), "i");
        }
    }
}
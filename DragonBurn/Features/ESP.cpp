#include "..\Core\Render.h"
#include "..\Core\Config.h"
#include "..\Core\Cheats.h"
#include "..\Helpers\GetWeaponIcon.h"
#include "ESP.h"
#include "../Helpers/Logger.h"


namespace ESP
{

    // Static weapon icon sizes lookup table - const for better optimization
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
{"usp_silencer", {13.0f, 13.0f, 0.0f, 0.0f}},
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
{"m4a1_silencer", {13.0f, 13.0f, -10.0f, 0.0f}},
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
        // Simplified - both box types use the same calculation
        return Render::Get2DBox(Entity);
    }

    DrawData CalculateDrawData(const ImVec4& Rect, const CEntity& Entity)
    {
        DrawData data;

        data.rectStart = ImVec2(Rect.x, Rect.y);
        data.rectEnd = ImVec2(Rect.x + Rect.z, Rect.y + Rect.w);
        data.rectCenter = ImVec2(Rect.x + Rect.z * 0.5f, Rect.y + Rect.w * 0.5f);
        data.rectWidth = Rect.z;
        data.rectHeight = Rect.w;

        // Pre-calculate weapon text position
        WeaponIconSize iconSize = weaponIconSizes[Entity.Pawn.WeaponName];
        data.weaponTextPos = ImVec2(
            Rect.x + (Rect.z - iconSize.width) * 0.5f + iconSize.offsetX,
            Rect.y + Rect.w + 1.0f + iconSize.offsetY
        );
        if (ESPConfig::AmmoBar)
            data.weaponTextPos.y += 6.0f;

        // Pre-calculate name text position
        data.nameTextPos = ImVec2(data.rectCenter.x, Rect.y - 10.0f);

        // Pre-calculate scope icon position
        data.scopeIconPos = data.rectStart;

        data.isValid = true;
        return data;
    }

    void BatchBoneLines(BatchData& batch, const CEntity& Entity, ImU32 color, float thickness)
    {
        // Pre-defined bone connections for better performance
        constexpr std::array<std::pair<int, int>, 13> boneConnections = { {
            {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {2, 6}, {6, 7},
            {0, 8}, {8, 9}, {9, 10}, {0, 11}, {11, 12}, {12, 13}
        } };

        // Reserve space to avoid reallocation
        batch.lines.reserve(batch.lines.size() + boneConnections.size());

        for (const auto& connection : boneConnections)
        {
            // Note: This would need access to actual bone positions from Entity
            // For now, using placeholder - you'd need to implement proper bone position retrieval
            ImVec2 start = ImVec2(0, 0); // Entity.GetBonePosition(connection.first)
            ImVec2 end = ImVec2(0, 0);   // Entity.GetBonePosition(connection.second)

            batch.lines.push_back({ start, end, color, thickness });
        }
    }

    void BatchCornerBox(BatchData& batch, const ImVec4& Rect, ImU32 color, float thickness)
    {
        const float quarterWidth = Rect.z * 0.25f;
        const float quarterHeight = Rect.w * 0.25f;
        const ImVec2 topLeft(Rect.x, Rect.y);
        const ImVec2 topRight(Rect.x + Rect.z, Rect.y);
        const ImVec2 bottomLeft(Rect.x, Rect.y + Rect.w);
        const ImVec2 bottomRight(Rect.x + Rect.z, Rect.y + Rect.w);

        // 8 corner segments as in original
        std::array<std::pair<ImVec2, ImVec2>, 8> segments = { {
            {topLeft, {topLeft.x + quarterWidth, topLeft.y}},
            {topLeft, {topLeft.x, topLeft.y + quarterHeight}},
            {topRight, {topRight.x - quarterWidth, topRight.y}},
            {topRight, {topRight.x, topRight.y + quarterHeight}},
            {bottomLeft, {bottomLeft.x + quarterWidth, bottomLeft.y}},
            {bottomLeft, {bottomLeft.x, bottomLeft.y - quarterHeight}},
            {bottomRight, {bottomRight.x - quarterWidth, bottomRight.y}},
            {bottomRight, {bottomRight.x, bottomRight.y - quarterHeight}}
        } };

        for (const auto& seg : segments)
        {
            batch.lines.push_back({ seg.first, seg.second, color, thickness });
        }
    }

    void BatchOutlinedText(BatchData& batch, const std::string& text, const ImVec2& pos,
        ImU32 textColor, ImU32 outlineColor, float fontSize, ImFont* font)
    {
        batch.texts.reserve(batch.texts.size() + 5); // 4 outline + 1 main text

        // Add outline texts
        for (const auto& offset : TEXT_OUTLINE_OFFSETS)
        {
            batch.texts.push_back({
                text,
                ImVec2(pos.x + offset.x, pos.y + offset.y),
                outlineColor,
                fontSize,
                font
                });
        }

        // Add main text
        batch.texts.push_back({ text, pos, textColor, fontSize, font });
    }

    void RenderBatch(const BatchData& batch, bool preview)
    {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        if (preview)
            drawList = ImGui::GetWindowDrawList();

        // Batch render all lines
        for (const auto& line : batch.lines)
        {
            drawList->AddLine(line.p1, line.p2, line.color, line.thickness);
        }

        // Batch render all rectangles
        for (const auto& rect : batch.rectangles)
        {
            if (rect.filled)
            {
                drawList->AddRectFilled(rect.start, rect.end, rect.color, rect.rounding);
            }
            else
            {
                drawList->AddRect(rect.start, rect.end, rect.color, rect.rounding, 0, rect.thickness);
            }
        }

        // Batch render all texts
        for (const auto& text : batch.texts)
        {
            drawList->AddText(text.font, text.fontSize, text.position, text.color, text.text.c_str());
        }
    }

    void RenderPlayerESP(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect,
        int LocalPlayerControllerIndex, int Index, bool bIsVisible)
    {
        // Pre-calculate all draw data once
        DrawData drawData = CalculateDrawData(Rect, Entity);
        if (!drawData.isValid) return;

        // Create batch data with estimated capacity
        BatchData batch;
        batch.Reserve(20, 5, 10); // Rough estimates

        // Cache frequently used values
        std::string weaponIcon = GunIcon(Entity.Pawn.WeaponName);
        auto* WeaponsIconsFont = ImGui::GetIO().Fonts->Fonts[1];

        if (ESPConfig::ShowBoneESP)
        {
            Render::DrawBone(Entity, ESPConfig::BoneColor, 1.3f);
        }

        // Render LOS line (direct call - single line)
        if (ESPConfig::ShowEyeRay)
        {
            Render::ShowLosLine(Entity, 50.0f, ESPConfig::EyeRayColor, 1.3f);
        }

        // Render head circle (direct call - single circle)
        if (ESPConfig::ShowHeadBox)
        {
            Render::DrawHeadCircle(Entity, ESPConfig::HeadBoxColor);
        }

        // Batch filled box rendering
        if (ESPConfig::FilledBox)
        {
            if (ESPConfig::FilledVisBox)
            {

                ImU32 fillColor;
                fillColor = bIsVisible ? ESPConfig::BoxFilledVisColor : ESPConfig::FilledColor;
                batch.rectangles.push_back({
                    drawData.rectStart, drawData.rectEnd, fillColor,
                    ESPConfig::BoxRounding, 1.0f, true
                    });
            }
            else
            {
                if (ESPConfig::MultiColor)
                {
                    // Need to handle gradient - this requires direct rendering, not batching
                    auto* drawList = ImGui::GetBackgroundDrawList();
                    drawList->AddRectFilledMultiColor(
                        drawData.rectStart, drawData.rectEnd,
                        ESPConfig::BoxColor, ESPConfig::FilledColor,
                        ESPConfig::FilledColor, ESPConfig::FilledColor2
                    );
                }
                else
                {
                    batch.rectangles.push_back({
                        drawData.rectStart, drawData.rectEnd, ESPConfig::FilledColor,
                        ESPConfig::BoxRounding, 1.0f, true
                        });
                }
            }
        }

        // Batch box ESP rendering
        if (ESPConfig::ShowBoxESP)
        {
            const ImU32 boxColor = (bIsVisible && ESPConfig::VisibleCheck) ?
                ESPConfig::VisibleColor : ESPConfig::BoxColor;

            if (ESPConfig::BoxType == 0)
            {
                // Regular box
                if (ESPConfig::OutLine)
                {
                    batch.rectangles.push_back({
                        drawData.rectStart, drawData.rectEnd,
                        ESPConfig::BoxColor & IM_COL32_A_MASK,
                        ESPConfig::BoxRounding, 3.0f, false
                        });
                }

                batch.rectangles.push_back({
                    drawData.rectStart, drawData.rectEnd, boxColor,
                    ESPConfig::BoxRounding, 1.3f, false
                    });
            }
            else if (ESPConfig::BoxType == 1)
            {
                // Corner box
                BatchCornerBox(batch, Rect, boxColor, 1.3f);

                if (ESPConfig::OutLine)
                {
                    BatchCornerBox(batch, Rect, ESPConfig::BoxColor & IM_COL32_A_MASK, 3.0f);
                }
            }
        }

        // Line to enemy (direct call - single line)
        if (ESPConfig::ShowLineToEnemy)
        {
            Render::LineToEnemy(Rect, ESPConfig::LineToEnemyColor, 1.2f);
        }

        //// Batch weapon ESP text
        if (ESPConfig::ShowWeaponESP)
        {
            if (!weaponIcon.empty()) {
                // Get weapon icon size from lookup table (same as original)
                WeaponIconSize iconSize = weaponIconSizes[Entity.Pawn.WeaponName];

                // Calculate position exactly like original
                ImVec2 textPosition = {
                    Rect.x + (Rect.z - iconSize.width) * 0.5f + iconSize.offsetX,
                    Rect.y + Rect.w + 1.0f + iconSize.offsetY
                };

                if (ESPConfig::AmmoBar)
                    textPosition.y += 6.0f;

                // Use batched outlined text rendering
                BatchOutlinedText(batch, weaponIcon, textPosition,
                    ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255),
                    10.0f, WeaponsIconsFont);
            }

            //else if (weaponIcon.empty()) {
            //    ImVec2 textPosition = {
            //   Rect.x + (Rect.z - 13.0f) * 0.5f,
            //   Rect.y + Rect.w + 1.0f
            //    };

            //    if (ESPConfig::AmmoBar)
            //        textPosition.y += 6.0f;

            //    // Render weapon name
            //    BatchOutlinedText(batch, Entity.Pawn.WeaponName, textPosition,
            //        ImColor(255, 0, 0, 255), ImColor(0, 0, 0, 255),
            //        10.0f, ImGui::GetIO().Fonts->Fonts[0]);
            //}
        }

        // Batch scoped indicator
        if (ESPConfig::ShowIsScoped)
        {
            bool isScoped = false;
            memoryManager.ReadMemory<bool>(Entity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
            if (isScoped)
            {
                BatchOutlinedText(batch, "s", drawData.scopeIconPos,
                    ImColor(131, 137, 150, 255), ImColor(0, 0, 0, 255),
                    12.0f, WeaponsIconsFont);
            }
        }

        // Batch player name
        if (ESPConfig::ShowPlayerName)
        {
            BatchOutlinedText(batch, Entity.Controller.PlayerName, drawData.nameTextPos,
                ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255),
                10.0f, ImGui::GetIO().Fonts->Fonts[0]);
        }

        // Render all batched elements at once
        RenderBatch(batch);
    }

    void DrawPreviewBox(const ImVec2& startPos, const ImVec2& endPos, ImColor boxColor,
        float rounding, float thickness, bool filled)
    {
        auto* drawList = ImGui::GetWindowDrawList();
        if (filled)
            drawList->AddRectFilled(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All);
        else
            drawList->AddRect(startPos, endPos, boxColor, rounding, ImDrawCornerFlags_All, thickness);
    }

    // In ESP.cpp, replace the RenderPreview function with this corrected version:

    void RenderPreview(ImVec2 windowSize)
    {
        if (!ESPConfig::ShowPreview)
            return;

        auto* drawList = ImGui::GetWindowDrawList();
        const auto& io = ImGui::GetIO();
        auto* font0 = io.Fonts->Fonts[0];
        auto* font1 = io.Fonts->Fonts[1];

        const ImVec2 rectSize(100, 150);
        const ImVec2 rectPos((windowSize.x - rectSize.x) * 0.45f, (windowSize.y - rectSize.y) * 0.3f);
        ImVec2 centerPos = ImGui::GetCursorScreenPos();
        centerPos.x += rectPos.x;
        centerPos.y += rectPos.y * -1.20f;

        // Create batch for preview elements
        BatchData previewBatch;
        previewBatch.Reserve(20, 10, 5);

        // Eye ray
        if (ESPConfig::ShowEyeRay)
        {
            const ImVec2 lineStart(centerPos.x + 44, centerPos.y + 15);
            const ImVec2 lineEnd(centerPos.x - 10, centerPos.y + 20);
            previewBatch.lines.push_back({ lineStart, lineEnd, ESPConfig::EyeRayColor, 2.0f });
        }

        // Bone ESP
        if (ESPConfig::ShowBoneESP)
        {
            constexpr std::array<std::pair<ImVec2, ImVec2>, 13> boneSegments = { {
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
            } };

            for (const auto& seg : boneSegments)
            {
                ImVec2 start(centerPos.x + seg.first.x, centerPos.y + seg.first.y);
                ImVec2 end(centerPos.x + seg.second.x, centerPos.y + seg.second.y);
                previewBatch.lines.push_back({ start, end, ESPConfig::BoneColor, 1.8f });
            }
        }

        // Head box (direct call - single circle)
        if (ESPConfig::ShowHeadBox)
        {
            drawList->AddCircle({ centerPos.x + 44, centerPos.y + 17 }, 12.0f, ESPConfig::HeadBoxColor, 0, 1.8f);
        }

        // Filled box
        if (ESPConfig::FilledBox)
        {
            if (ESPConfig::MultiColor)
            {
                // Direct rendering for gradient
                drawList->AddRectFilledMultiColor(
                    centerPos,
                    { centerPos.x + rectSize.x, centerPos.y + rectSize.y },
                    ESPConfig::BoxColor, ESPConfig::FilledColor,
                    ESPConfig::FilledColor, ESPConfig::FilledColor2
                );
            }
            else
            {
                previewBatch.rectangles.push_back({
                    centerPos,
                    {centerPos.x + rectSize.x, centerPos.y + rectSize.y},
                    ESPConfig::FilledColor, ESPConfig::BoxRounding, 1.0f, true
                    });
            }
        }

        // Box ESP
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

        // Health bar (direct rendering - single rect)
        if (ESPConfig::ShowHealthBar)
        {
            const ImVec2 hbStart(centerPos.x - 6, centerPos.y);
            const ImVec2 hbEnd(centerPos.x - 3, centerPos.y + rectSize.y);
            drawList->AddRectFilled(hbStart, hbEnd, ImColor(0, 255, 0, 255));
        }

        // Armor bar (direct rendering - single rect)
        if (ESPConfig::ArmorBar)
        {
            const ImVec2 abStart(centerPos.x - 9, centerPos.y);
            const ImVec2 abEnd(centerPos.x - 6, centerPos.y + rectSize.y);
            drawList->AddRectFilled(abStart, abEnd, ImColor(0, 128, 255, 255));
        }

        // Ammo bar (direct rendering - single rect)
        if (ESPConfig::AmmoBar)
        {
            const ImVec2 ammoStart(centerPos.x, centerPos.y + rectSize.y + 2);
            const ImVec2 ammoEnd(centerPos.x + rectSize.x, centerPos.y + rectSize.y + 5);
            drawList->AddRectFilled(ammoStart, ammoEnd, ImColor(255, 255, 0, 255));
        }

        // Line to enemy
        if (ESPConfig::ShowLineToEnemy)
        {
            ImVec2 lineStart = { centerPos.x + rectSize.x * 0.5f, centerPos.y };
            ImVec2 lineEnd;

            switch (ESPConfig::LinePos)
            {
            case 0: lineEnd = { lineStart.x, lineStart.y - 50 }; break;
            case 1: lineEnd = { Gui.Window.Size.x * 0.5f, Gui.Window.Size.y * 0.5f }; break;
            case 2:
                lineStart.y = centerPos.y + rectSize.y;
                lineEnd = { lineStart.x, lineStart.y + 20.0f };
                break;
            }
            previewBatch.lines.push_back({ lineStart, lineEnd, ESPConfig::LineToEnemyColor, 1.8f });
        }

        // Batch text elements
        if (ESPConfig::ShowPlayerName)
        {
            const ImVec2 textPos(centerPos.x + 36, centerPos.y - 12);
            BatchOutlinedText(previewBatch, "Player", textPos,
                ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255),
                12.0f, font0);
        }

        if (ESPConfig::ShowDistance)
        {
            const ImVec2 textPos(centerPos.x + 105, centerPos.y);
            BatchOutlinedText(previewBatch, "108m", textPos,
                ImColor(0, 98, 98, 255), ImColor(0, 0, 0, 255),
                12.0f, font0);
        }

        if (ESPConfig::ShowWeaponESP)
        {
            ImVec2 weaponPos(centerPos.x + 35, centerPos.y + 150);
            if (ESPConfig::AmmoBar)
                weaponPos.y += 5;
            BatchOutlinedText(previewBatch, "W", weaponPos,
                ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255),
                12.0f, font1);
        }

        if (ESPConfig::ShowIsScoped)
        {
            BatchOutlinedText(previewBatch, "s", centerPos,
                ImColor(131, 137, 150, 255), ImColor(0, 0, 0, 255),
                15.0f, font1);
        }

        // Render all batched preview elements
        RenderBatch(previewBatch, true);
    }
}
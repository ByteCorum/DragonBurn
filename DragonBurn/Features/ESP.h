#pragma once
#include "../Core/Render.h"

#include <vector>
#include <array>

namespace ESP
{
    struct WeaponIconSize
    {
        float width;
        float height;
        float offsetX;
        float offsetY;
    };

    // Pre-calculated draw data to reduce per-frame calculations
    struct DrawData
    {
        ImVec2 rectStart;
        ImVec2 rectEnd;
        ImVec2 rectCenter;
        float rectWidth;
        float rectHeight;
        ImVec2 weaponTextPos;
        ImVec2 nameTextPos;
        ImVec2 scopeIconPos;
        bool isValid;

        DrawData() : isValid(false) {}
    };

    // Batch drawing structures
    struct LineSegment
    {
        ImVec2 p1, p2;
        ImU32 color;
        float thickness;
    };

    struct RectangleData
    {
        ImVec2 start, end;
        ImU32 color;
        float rounding;
        float thickness;
        bool filled;
    };

    struct TextData
    {
        std::string text;
        ImVec2 position;
        ImU32 color;
        float fontSize;
        ImFont* font;
    };

    // Batch containers for reducing draw calls
    struct BatchData
    {
        std::vector<LineSegment> lines;
        std::vector<RectangleData> rectangles;
        std::vector<TextData> texts;

        void Clear()
        {
            lines.clear();
            rectangles.clear();
            texts.clear();
        }

        void Reserve(size_t lineCount, size_t rectCount, size_t textCount)
        {
            lines.reserve(lineCount);
            rectangles.reserve(rectCount);
            texts.reserve(textCount);
        }
    };

    // Static weapon icon sizes lookup table
    //extern const std::unordered_map<std::string, WeaponIconSize> weaponIconSizes;

    // Cached outline offsets for text
    constexpr std::array<ImVec2, 4> TEXT_OUTLINE_OFFSETS = { {
        {-1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, -1.0f}
    } };

    // Function declarations
    ImVec4 GetBoxRect(const CEntity& Entity, int BoxType);

    DrawData CalculateDrawData(const ImVec4& Rect, const CEntity& Entity);

    void BatchBoneLines(BatchData& batch, const CEntity& Entity, ImU32 color, float thickness);
    void BatchCornerBox(BatchData& batch, const ImVec4& Rect, ImU32 color, float thickness);
    void BatchOutlinedText(BatchData& batch, const std::string& text, const ImVec2& pos,
        ImU32 textColor, ImU32 outlineColor, float fontSize, ImFont* font);

    void RenderBatch(const BatchData& batch , bool Preview = false);

    void RenderPlayerESP(const CEntity& LocalEntity, const CEntity& Entity, ImVec4 Rect,
        int LocalPlayerControllerIndex, int Index, bool bIsVisible);

    void DrawPreviewBox(const ImVec2& startPos, const ImVec2& endPos, ImColor boxColor,
        float rounding, float thickness, bool filled);

    void RenderPreview(ImVec2 windowSize);
}

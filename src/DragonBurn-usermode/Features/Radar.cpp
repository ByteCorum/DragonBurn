#include "Radar.h"

Vec2 RevolveCoordinatesSystem(float RevolveAngle, Vec2 OriginPos, Vec2 DestPos)
{
    if (RevolveAngle == 0)
        return DestPos;

    // Convert angle to radians once
    float rad = RevolveAngle * (M_PI / 180.0f);
    float cosVal = cos(rad);
    float sinVal = sin(rad);

    // Cache differences
    float dx = DestPos.x - OriginPos.x;
    float dy = DestPos.y - OriginPos.y;

    Vec2 ResultPos;
    ResultPos.x = OriginPos.x + dx * cosVal + dy * sinVal;
    ResultPos.y = OriginPos.y - dx * sinVal + dy * cosVal;
    return ResultPos;
}


void Base_Radar::SetRange(const float& Range)
{
	this->RenderRange = Range;
}

void Base_Radar::SetCrossColor(const ImColor& Color)
{
	this->CrossColor = Color;
}

void Base_Radar::SetPos(const Vec2& Pos)
{
	this->Pos = Pos;
}

void Base_Radar::SetSize(const float& Size)
{
	this->Width = Size;
}

float Base_Radar::GetSize()
{
	return this->Width;
}

Vec2 Base_Radar::GetPos()
{
	return this->Pos;
}

void Base_Radar::SetProportion(const float& Proportion)
{
	this->Proportion = Proportion;
}

void Base_Radar::SetDrawList(ImDrawList* DrawList)
{
	this->DrawList = DrawList;
}

void Base_Radar::AddPoint(const Vec3& LocalPos, const float& LocalYaw, const Vec3& Pos, ImColor Color, int Type, float Yaw)
{
    this->LocalYaw = LocalYaw;

    float dx = LocalPos.x - Pos.x;
    float dy = LocalPos.y - Pos.y;
    float distance = sqrt(dx * dx + dy * dy);

    float angleRad = (LocalYaw * (M_PI / 180.0f)) - atan2(Pos.y - LocalPos.y, Pos.x - LocalPos.x);

    const float scale = (2.0f * this->RenderRange) / this->Proportion;
    distance *= scale;

    Vec2 PointPos;
    PointPos.x = this->Pos.x + distance * sin(angleRad);
    PointPos.y = this->Pos.y - distance * cos(angleRad);

    const float renderRange = this->RenderRange;
    if (PointPos.x < this->Pos.x - renderRange || PointPos.x > this->Pos.x + renderRange ||
        PointPos.y > this->Pos.y + renderRange || PointPos.y < this->Pos.y - renderRange)
    {
        return;
    }

    this->Points.push_back(std::make_tuple(PointPos, Color, Type, Yaw));
}

void DrawTriangle(Vec2 Center, ImColor Color, float Width, float Height, float Yaw)
{
	Vec2 a, b, c;
	Vec2 Re_a, Re_b, Re_c;
	a = Vec2{ Center.x - Width / 2,Center.y };
	b = Vec2{ Center.x + Width / 2,Center.y };
	c = Vec2{ Center.x,Center.y - Height };
	a = RevolveCoordinatesSystem(-Yaw, Center, a);
	b = RevolveCoordinatesSystem(-Yaw, Center, b);
	c = RevolveCoordinatesSystem(-Yaw, Center, c);
	ImGui::GetForegroundDrawList()->AddTriangleFilled(
		ImVec2(a.x, a.y),
		ImVec2(b.x, b.y),
		ImVec2(c.x, c.y),
		Color);
}

void Base_Radar::Render()
{
    if (Width <= 0)
        return;

    const Vec2 pos = this->Pos;
    const float halfWidth = this->Width * 0.5f;

    const ImVec2 crossLine1Start = Vec2(pos.x - halfWidth, pos.y).ToImVec2();
    const ImVec2 crossLine1End = Vec2(pos.x + halfWidth, pos.y).ToImVec2();
    const ImVec2 crossLine2Start = Vec2(pos.x, pos.y - halfWidth).ToImVec2();
    const ImVec2 crossLine2End = Vec2(pos.x, pos.y + halfWidth).ToImVec2();

    if (this->Opened)
    {
        if (this->ShowCrossLine)
        {
            this->DrawList->AddLine(crossLine1Start, crossLine1End, this->CrossColor, 1);
            this->DrawList->AddLine(crossLine2Start, crossLine2End, this->CrossColor, 1);
        }

        const float degToRad = static_cast<float>(M_PI) / 180.0f;

        for (const auto& pointSingle : this->Points)
        {
            Vec2 pointPos = std::get<0>(pointSingle);
            const ImColor pointColor = std::get<1>(pointSingle);
            const int pointType = std::get<2>(pointSingle);
            const float pointYaw = std::get<3>(pointSingle);
            const ImVec2 imPointPos = pointPos.ToImVec2();

            if (pointType == 0)
            {
                this->DrawList->AddCircle(imPointPos, this->CircleSize, pointColor);
                this->DrawList->AddCircleFilled(imPointPos, this->CircleSize, ImColor(0, 0, 0));
            }
            else if (pointType == 1)
            {
                const float angle = (this->LocalYaw - pointYaw) + 180.0f;
                const Vec2 rePoint = RevolveCoordinatesSystem(angle, pos, pointPos);

                const Vec2 re_a(rePoint.x, rePoint.y + this->ArrowSize);
                const Vec2 re_b(rePoint.x - this->ArrowSize / 1.5f, rePoint.y - this->ArrowSize / 2.0f);
                const Vec2 re_c(rePoint.x + this->ArrowSize / 1.5f, rePoint.y - this->ArrowSize / 2.0f);

                const Vec2 a = RevolveCoordinatesSystem(-angle, pos, re_a);
                const Vec2 b = RevolveCoordinatesSystem(-angle, pos, re_b);
                const Vec2 c = RevolveCoordinatesSystem(-angle, pos, re_c);

                this->DrawList->AddQuadFilled(
                    ImVec2(a.x, a.y),
                    ImVec2(b.x, b.y),
                    imPointPos,
                    ImVec2(c.x, c.y),
                    pointColor
                );
                this->DrawList->AddQuad(
                    ImVec2(a.x, a.y),
                    ImVec2(b.x, b.y),
                    imPointPos,
                    ImVec2(c.x, c.y),
                    ImColor(0, 0, 0, 150),
                    0.1f
                );
            }
            else
            {
                const float angle = (this->LocalYaw - pointYaw) - 90.0f;
                this->DrawList->AddCircleFilled(imPointPos, 0.85f * this->ArcArrowSize, pointColor, 30);
                this->DrawList->AddCircle(imPointPos, 0.95f * this->ArcArrowSize, ImColor(0, 0, 0, 150), 0, 0.1f);

                ImVec2 trianglePoint;
                trianglePoint.x = pointPos.x + (this->ArcArrowSize + this->ArcArrowSize / 3.0f) * cosf(-angle * degToRad);
                trianglePoint.y = pointPos.y - (this->ArcArrowSize + this->ArcArrowSize / 3.0f) * sinf(-angle * degToRad);

                ImVec2 trianglePoint1;
                trianglePoint1.x = pointPos.x + this->ArcArrowSize * cosf(-(angle - 30.0f) * degToRad);
                trianglePoint1.y = pointPos.y - this->ArcArrowSize * sinf(-(angle - 30.0f) * degToRad);

                ImVec2 trianglePoint2;
                trianglePoint2.x = pointPos.x + this->ArcArrowSize * cosf(-(angle + 30.0f) * degToRad);
                trianglePoint2.y = pointPos.y - this->ArcArrowSize * sinf(-(angle + 30.0f) * degToRad);

                this->DrawList->PathLineTo(trianglePoint);
                this->DrawList->PathLineTo(trianglePoint1);
                this->DrawList->PathLineTo(trianglePoint2);
                this->DrawList->PathFillConvex(ImColor(220, 220, 220, 240));
            }
        }
    }

    this->Points.clear();
}

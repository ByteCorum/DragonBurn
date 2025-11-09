#include "RCS.h"

void RCS::UpdateAngles(const CEntity& Local, Vec2& Angles)
{
	auto oldPunch = Vec2{ };
	auto shotsFired = Local.Pawn.ShotsFired;

	int ScreenCenterX = Gui.Window.Size.x / 2;
	int ScreenCenterY = Gui.Window.Size.y / 2;

	if (shotsFired)
	{
		uintptr_t clientState;
		auto viewAngles = Local.Pawn.ViewAngle;
		auto aimPunch = Local.Pawn.AimPunchAngle;

		auto newAngles = Vec2
		{
			viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
			viewAngles.y + oldPunch.y - aimPunch.y * 2.f,
		};

		if (newAngles.x > 89.f)
			newAngles.x = 89.f;

		if (newAngles.x < -89.f)
			newAngles.x = -89.f;

		while (newAngles.y > 180.f)
			newAngles.y -= 360.f;

		while (newAngles.y < -180.f)
			newAngles.y += 360.f;

		newAngles.x += ScreenCenterX;
		newAngles.y += ScreenCenterY;

		Angles = newAngles;
		oldPunch = aimPunch;
	}
	else
	{
		oldPunch.x = oldPunch.y = 0.f;
	}

	Vec2 PunchAngle;
	if (Local.Pawn.AimPunchCache.Count <= 0 && Local.Pawn.AimPunchCache.Count > 0xFFFF)
		return;
	if (!memoryManager.ReadMemory<Vec2>(Local.Pawn.AimPunchCache.Data + (Local.Pawn.AimPunchCache.Count - 1) * sizeof(Vec3), PunchAngle))
		return;

	Angles.x = PunchAngle.x;
	Angles.y = PunchAngle.y;
}

void RCS::RecoilControl(CEntity LocalPlayer)
{
	if (!LegitBotConfig::RCS)
		return;

	static Vec2 OldPunch = { 0, 0 };

	if (LocalPlayer.Pawn.ShotsFired > RCSBullet)
	{
		Vec2 viewAngles = LocalPlayer.Pawn.ViewAngle;
		Vec2 delta = viewAngles - (viewAngles + (OldPunch - (LocalPlayer.Pawn.AimPunchAngle * 2.f)));
		int MouseX = static_cast<int>(std::round((delta.y * RCSScale.x / LocalPlayer.Client.Sensitivity) / 0.011f));
		int MouseY = static_cast<int>(std::round((delta.x * RCSScale.y / LocalPlayer.Client.Sensitivity) / 0.011f));

		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			mouse_event(MOUSEEVENTF_MOVE, MouseX, -MouseY, NULL, NULL);
			OldPunch = LocalPlayer.Pawn.AimPunchAngle * 2.0f;
		}
	}
	else
		OldPunch = Vec2{ 0,0 };
}
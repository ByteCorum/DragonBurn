#pragma once
#include "Aimbot.h"

namespace RCS
{
	inline int RCSBullet = 1;
	inline Vec2 RCSScale = { 1.4f,1.4f };

	void UpdateAngles(const CEntity&, Vec2&);
	void RecoilControl(CEntity);
}

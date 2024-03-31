#pragma once
#include "Game.h"
#include "Entity.h"
#include "Features\Legitbot.hpp"
#include "Features/Radar.h"
#include "Features/TriggerBot.h"
#include "Features/Misc.h"

namespace Cheats
{
	void KeyCheckThread();

	void RenderCrossHair(ImDrawList* drawList) noexcept;

	void RadarSetting(Base_Radar& Radar);

	void Run();
}

struct {
	ImFont* normal15px = nullptr;
} fonts;
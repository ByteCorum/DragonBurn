#pragma once
#include "Game.h"
#include "Entity.h"
#include "Features\Legitbot.hpp"
#include "Features/Radar.h"
#include "Features/TriggerBot.h"
#include "Features/Misc.h"

namespace Cheats
{

	void Run();
}

struct {
	ImFont* normal15px = nullptr;
} fonts;
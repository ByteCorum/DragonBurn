#pragma once
#include <vector>
#include <string>
#include "crypto.h"

namespace cfg
{
	static std::string version = xorstr_("3.5.7.0");

	static std::vector<uint8_t> key = {};

	static std::vector<uint8_t> image = {};

	static std::vector<uint8_t> imageLegacy = {};
}
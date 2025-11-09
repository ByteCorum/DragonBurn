#pragma once
#include <vector>
#include <string>
#include "crypto.h"

namespace cfg
{
	static std::string kmVersion = xorstr_("2.4.7.1");
	static std::string umVersion = xorstr_("3.6.8.1");
	static std::string name    = xorstr_("DragonBurn");
	static std::string author  = xorstr_("ByteCorum");

	static std::vector<uint8_t> key = {};

	static std::vector<uint8_t> image = {};

	static std::vector<uint8_t> imageLegacy = {};
}
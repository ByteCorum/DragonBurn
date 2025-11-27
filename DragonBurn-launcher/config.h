#pragma once
#include <string>

namespace config
{
	static const std::string author = "ByteCorum";
	static const std::string productName = "DragonBurn launcher";
	static const std::string version = "1.0.0.0";

	static bool debug = false;
	static const std::string LogFile = "runtime-logs.txt";
	static const bool allowLineOperstions = false;

	static bool flag_SecureMode = false;
	static bool flag_LegacyImg = false;
	static bool flag_ForcePrefs = false;
}
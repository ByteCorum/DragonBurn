#include "config.h"
#include "cui/logger.h"
#include "win-api/win-api.h"

int main(int argc, char** argv)
{
	config::flag_SecureMode = winapi::FindExecFlag(argc, argv, "securemode");
	config::flag_LegacyImg = winapi::FindExecFlag(argc, argv, "legacyimg");
	config::flag_ForcePrefs = winapi::FindExecFlag(argc, argv, "forceprefs");


	return 0;
}
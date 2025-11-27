#include "config.h"
#include "cui/logger.h"
#include "win-api/win-api.h"

int main(int argc, char** argv)
{
	config::flag_SecureMode = winapi::FindExecFlag(argc, argv, "securemode");
	config::flag_LegacyImg = winapi::FindExecFlag(argc, argv, "legacyimg");
	config::flag_ForcePrefs = winapi::FindExecFlag(argc, argv, "forceprefs");
	config::debug = winapi::FindExecFlag(argc, argv, "debugmode");

	logger::Colored(
R"LOGO(
______                            ______                  
|  _  \                           | ___ \                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    
)LOGO", logger::Colors::LightMagenta);
	logger::Info(config::productName + " v" + config::version + " by " + config::author);
	logger::Info("https://github.com/ByteCorum/DragonBurn");
	logger::Info("https://discord.gg/5WcvdzFybD\n");

	if (config::debug)
		logger::Info("Detected debug flag");
	if (config::flag_SecureMode)
		logger::Info("Detected secure mode flag");
	if (config::flag_LegacyImg)
		logger::Info("Detected legacy image flag");
	if (config::flag_ForcePrefs)
		logger::Info("Detected force preferences flag");

	return 0;
}
//______                            ______                  
//|  _  \                           | ___ \                 
//| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
//| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
//| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
//|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
//                  __/ |                                   
//                 |___/                                    
//
//https://discord.gg/5WcvdzFybD
//https://github.com/ByteCorum/DragonBurn

#include "Core/Cheats.h"
#include "Offsets/Offsets.h"
#include "Resources/Language.h"
#include "Core/Init.h"
#include "Config/ConfigSaver.h"
#include "Helpers/Logger.h"
#include "Helpers/UIAccess.h"
#include <filesystem>
#include <KnownFolders.h>
#include <ShlObj.h>

using namespace std;

namespace fs = filesystem;
string fileName;
bool secureMode, legacyImg;

void Cheat();
bool CheckArg(const int&, char**, const std::string&);

int main(int argc, char* argv[])
{
	secureMode = CheckArg(argc, argv, "securemode");
	legacyImg = CheckArg(argc, argv, "legacyimg");

//do not use uaicess for debugging/profiling (uiacess restarts the cheat)
#ifndef DBDEBUG
	DWORD err = PrepareForUIAccess();
	if (err != ERROR_SUCCESS)
	{
		MessageBoxA(NULL, "Failed to elevate to UIAccess.", "Error", MB_OK);
		return -1;
	}
#endif

	Cheat();
	return 0;
}

bool CheckArg(const int& argc, char** argv, const std::string& value)
{
	for (size_t i = 0; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "--" + value || arg == "/" + value)
			return true;
	}
	return false;
}

void Cheat()
{
	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
	SetConsoleTitle(L"DragonBurn");
	int tryCount = 0;
	//Init::Verify::RandTitle();

	Log::Custom(R"LOGO(______                            ______                  
|  _  \                           | ___ \                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    

https://discord.gg/5WcvdzFybD
https://github.com/ByteCorum/DragonBurn


)LOGO", 13);

	if (!Init::Verify::CheckWindowVersion())
		Log::Warning("Your os is unsupported, bugs may occurred", true);

#ifndef DBDEBUG

	tryCount = 0;
CHECK_VER://CHECK_VER
	Log::Info("Checking cheat version...");
	try 
	{
		bool result = Init::Verify::CheckCheatVersion();
		Log::PreviousLine();
		if (result)
			Log::Fine("Your cheat version is up to date and supported");
		else
			Log::Error("Your cheat version is out of support");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		std::string errorMsg = error.what();
		if (errorMsg.find("bad internet connection") != std::string::npos && tryCount < 3)
		{
			Log::Error(errorMsg, false, false);
			Log::Info("Reconnecting...");
			tryCount++;
			goto CHECK_VER;//CHECK_VER
		}
		else
			Log::Error(errorMsg);
	}
#endif

	tryCount = 0;
UPDATE_OFFSETS://UPDATE_OFFSETS
	Log::Info("Updating offsets...");
	try 
	{
		Offset.UpdateOffsets();
		Log::PreviousLine();
		Log::Fine("Offsets updated");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		std::string errorMsg = error.what();
		if (errorMsg.find("bad internet connection") != std::string::npos && tryCount < 3)
		{
			Log::Error(errorMsg, false, false);
			Log::Info("Reconnecting...");
			tryCount++;
			goto UPDATE_OFFSETS;//UPDATE_OFFSETS
		}
		else
			Log::Error(errorMsg);
	}

	Log::Info("Connecting to kernel mode driver...");
	if (memoryManager.ConnectDriver(L"\\\\.\\DragonBurn-kmd"))
	{
		Log::PreviousLine();
		Log::Fine("Successfully connected to kernel mode driver");
	}
	else
	{
		Log::PreviousLine();
		Log::Error("Failed to connect to kernel mode driver", false, false);
		Log::Info("Triggered auto-map protocol");
		Log::Info("Looking for kernel mapper...");

		if (fs::exists("DragonBurn-kernel.exe")) 
		{
			Log::PreviousLine();
			std::string mapperInfo = "Executing kernel mapper, flags: "
				+ std::string(secureMode ? "--securemode" : "")
				+ std::string(legacyImg ? "--legacyimg" : "")
				+ std::string("...");
			Log::Info(mapperInfo);
			int result = Init::Verify::ExecuteMapper(secureMode, legacyImg);

			Log::PreviousLine();
			if (result == 0)
				Log::Fine("Successfully mapped kernel mode driver");
			else
				Log::Error("Failed to map kernel mode driver");
		}
		else
		{
			Log::PreviousLine();
			Log::Warning("It might have been deleted by AV, turn off AV and clean temp");
			Log::Error("Failed to find kernel mapper");
		}
	}

	Log::Info("Waiting for CS2...");
	bool preStart = false;
	while (memoryManager.GetProcessID(L"cs2.exe") == 0)
	{
		preStart = true;
		Sleep(500);
	}
	if (preStart)
	{
		Log::PreviousLine();
		Log::Info("Connecting to CS2(it may take some time)...");
		Sleep(23000);
	}

	Log::PreviousLine();
	Log::Fine("Connected to CS2");
	Log::Info("Linking to CS2...");

#ifndef DBDEBUG
	try 
	{
		if (!Init::Client::CheckCS2Version()) 
		{
			Log::PreviousLine();
			Log::Warning("Offsets are outdated, we'll update them asap. With current offsets, cheat may work unstable", true);
		}
	}
	catch(const std::exception& error)
	{
		Log::PreviousLine();
		Log::Error(error.what());
	}
#endif

	if (!memoryManager.Attach(memoryManager.GetProcessID(L"cs2.exe")))
	{
		Log::PreviousLine();
		Log::Error("Failed to attach to the process");
	}

	if (!gGame.InitAddress())
	{
		Log::PreviousLine();
		Log::Error("Failed to Init Address");
	}

	g_globalVars = std::make_unique<globalvars>();
	if (!g_globalVars->UpdateGlobalvars()) {
		Log::PreviousLine();
		Log::Error("Offsets are outdated");
	}

	Log::PreviousLine();
	Log::Fine("Linked to CS2");

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK)
		Log::Error("Failed to get the Documents folder path");

	MenuConfig::path = documentsPath;
	MenuConfig::docPath = documentsPath;
	MenuConfig::path += "\\DragonBurn";
	try
	{
		if (fs::exists(MenuConfig::docPath + "\\Adobe Software Data"))
			fs::rename(MenuConfig::docPath + "\\Adobe Software Data", MenuConfig::path);
		if (fs::exists(MenuConfig::path))
			Log::Fine("Config folder connected: " + MenuConfig::path);
		else
		{
			if (fs::create_directory(MenuConfig::path))
				Log::Fine("Config folder connected: " + MenuConfig::path);
			else
				Log::Error("Failed to create the config directory");
		}
		if (fs::exists(MenuConfig::path + "\\default.cfg"))
			MenuConfig::defaultConfig = true;
	}
	catch (const std::exception& error)
	{
		Log::Error(error.what());
	}

	Misc::Layout = Misc::DetectKeyboardLayout();

	Log::Fine("DragonBurn loaded");


#ifndef DBDEBUG
	Sleep(3000);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	try
	{
		Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run);
	}
	catch (std::exception& error)
	{
		Log::Error(error.what());
	}
}

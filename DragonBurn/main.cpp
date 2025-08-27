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
#include "Resources/Language.hpp"
#include "Core/Init.h"
#include "Config/ConfigSaver.h"
#include "Helpers/Logger.h"
#include "Helpers/UIAccess.h"
#include <filesystem>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <format>

using namespace std;

namespace fs = filesystem;
string fileName;

void Cheat();

int main()
{

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
}

void Cheat()
{
	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
	SetConsoleTitle(L"DragonBurn");
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
	{
		Log::Warning("Your os is unsupported, bugs may occurred", true);
	}

#ifndef DBDEBUG
	Log::Info("Checking cheat version");
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
		Log::Error(format("Error: {}", error.what()));
	}

#endif

	Log::Info("Updating offsets");
	try 
	{
		Offset.UpdateOffsets();
		Log::PreviousLine();
		Log::Fine("Offsets updated");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		Log::Error(format("Error: {}",error.what()));
	}

	Log::Info("Connecting to kernel mode driver");
	if (memoryManager.ConnectDriver(L"\\\\.\\DragonBurn-kmd"))
	{
		Log::PreviousLine();
		Log::Fine("Successfully connected to kernel mode driver");
	}
	else
	{
		Log::PreviousLine();
		Log::Warning("first, run DragonBurn-kernel.exe/ Once.");
		Log::Error("Failed to connect to kernel mode driver");
	}

	std::cout << '\n';
	bool preStart = false;
	while (memoryManager.GetProcessID(L"cs2.exe") == 0)
	{
		Log::PreviousLine();
		Log::Info("Waiting for CS2");
		preStart = true;
	}

	if (preStart)
	{
		Log::PreviousLine();
		Log::Info("Connecting to CS2(it may take some time)");
		Sleep(23000);
	}

	Log::PreviousLine();
	Log::Fine("Connected to CS2");
	Log::Info("Linking to CS2");

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
		Log::Error(format("Error: {}", error.what()));
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

	Log::PreviousLine();
	Log::Fine("Linked to CS2");

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK)
		Log::Error("Failed to get the Documents folder path");

	MenuConfig::path = documentsPath;
	MenuConfig::docPath = documentsPath;
	MenuConfig::path += "\\DragonBurn";

	if (fs::exists(MenuConfig::docPath + "\\Adobe Software Data"))
	{
		fs::rename(MenuConfig::docPath + "\\Adobe Software Data", MenuConfig::path);
	}

	if (fs::exists(MenuConfig::path))
	{
		Log::Fine("Config folder connected: " + MenuConfig::path);
	}
	else
	{
		if (fs::create_directory(MenuConfig::path))
		{
			Log::Fine("Config folder connected: " + MenuConfig::path);
		}
		else
		{
			Log::Error("Failed to create the config directory");
		}
	}

	if (fs::exists(MenuConfig::path + "\\default.cfg"))
		MenuConfig::defaultConfig = true;

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

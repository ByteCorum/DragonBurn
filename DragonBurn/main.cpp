//______                            ______                  
//|  _  \\                           | ___ \\                 
//| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
//| | | | '__/ _` |/ _` |/ _ \\| '_ \\| ___ \\ | | | '__| '_ \\ 
//| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
//|___/ |_|  \\__,_|\\__, |\\___/|_| |_\\____/ \\__,_|_|  |_| |_|
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
|  _  \\                           | ___ \\                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \\| '_ \\| ___ \\ | | | '__| '_ \\ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \\__,_|\\__, |\\___/|_| |_\\____/ \\__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    

https://discord.gg/5WcvdzFybD
https://github.com/ByteCorum/DragonBurn


)LOGO", 13);

	if (!Init::Verify::CheckWindowVersion())
		Log::Warning("Your os is unsupported, bugs may occurred", true);

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK)
		Log::Error("Failed to get the Documents folder path");

	MenuConfig::docPath = documentsPath;
	MenuConfig::path = MenuConfig::docPath + "\\DragonBurn";
	try
	{
		if (fs::exists(MenuConfig::docPath + "\\Adobe Software Data"))
			fs::rename(MenuConfig::docPath + "\\Adobe Software Data", MenuConfig::path);

		try
		{
			Log::Fine("Config folder connected: " + MenuConfig::path);
		}
		catch (std::exception error)
		{
			Log::Error(error.what());
		}

		if (fs::exists(MenuConfig::path + "\\default.cfg"))
			MenuConfig::defaultConfig = true;

		Misc::Layout = Misc::DetectKeyboardLayout();
	}
	catch (const std::exception& error)
	{
		Log::Error(error.what());
	}

	Log::Fine("DragonBurn loaded");

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
		Log::Error(error.what());
	}

	Log::Info("Initializing stealth memory manager");
	if (memoryManager.ConnectDriver(L"usermode")) // Compatibility call
	{
		Log::PreviousLine();
		Log::Fine("Successfully initialized stealth memory manager");
	}
	else
	{
		Log::PreviousLine();
		Log::Error("Failed to initialize stealth memory manager");
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

	if (!memoryManager.Attach(memoryManager.GetProcessID(L"cs2.exe")))
	{
		Log::PreviousLine();
		Log::Error("Failed to establish stealth connection");
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

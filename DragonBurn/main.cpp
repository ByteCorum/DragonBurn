﻿#define _CRT_SECURE_NO_WARNINGS
#include "Cheats.h"
#include "Offsets.h"
#include "Resources/Language.h"
#include "Utils/Initial/Init.h"
#include "Utils/ConfigSaver.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <stdio.h>

using namespace std;

namespace fs = filesystem;
string fileName;

void Exit()
{
	system("pause");
	exit(0);
}


void Cheat()
{
	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
	srand(time(0));
	Init::Verify::RandTitle();
	if (Init::Verify::CheckWindowVersion())
	{
		MenuConfig::MaxFrameRate = Init::Client::getMaxFrameRate();
	}
		
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 13);	
	cout << R"LOGO(______                            ______                  
|  _  \                           | ___ \                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    )LOGO" << endl << endl << endl;

	bool show = false;
	while (ProcessMgr.GetProcessID("cs2.exe") == 0)
	{
		if (!show) 
		{
			SetConsoleTextAttribute(hConsole, 14);
			cout << "[INFO] Waiting for CS2." << endl;
			show = true;
		}
	} 
	if (show)
	{
		SetConsoleTextAttribute(hConsole, 14);
		cout << "[INFO] Connecting to CS2." << endl;
		Sleep(20000);
	}


	auto ProcessStatus = ProcessMgr.Attach("cs2.exe");
	switch (ProcessStatus) 
	{
	case 1:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Game not found." << endl;
		Exit();
		break;
	case 2:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Failed to hook process, please run the cheat as Administrator (Right click DragonBurn > Run as Adminstrator)." << endl;
		Exit();
		break;
	case 3:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Failed to get module address." << endl;
		Exit();
		break;
	default:
		break;
	}
	

	switch (Init::Client::CheckCS2Version()) 
	{
	case 0:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Failed to get the current game version." << endl;
		system("pause");
		break;

	case 1:
		SetConsoleTextAttribute(hConsole, 14);
		cout << "[Info] This version of the cheat was made for a later version of the game and may work unstable on the current version." << endl;
		system("pause");
		break;

	case 2:
		break;

	default:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Failed to get the current game version." << endl;
		system("pause");
		break;
	}

	if (!gGame.InitAddress())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "[ERROR] Failed to call InitAddress()." << endl;
		Exit();
	}

	SetConsoleTextAttribute(hConsole, 11);
	cout << "[Game] Process ID: " << ProcessMgr.ProcessID << endl;
	cout << "[Game] Client Address: " << gGame.GetClientDLLAddress() << endl;

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK) {
		SetConsoleTextAttribute(hConsole, 12);
		cerr << "[ERROR]Failed to get the Documents folder path." << endl;
		Exit();
	}
	MenuConfig::path = documentsPath;
	MenuConfig::path += "\\DragonBurn";

	if (fs::exists(MenuConfig::path))
	{
		cout << "[Info] Config folder connected: " << MenuConfig::path << endl;
	}
	else
	{
		if (fs::create_directory(MenuConfig::path))
		{
			cout << "[Info] Config folder created: " << MenuConfig::path << endl;
		}
		else
		{
			cerr << "[Info] Error: Failed to create the config directory." << endl;
			Exit();
		}
	}

	ifstream defaultConfig(MenuConfig::path + "\\default.cfg");
	if (defaultConfig.is_open())
	{
		MenuConfig::defaultConfig = true;
		defaultConfig.close();		
	}

	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	cout << "DragonBurn loaded!!!\nGLHF" << endl;
	Sleep(3000);
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	try
	{
		Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run);
	}
	catch (OSImGui::OSException& e)
	{
		cout << e.what() << endl;
	}
}

int main()
{
	Cheat();
}
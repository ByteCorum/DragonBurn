#pragma once
#include <fstream>
#include <tchar.h>
#include <shellapi.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <psapi.h>
#include <stdexcept>
#include "../Offsets/Offsets.h"
#include "../Helpers/WebApi.h"
#include "../Core/Config.h"

inline bool keyWasPressed = false;

inline std::string WStringToString(const std::wstring& wstr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::vector<char> buffer(bufferSize);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.data(), bufferSize, NULL, NULL);
    return std::string(buffer.data());
}

namespace Init
{
    using namespace std;

	class Verify
	{
	public:
		// Check if the Windows version is higher than 7
		static bool CheckWindowVersion() {
            OSVERSIONINFOEX osvi;
            ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
            osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
            osvi.dwMajorVersion = 6;
            osvi.dwMinorVersion = 1;

            ULONGLONG conditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
            conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

            if (VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
            {
                return true;
            }
            return false;
		}

        //static void RandTitle()
        //{
        //    srand(time(0));
        //    constexpr int length = 25;
        //    const auto characters = TEXT("0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM");
        //    TCHAR title[length + 1]{};

        //    for (int j = 0; j < length; j++)
        //    {
        //        title[j] += characters[rand() % 63];
        //    }

        //    SetConsoleTitle(title);
        //}

        static bool CheckCheatVersion()
        {
            std::string supportedVersions;
            Web::Get("https://raw.githubusercontent.com/ByteCorum/DragonBurn/data/version", supportedVersions);

            if (supportedVersions.find(MenuConfig::version) != std::string::npos)
                return true;
            return false;
        }
	};

    class Client
    {
    public:
        static bool CheckCS2Version()
        {
            std::string supportedVersion;
            Web::Get("https://raw.githubusercontent.com/ByteCorum/DragonBurn/data/cs2-version", supportedVersion);
            if (supportedVersion == "-1")
                return true;

            //getting processPath
            std::string processPath;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, memoryManager.GetProcessID(L"cs2.exe"));
            if (hProcess) 
            {
                wchar_t buffer[MAX_PATH];
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageName(hProcess, 0, buffer, &size))
                {
                    CloseHandle(hProcess);
                    processPath = WStringToString(buffer);
                }
                else
                {
                    CloseHandle(hProcess);
                    throw std::runtime_error("failed to get process path");
                }
            }
            else 
                throw std::runtime_error("failed to open process");

            // get path to built_from_cl.txt
            int pos = processPath.rfind("bin");
            if (pos != std::string::npos) 
                processPath = processPath.substr(0, pos + 3) + "\\built_from_cl.txt";
            else
                throw std::runtime_error("failed to find version file");

            //reading file
            std::ifstream file(processPath);
            if (file.is_open()) 
            {
                std::string gameVersion;
                std::getline(file, gameVersion);
                file.close();

                if (supportedVersion == gameVersion)
                    return true;
                else
                    return false;
            }
            else
                throw std::runtime_error("failed to get game version");
        }

        // Check if the game window is activated
        static bool isGameWindowActive() {
            HWND hwnd_cs2 = FindWindow(NULL, TEXT("Counter-Strike 2"));

            if (hwnd_cs2 != NULL) {
                HWND foreground_window = GetForegroundWindow();
                if (foreground_window == hwnd_cs2) {
                    return true;
                }
            }
            return false;
        }

        static void Exit()
        {
            exit(0);
        }
    };

}


#pragma once
#include <fstream>
#include <tchar.h>
#include <shellapi.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <psapi.h>
#include <stdexcept>
#include <format>
#include "../Offsets/Offsets.h"
#include "../Helpers/WebApi.h"
#include "../Helpers/StorageMgr.h"

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
            std::vector<std::string> versions;
            try
            {
                json localVer = json::parse(storage::ReadStorageFile("versions.json"));

                if (!localVer.contains("last-access-time") || !localVer.contains("usermode-ver") || localVer["usermode-ver"].is_null() || !localVer["usermode-ver"].is_array())
                    throw std::runtime_error("Invalid json data");

                std::stringstream ss(localVer["last-access-time"].get<std::string>());
                std::chrono::system_clock::time_point lastAccessTime;
                std::chrono::from_stream(ss, "%Y-%m-%d %H:%M:%S", lastAccessTime);

                if (std::chrono::system_clock::now() - lastAccessTime > std::chrono::minutes(10))
                    throw std::runtime_error("Version data is outdated");

                for (const auto& version : localVer["usermode-ver"])
                    versions.push_back(version.get<std::string>());
            }
            catch (std::exception error)
            {
                json cloudVer = json::parse(Web::Get("https://api.jsonbin.io/v3/b/690e4759ae596e708f4b20b3"))["record"];

                if (!cloudVer.contains("usermode-ver") || cloudVer["usermode-ver"].is_null() || !cloudVer["usermode-ver"].is_array())
                    throw std::runtime_error("Invalid json data");

                for (const auto& version : cloudVer["usermode-ver"])
                    versions.push_back(version.get<std::string>());

                auto now = std::chrono::system_clock::now();
                cloudVer["last-access-time"] = std::format("{:%Y-%m-%d %H:%M:%S}", now);

                storage::WriteStorageFile("versions.json", cloudVer.dump(4));
            }

            if (std::find(versions.begin(), versions.end(), MenuConfig::version) != versions.end())
                return true;
            return false;
        }

        static int ExecuteMapper(bool secureMode, bool legacyImg, bool forceprefs)
        {
            STARTUPINFOW si = { sizeof(STARTUPINFOW) };
            PROCESS_INFORMATION pi = {};

            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;

            std::wstring cmdLine = L"DragonBurn-kernel.exe" 
                + std::wstring(secureMode ? L" --securemode" : L"")
                + std::wstring(legacyImg ? L" --legacyimg" : L"")
                + std::wstring(forceprefs ? L" --forceprefs" : L"");
            BOOL success = CreateProcessW(
                nullptr,                   // Application name
                &cmdLine[0],               // Command line (must be modifiable)
                nullptr,                   // Process security attributes
                nullptr,                   // Thread security attributes
                FALSE,                     // Inherit handles
                CREATE_NEW_CONSOLE,        // Creation flags - creates new console
                nullptr,                   // Environment
                nullptr,                   // Current directory
                &si,                       // Startup info
                &pi                        // Process info
            );

            int result = -1;
            if (success)
            {
                WaitForSingleObject(pi.hProcess, INFINITE);

                DWORD exitCode;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                result = static_cast<int>(exitCode);

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }

            return result;
        }
	};

    class Client
    {
    public:
        //static std::string GetCs2Version(int pid)
        //{
        //    std::wstring processPath;
        //    WCHAR filename[MAX_PATH];

        //    HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        //    if (processHandle != NULL) {
        //        if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0)
        //            throw std::runtime_error("failed to get process path");
        //        else
        //            processPath = filename;
        //        CloseHandle(processHandle);
        //    }
        //    else
        //        throw std::runtime_error("failed to open process");

        //    int pos = processPath.rfind(L"bin");
        //    if (pos != std::wstring::npos) 
        //        processPath = processPath.substr(0, pos + 3) + L"\\built_from_cl.txt";
        //    else
        //        throw std::runtime_error("failed to find version file");

        //    std::string gameVersion;
        //    std::ifstream file(processPath);
        //    if (file.is_open()) 
        //    {
        //        std::getline(file, gameVersion);
        //        file.close();
        //    }
        //    else
        //        throw std::runtime_error("failed to get game version");

        //    return gameVersion;
        //}

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


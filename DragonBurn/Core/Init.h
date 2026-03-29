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

        static int ExecuteMapper(bool secureMode = false, bool legacyImg = false)
        {
            STARTUPINFOW si = { sizeof(STARTUPINFOW) };
            PROCESS_INFORMATION pi = {};

            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;

            std::wstring cmdLine = L"DragonBurn-kernel.exe"
                + std::wstring(secureMode ? L" --securemode" : L"")
                + std::wstring(legacyImg ? L" --legacyimg" : L"");
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
        static std::string GetCs2Version(int pid)
        {
            std::wstring processPath;
            WCHAR filename[MAX_PATH];

            HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (processHandle != NULL) {
                if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0)
                    throw std::runtime_error("failed to get process path");
                else
                    processPath = filename;
                CloseHandle(processHandle);
            }
            else
                throw std::runtime_error("failed to open process");

            int pos = processPath.rfind(L"bin");
            if (pos != std::wstring::npos) 
                processPath = processPath.substr(0, pos + 3) + L"\\built_from_cl.txt";
            else
                throw std::runtime_error("failed to find version file");

            std::string gameVersion;
            std::ifstream file(processPath);
            if (file.is_open()) 
            {
                std::getline(file, gameVersion);
                file.close();
            }
            else
                throw std::runtime_error("failed to get game version");

            return gameVersion;
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


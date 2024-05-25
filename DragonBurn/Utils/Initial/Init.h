#pragma once
#include <fstream>
#include <tchar.h>
#include <shellapi.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <psapi.h>
#include "../../Offsets.h"

inline std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
inline bool keyWasPressed = false;

inline std::string WStringToString(const std::wstring& wstr) {
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

        static void RandTitle()
        {
            constexpr int length = 25;
            const auto characters = TEXT("0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM");
            TCHAR title[length + 1]{};

            for (int j = 0; j != length; j++)
            {
                title[j] += characters[rand() % 63];
            }

            SetConsoleTitle(title);
        }
	};

    class Client
    {
    public:
        // Get the maximum framerate with vsync
        static int getMaxFrameRate() {
            HDC hdc = GetDC(NULL);
            int rate = GetDeviceCaps(hdc, VREFRESH);
            ReleaseDC(NULL, hdc);
            return rate;
        }

        // Checks cs2 version
        static int CheckCS2Version()
        {
            DWORD pid = ProcessMgr.GetProcessID("cs2.exe");
            long long curVer = -1;
            std::string processPath;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (hProcess) 
            {
                wchar_t buffer[MAX_PATH];
                if (GetModuleFileNameEx(hProcess, NULL, buffer, MAX_PATH))
                    processPath = WStringToString(buffer);
                else 
                    return 0;
                CloseHandle(hProcess);
            }
            else 
                return 0;

            int pos = processPath.rfind("bin");
            if (pos != std::string::npos) 
                processPath = processPath.substr(0, pos + 3);
            else
                return 0;
            processPath += "\\built_from_cl.txt";

            std::ifstream file(processPath);
            if (file.is_open()) 
            {
                std::string line;
                if (std::getline(file, line))
                    curVer = stoi(line);
                else
                    return 0;
                file.close();
            }
            else
                return 0;

            if (Offset::CS2ver == curVer)
                return 2;
            else
                return 1;
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


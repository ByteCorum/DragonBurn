#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <TlHelp32.h>
#include "json.hpp"

#include "kdmapper.h"
#include "utils.h"
#include "intel_driver.h"
#include "cfg.h"
#include "web_api.h"
#include "logger.h"

using json = nlohmann::json;

LONG WINAPI SimplestCrashHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
	{
		std::ostringstream oss;
		oss << "Crash at addr 0x" << ExceptionInfo->ExceptionRecord->ExceptionAddress << L" by 0x" << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode;
		Log::Error(oss.str(), false);
	}
	else
		Log::Error("Program crashed", false);

	if (intel_driver::hDevice)
		intel_driver::Unload();

	return EXCEPTION_EXECUTE_HANDLER;
}

bool callbackExample(ULONG64* param1, ULONG64* param2, ULONG64 allocationPtr, ULONG64 allocationSize) {
	UNREFERENCED_PARAMETER(param1);
	UNREFERENCED_PARAMETER(param2);
	UNREFERENCED_PARAMETER(allocationPtr);
	UNREFERENCED_PARAMETER(allocationSize);
	Log::Fine("Driver callback called");
	
	/*
	This callback occurs before call driver entry and
	can be useful to pass more customized params in 
	the last step of the mapping procedure since you 
	know now the mapping address and other things
	*/
	return true;
}

DWORD getParentProcess()
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	DWORD ppid = 0, pid = GetCurrentProcessId();

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	__try {
		if (hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == 0) __leave;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);
		if (!Process32First(hSnapshot, &pe32)) __leave;

		do {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));

	}
	__finally {
		if (hSnapshot != INVALID_HANDLE_VALUE && hSnapshot != 0) CloseHandle(hSnapshot);
	}
	return ppid;
}

bool CheckArg(const int argc, wchar_t** argv, const wchar_t* arg) 
{
	size_t plen = wcslen(arg);

	for (int i = 1; i < argc; i++) 
	{
		if (wcslen(argv[i]) == plen + 1ull && _wcsicmp(&argv[i][1], arg) == 0 && argv[i][0] == '/')
			return true;
		else if (wcslen(argv[i]) == plen + 2ull && _wcsicmp(&argv[i][2], arg) == 0 && argv[i][0] == '-' && argv[i][1] == '-')
			return true;
	}
	return false;
}

bool CheckCheatVersion()
{
	std::vector<std::string> versions;

	std::string versionData;
	Web::Get("https://api.jsonbin.io/v3/b/690e4759ae596e708f4b20b3", versionData);
	json versionJson = json::parse(versionData)["record"];

	if (!versionJson.contains("kernel-ver") || versionJson["kernel-ver"].is_null() || !versionJson["kernel-ver"].is_array())
		throw std::runtime_error("Invalid json data");

	for (const auto& version : versionJson["kernel-ver"])
		versions.push_back(version.get<std::string>());

	if (std::find(versions.begin(), versions.end(), cfg::kmVersion) != versions.end())
		return true;
	return false;
}

bool IsDriverRunning(const LPCWSTR name)
{
	HANDLE kernelDriver = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (kernelDriver == INVALID_HANDLE_VALUE)
        return false;

	BOOL result = CloseHandle(kernelDriver);
    return true;
}

int wmain(const int argc, wchar_t** argv)
{
	SetUnhandledExceptionFilter(SimplestCrashHandler);

	Log::Custom(R"LOGO(______                            ______                  
|  _  \                           | ___ \                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    
)LOGO", 13);
	Log::Info(cfg::name + " v" + cfg::umVersion + " & v" + cfg::kmVersion + " by " + cfg::author);
	Log::Info("https://github.com/ByteCorum/DragonBurn");
	Log::Info("https://discord.gg/5WcvdzFybD\n");

	//free --> Automatically frees mapped memory after execution. Dangerous unless the driver finishes instantly
	//indPagesMode --> Maps the driver into non-contiguous, separate memory pages. Better for stealth, but more complex
	//copyHeader --> Ennsures the PE headers are copied into memory	Needed for drivers that inspect their own image
	//passAllocationPtr --> Passes allocated memory pointer as first param to entry point. Used by custom loaders or shellcode-style drivers
	// Can't use --free and --indPages at the same time"
	bool free = false;
	bool indPagesMode = CheckArg(argc, argv, L"securemode");
	bool legacyImg = CheckArg(argc, argv, L"legacyimg");
	bool copyHeader = false;
	bool passAllocationPtr = false;

	if (legacyImg)
	{
		Log::Info("Enabled: Legacy DragonBurn kernel");
		Log::Warning("Legacy DragonBurn kernel is deprecated, it's better to use new one");
	}
	if (indPagesMode)
		Log::Info("Enabled: Secure mapping and execution mode");

	if (IsDriverRunning(L"\\\\.\\DragonBurn-kmd"))
		Log::Error("Kernel mode driver is already mapped");

#ifndef _DEBUG
	int tryCount = 0;
CHECK_VER://CHECK_VER
	Log::Info("Checking mapper version...");
	try
	{
		bool result = CheckCheatVersion();
		Log::PreviousLine();
		if (result)
			Log::Fine("Your mapper version is up to date and supported");
		else
			Log::Error("Your mapper version is out of support");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		std::string errorMsg = error.what();
		if (errorMsg.find("bad internet connection") != std::string::npos && tryCount < 3)
		{
			Log::Error(errorMsg, false);
			Log::Info("Reconnecting...");
			tryCount++;
			goto CHECK_VER;//CHECK_VER
		}
		else
			Log::Error(errorMsg);
	}
#endif

	BYTE* img = nullptr;
	if (!legacyImg)
	{
		if (cfg::image.empty())
			Log::Error("Driver image is empty");
		RollingVectorProcedure(cfg::image, cfg::key);
		img = cfg::image.data();
	}
	else
	{
		if (cfg::imageLegacy.empty())
			Log::Error("Driver image is empty");
		RollingVectorProcedure(cfg::imageLegacy, cfg::key);
		img = cfg::imageLegacy.data();
	}

	if (!NT_SUCCESS(intel_driver::Load()))
		Log::Error("Failed to connect to intel driver");

	kdmapper::AllocationMode mode = kdmapper::AllocationMode::AllocatePool;
	if (indPagesMode)
		mode = kdmapper::AllocationMode::AllocateIndependentPages;

	NTSTATUS exitCode = 0;
	if (!kdmapper::MapDriver(img, 0, 0, free, !copyHeader, mode, passAllocationPtr, callbackExample, &exitCode))
	{
		intel_driver::Unload();
		Log::Error("Failed to map DragonBurn driver");
	}

	if (!NT_SUCCESS(intel_driver::Unload()))
		Log::Warning("Warning failed to unload intel driver", true);

	Log::Fine("DragonBurn driver mapped successfully");
	system("pause");
	return 0;
}
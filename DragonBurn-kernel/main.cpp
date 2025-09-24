#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <TlHelp32.h>

#include "kdmapper.hpp"
#include "utils.hpp"
#include "intel_driver.hpp"
#include "cfg.hpp"
#include "web_api.hpp"
#include "logger.hpp"

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

int wmain(const int argc, wchar_t** argv)
{
	SetUnhandledExceptionFilter(SimplestCrashHandler);

	printf(R"LOGO(______                            ______                  
|  _  \                           | ___ \                 
| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
                  __/ |                                   
                 |___/                                    

https://discord.gg/5WcvdzFybD
https://github.com/ByteCorum/DragonBurn

)LOGO");

	//free --> Automatically frees mapped memory after execution. Dangerous unless the driver finishes instantly
	//indPagesMode --> Maps the driver into non-contiguous, separate memory pages. Better for stealth, but more complex
	//copyHeader --> Ennsures the PE headers are copied into memory	Needed for drivers that inspect their own image
	//passAllocationPtr --> Passes allocated memory pointer as first param to entry point. Used by custom loaders or shellcode-style drivers
	// Can't use --free and --indPages at the same time"
	bool free = false;
	bool indPagesMode = !CheckArg(argc, argv, L"legacymethod");
	bool legacyImg = !CheckArg(argc, argv, L"legacyimg");
	bool copyHeader = false;
	bool passAllocationPtr = false;

	if (legacyImg)
	{
		Log::Info("Using legacy DragonBurn kernel mode driver ");
		Log::Warning("Legacy DragonBurn kernel mode driver is deprecated, it's better to use new one");
	}
	if (!indPagesMode)
	{
		Log::Info("Using legacy kernel mode driver mapping algorithm");
		Log::Warning("Legacy kernel mode driver mapping algorithm is deprecated, it's better to use new one");
	}

#ifndef _DEBUG
	const std::string curVersionUrl = "https://raw.githubusercontent.com/ByteCorum/DragonBurn/data/version";
	std::string supportedVersions;
	Log::Info("Checking mapper version...");
	try
	{
		Web::Get(curVersionUrl, supportedVersions);
		if (supportedVersions.find(cfg::version) != std::string::npos)
		{
			Log::PreviousLine();
			Log::Fine("Your mapper version is up to date and supported");
		}
		else
			Log::Error("Your mapper version is out of support");
	}
	catch (const std::runtime_error& error){Log::Error(error.what());}
#endif

	if (!intel_driver::Load())
		Log::Error("Failed to connect to intel driver");

	kdmapper::AllocationMode mode = kdmapper::AllocationMode::AllocatePool;
	if (indPagesMode)
		mode = kdmapper::AllocationMode::AllocateIndependentPages;

	NTSTATUS exitCode = 0;
	BYTE* img = nullptr;
	if (!legacyImg)
	{
		RollingVectorProcedure(cfg::image, cfg::key);
		img = cfg::image.data();
	}
	else
	{
		RollingVectorProcedure(cfg::imageLegacy, cfg::key);
		img = cfg::imageLegacy.data();
	}

	if (!kdmapper::MapDriver(img, 0, 0, free, !copyHeader, mode, passAllocationPtr, callbackExample, &exitCode))
	{
		intel_driver::Unload();
		Log::Error("Failed to map DragonBurn driver");
	}

	if (!intel_driver::Unload())
		Log::Warning("Warning failed to unload intel driver", true);

	Log::Fine("DragonBurn driver mapped successfully");
	system("pause");
	return 0;
}
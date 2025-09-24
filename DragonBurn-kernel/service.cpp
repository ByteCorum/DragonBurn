#include "service.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include "logger.h"
#include "utils.h"
#include "nt.h"

bool service::RegisterAndStart(const std::wstring& driver_path, const std::wstring& serviceName) {
	std::ostringstream ss;
	const static DWORD ServiceTypeKernel = 1;
	const std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
	const std::wstring nPath = L"\\??\\" + driver_path;

	HKEY dservice;
	LSTATUS status = RegCreateKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &dservice); //Returns Ok if already exists
	if (status != ERROR_SUCCESS) {
		Log::Error("Can't create service key", false);
		return false;
	}

	status = RegSetKeyValueW(dservice, NULL, L"ImagePath", REG_EXPAND_SZ, nPath.c_str(), (DWORD)(nPath.size()*sizeof(wchar_t)));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		Log::Error("Can't create 'ImagePath' registry value", false);
		return false;
	}

	status = RegSetKeyValueW(dservice, NULL, L"Type", REG_DWORD, &ServiceTypeKernel, sizeof(DWORD));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		Log::Error("Can't create 'Type' registry value", false);
		return false;
	}

	RegCloseKey(dservice);

	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL) {
		return false;
	}

	//auto RtlAdjustPrivilege = (nt::RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
	//auto NtLoadDriver = (nt::NtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver");

	ULONG SE_LOAD_DRIVER_PRIVILEGE = 10UL;
	BOOLEAN SeLoadDriverWasEnabled;
	NTSTATUS Status = nt::RtlAdjustPrivilege(SE_LOAD_DRIVER_PRIVILEGE, TRUE, FALSE, &SeLoadDriverWasEnabled);
	if (!NT_SUCCESS(Status)) {
		Log::Error("Failed to acquire SE_LOAD_DRIVER_PRIVILEGE. Make sure you are running as administrator.", false);
		return false;
	}

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + serviceName;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	Status = nt::NtLoadDriver(&serviceStr);

	ss << "NtLoadDriver Status 0x" << std::hex << Status;
	Log::Fine(ss.str());
	ss.str("");

	if (Status == STATUS_IMAGE_CERT_REVOKED)
		Log::Error("Your vulnerable driver list is enabled and have blocked the driver loading, you must disable vulnerable driver list to use kdmapper with intel driver\n>>>Registry path to disable vulnerable driver list: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\CI\\Config\n>>>Set 'VulnerableDriverBlocklistEnable' as dword to 0", false);

	else if (Status == STATUS_ACCESS_DENIED || Status == STATUS_INSUFFICIENT_RESOURCES)
	{
		ss << "Access Denied or Insufficient Resources (0x" << std::hex << Status << "), Probably some anticheat or antivirus running blocking the load of vulnerable driver";
		Log::Error(ss.str(), false);
		ss.str("");
	}

	//Never should occur since kdmapper checks for "IsRunning" driver before
	if (Status == STATUS_IMAGE_ALREADY_LOADED)
		return true;

	return NT_SUCCESS(Status);
}

bool service::StopAndRemove(const std::wstring& serviceName)
{
	std::ostringstream ss;
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL)
		return false;

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + serviceName;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	HKEY driver_service;
	std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
	LSTATUS status = RegOpenKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &driver_service);
	if (status != ERROR_SUCCESS) {
		if (status == ERROR_FILE_NOT_FOUND) {
			return true;
		}
		return false;
	}
	RegCloseKey(driver_service);

	NTSTATUS st = nt::NtUnloadDriver(&serviceStr);
	ss << "NtUnloadDriver Status 0x" << std::hex << st;
	Log::Fine(ss.str());
	ss.str("");

	if (st != ERROR_SUCCESS) {
		Log::Error("Driver Unload Failed!!", false);
		status = RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		return false; //lets consider unload fail as error because can cause problems with anti cheats later
	}

	status = RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
	if (status != ERROR_SUCCESS) {
		return false;
	}
	return true;
}

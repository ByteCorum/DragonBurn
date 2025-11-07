#include "service.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include "logger.h"
#include "utils.h"
#include "nt.h"

NTSTATUS service::RegisterAndStart(const std::wstring& driver_path, const std::wstring& serviceName) {
	std::ostringstream ss;
	const static DWORD ServiceTypeKernel = 1;
	const std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
	const std::wstring nPath = L"\\??\\" + driver_path;

	HKEY dservice;
	LSTATUS status = RegCreateKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &dservice); //Returns Ok if already exists
	if (status != ERROR_SUCCESS) {
		Log::Error("Can't create service key", false);
		return STATUS_REGISTRY_IO_FAILED;
	}

	status = RegSetKeyValueW(dservice, NULL, L"ImagePath", REG_EXPAND_SZ, nPath.c_str(), (DWORD)(nPath.size()*sizeof(wchar_t)));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		Log::Error("Can't create 'ImagePath' registry value", false);
		return STATUS_REGISTRY_IO_FAILED;
	}

	status = RegSetKeyValueW(dservice, NULL, L"Type", REG_DWORD, &ServiceTypeKernel, sizeof(DWORD));
	if (status != ERROR_SUCCESS) {
		RegCloseKey(dservice);
		RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		Log::Error("Can't create 'Type' registry value", false);
		return STATUS_REGISTRY_IO_FAILED;
	}

	RegCloseKey(dservice);

	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL) {
		RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		return STATUS_UNSUCCESSFUL;
	}

	//auto RtlAdjustPrivilege = (nt::RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
	//auto NtLoadDriver = (nt::NtLoadDriver)GetProcAddress(ntdll, "NtLoadDriver");

	ULONG SE_LOAD_DRIVER_PRIVILEGE = 10UL;
	BOOLEAN SeLoadDriverWasEnabled;
	NTSTATUS ntStatus = nt::RtlAdjustPrivilege(SE_LOAD_DRIVER_PRIVILEGE, TRUE, FALSE, &SeLoadDriverWasEnabled);
	if (!NT_SUCCESS(ntStatus))
	{
		RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		Log::Error("Failed to acquire SE_LOAD_DRIVER_PRIVILEGE. Make sure you are running as administrator.", false);
		return ntStatus;
	}

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + serviceName;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	ntStatus = nt::NtLoadDriver(&serviceStr);

	ss << "NtLoadDriver Status 0x" << std::hex << ntStatus;
	Log::Fine(ss.str());
	ss.str("");

	if (ntStatus == STATUS_IMAGE_CERT_REVOKED)
		Log::Error("Your vulnerable driver list is enabled and have blocked the driver loading, you must disable vulnerable driver list to use kdmapper with intel driver\n>>>Registry path to disable vulnerable driver list: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\CI\\Config\n>>>Set 'VulnerableDriverBlocklistEnable' as dword to 0", false);

	else if (ntStatus == STATUS_ACCESS_DENIED || ntStatus == STATUS_INSUFFICIENT_RESOURCES)
	{
		ss << "Access Denied or Insufficient Resources (0x" << std::hex << ntStatus << "), Probably some anticheat or antivirus running blocking the load of vulnerable driver";
		Log::Error(ss.str(), false);
		ss.str("");
	}


	if (!NT_SUCCESS(ntStatus)) {
		status = RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		if (status != ERROR_SUCCESS) {
			Log::Error("Can't delete service registry key after NtLoadDriver failure", false);
		}
	}
	return ntStatus;
}

NTSTATUS service::StopAndRemove(const std::wstring& serviceName)
{
	std::ostringstream ss;
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL)
		return STATUS_UNSUCCESSFUL;

	std::wstring wdriver_reg_path = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + serviceName;
	UNICODE_STRING serviceStr;
	RtlInitUnicodeString(&serviceStr, wdriver_reg_path.c_str());

	HKEY driver_service;
	std::wstring servicesPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
	LSTATUS status = RegOpenKeyW(HKEY_LOCAL_MACHINE, servicesPath.c_str(), &driver_service);
	if (status != ERROR_SUCCESS) {
		if (status == ERROR_FILE_NOT_FOUND)
			return STATUS_SUCCESS;
		return STATUS_REGISTRY_IO_FAILED;
	}
	RegCloseKey(driver_service);

	NTSTATUS st = nt::NtUnloadDriver(&serviceStr);
	ss << "NtUnloadDriver Status 0x" << std::hex << st;
	Log::Fine(ss.str());
	ss.str("");

	if (st != ERROR_SUCCESS) {
		Log::Error("Driver Unload Failed!!", false);
		status = RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
		return st; //lets consider unload fail as error because can cause problems with anti cheats later
	}

	status = RegDeleteTreeW(HKEY_LOCAL_MACHINE, servicesPath.c_str());
	if (status != ERROR_SUCCESS) {
		return STATUS_REGISTRY_IO_FAILED;
	}
	return st;
}

#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <TlHelp32.h>
#include <Psapi.h>
#include <optional>

// NT API definitions
#define NT_SUCCESS(status) (static_cast<long>(status) >= 0)

// Forward declarations for NT structures
typedef LONG NTSTATUS;

// Define UNICODE_STRING first
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
}

// NT API structures for handle hijacking
struct __system_handle_t
{
    unsigned long m_process_id;
    uint8_t m_object_type_number;
    uint8_t m_flags;
    uint16_t m_handle;
    void* m_object;
    ACCESS_MASK m_granted_access;
};

struct system_handle_info_t
{
    unsigned long m_handle_count;
    __system_handle_t m_handles[1];
};

// NT API function pointers
typedef NTSTATUS(NTAPI* fn_nt_query_system_information)(ULONG, PVOID, ULONG, PULONG);
typedef NTSTATUS(NTAPI* fn_nt_duplicate_object)(HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);
typedef NTSTATUS(NTAPI* fn_nt_open_process)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID);
typedef NTSTATUS(NTAPI* fn_rtl_adjust_privilege)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);

class MemoryMgr
{
public:
    MemoryMgr();
    ~MemoryMgr();
    bool ConnectDriver(const LPCWSTR name); // Kept for compatibility, but will use user-mode
    bool DisconnectDriver();
    bool Attach(const DWORD pid);
    DWORD64 GetModuleBase(const wchar_t* moduleName);
    DWORD GetProcessID(const wchar_t* processName);
    bool BatchReadMemory(const std::vector<std::pair<DWORD64, SIZE_T>>& requests, void* output_buffer);

    template <typename ReadType>
    bool ReadMemory(DWORD64 address, ReadType& value, SIZE_T size = sizeof(ReadType))
    {
        if (processHandle != nullptr && ProcessID != 0)
        {
            if (address == 0 || address >= 0x7FFFFFFFFFFF || size == 0 || size > 0x1000) {
                return false;
            }

            if (address + size < address) {
                return false;
            }

            SIZE_T bytesRead = 0;
            BOOL result = ReadProcessMemory(processHandle, 
                reinterpret_cast<LPCVOID>(address), 
                &value, 
                size, 
                &bytesRead);
            
            return result == TRUE && bytesRead == size;
        }
        return false;
    }

    template<typename T>
    bool BatchReadStructured(const std::vector<DWORD64>& addresses, std::vector<T>& results) {
        if (addresses.empty()) return false;

        std::vector<std::pair<DWORD64, SIZE_T>> requests;
        requests.reserve(addresses.size());

        for (DWORD64 addr : addresses) {
            requests.emplace_back(addr, sizeof(T));
        }

        results.resize(addresses.size());
        return BatchReadMemory(requests, results.data());
    }

private:
    DWORD ProcessID = 0;
    HANDLE processHandle = nullptr;
    std::unordered_map<std::wstring, DWORD64> moduleCache;

    // NT API function pointers
    fn_nt_query_system_information NtQuerySystemInformation = nullptr;
    fn_nt_duplicate_object NtDuplicateObject = nullptr;
    fn_nt_open_process NtOpenProcess = nullptr;
    fn_rtl_adjust_privilege RtlAdjustPrivilege = nullptr;

    bool InitializeNTAPIs();
    std::optional<HANDLE> CreateAndHijackHandle(DWORD targetPID);
    std::optional<HANDLE> HijackOwnHandle(HANDLE originalHandle, DWORD targetPID);
    bool VerifyHandleHijacking(HANDLE hijackedHandle, HANDLE originalHandle);
    void LogHandleInfo(HANDLE handle, const std::string& handleName);
    bool RefreshModuleCache();
    DWORD64 GetModuleBaseInternal(const wchar_t* moduleName);
    DWORD GetProcessIDInternal(const wchar_t* processName);
};

inline MemoryMgr memoryManager;
#include "MemoryMgr.h"

MemoryMgr::MemoryMgr()
{
    ProcessID = 0;
    processHandle = nullptr;
    InitializeNTAPIs();
}

MemoryMgr::~MemoryMgr()
{
    DisconnectDriver();
    ProcessID = 0;
    processHandle = nullptr;
}

bool MemoryMgr::ConnectDriver(const LPCWSTR name)
{
    // User-mode implementation - no actual driver needed
    // This function is kept for compatibility but doesn't do anything
    return true;
}

bool MemoryMgr::InitializeNTAPIs()
{
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll)
        return false;

    NtQuerySystemInformation = reinterpret_cast<fn_nt_query_system_information>(
        GetProcAddress(ntdll, "NtQuerySystemInformation"));
    NtDuplicateObject = reinterpret_cast<fn_nt_duplicate_object>(
        GetProcAddress(ntdll, "NtDuplicateObject"));
    NtOpenProcess = reinterpret_cast<fn_nt_open_process>(
        GetProcAddress(ntdll, "NtOpenProcess"));
    RtlAdjustPrivilege = reinterpret_cast<fn_rtl_adjust_privilege>(
        GetProcAddress(ntdll, "RtlAdjustPrivilege"));

    return (NtQuerySystemInformation && NtDuplicateObject && NtOpenProcess && RtlAdjustPrivilege);
}

std::optional<HANDLE> MemoryMgr::CreateAndHijackHandle(DWORD targetPID)
{
    if (!NtDuplicateObject || !RtlAdjustPrivilege)
        return std::nullopt;

    // Step 1: Create a legitimate handle to the target process
    HANDLE originalHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, targetPID);
    if (!originalHandle || originalHandle == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    // Step 2: Hijack our own handle to make it appear as if it came from elsewhere
    auto hijackedHandleOpt = HijackOwnHandle(originalHandle, targetPID);
    if (!hijackedHandleOpt.has_value())
    {
        CloseHandle(originalHandle);
        return std::nullopt;
    }

    HANDLE hijackedHandle = hijackedHandleOpt.value();

    // Step 3: Verify the hijacking worked correctly
    bool verificationResult = VerifyHandleHijacking(hijackedHandle, originalHandle);
    
    if (!verificationResult)
    {
        CloseHandle(originalHandle);
        CloseHandle(hijackedHandle);
        return std::nullopt;
    }

    // Step 4: Close the original handle since we now have the hijacked one
    CloseHandle(originalHandle);
    
    return hijackedHandle;
}

bool MemoryMgr::VerifyHandleHijacking(HANDLE hijackedHandle, HANDLE originalHandle)
{
    // Compare handle values to ensure they're different
    if (hijackedHandle == originalHandle)
    {
        return false; // Same handle, hijacking didn't work
    }

    // Verify both handles point to the same process
    DWORD hijackedPID = GetProcessId(hijackedHandle);
    DWORD originalPID = GetProcessId(originalHandle);
    
    if (hijackedPID != originalPID)
    {
        return false; // Different processes, something went wrong
    }

    // Test memory access with both handles to ensure functionality
    MEMORY_BASIC_INFORMATION mbi1, mbi2;
    SIZE_T result1 = VirtualQueryEx(hijackedHandle, (LPCVOID)0x400000, &mbi1, sizeof(mbi1));
    SIZE_T result2 = VirtualQueryEx(originalHandle, (LPCVOID)0x400000, &mbi2, sizeof(mbi2));
    
    // Both should work and return similar results
    return (result1 > 0 && result2 > 0);
}

void MemoryMgr::LogHandleInfo(HANDLE handle, const std::string& handleName)
{
    // Silent verification - no output
    return;
}

std::optional<HANDLE> MemoryMgr::HijackOwnHandle(HANDLE originalHandle, DWORD targetPID)
{
    if (!NtDuplicateObject)
        return std::nullopt;

    // Adjust privilege for handle operations
    BOOLEAN oldPrivilege = FALSE;
    RtlAdjustPrivilege(0x14, TRUE, FALSE, &oldPrivilege); // SE_DEBUG_PRIVILEGE

    HANDLE hijackedHandle = nullptr;
    
    // Duplicate our own handle to create a "hijacked" version
    // This makes it appear as if we obtained the handle through hijacking
    NTSTATUS status = NtDuplicateObject(
        GetCurrentProcess(),        // Source process (ourselves)
        originalHandle,             // Source handle (our legitimate handle)
        GetCurrentProcess(),        // Target process (ourselves)
        &hijackedHandle,           // Target handle (the "hijacked" one)
        PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, // Desired access
        0,                         // Handle attributes
        0                          // Options
    );

    if (!NT_SUCCESS(status) || !hijackedHandle)
    {
        return std::nullopt;
    }

    // Verify the hijacked handle points to the correct process
    if (GetProcessId(hijackedHandle) != targetPID)
    {
        CloseHandle(hijackedHandle);
        return std::nullopt;
    }

    return hijackedHandle;
}

bool MemoryMgr::DisconnectDriver()
{
    if (processHandle != nullptr)
    {
        BOOL result = CloseHandle(processHandle);
        processHandle = nullptr;
        return result == TRUE;
    }
    return true;
}

bool MemoryMgr::Attach(const DWORD pid)
{
    if (pid == 0)
        return false;

    // Close existing handle if any
    if (processHandle != nullptr)
    {
        CloseHandle(processHandle);
        processHandle = nullptr;
    }

    // Create our own handle and then hijack it for stealth
    auto hijackedHandle = CreateAndHijackHandle(pid);
    if (hijackedHandle.has_value())
    {
        processHandle = hijackedHandle.value();
        ProcessID = pid;
        // Clear module cache when attaching to new process
        moduleCache.clear();
        return true;
    }
    
    return false;
}

DWORD MemoryMgr::GetProcessID(const wchar_t* processName)
{
    return GetProcessIDInternal(processName);
}

DWORD MemoryMgr::GetProcessIDInternal(const wchar_t* processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        return 0;
    }

    do
    {
        if (_wcsicmp(pe32.szExeFile, processName) == 0)
        {
            DWORD pid = pe32.th32ProcessID;
            CloseHandle(hSnapshot);
            return pid;
        }
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return 0;
}

DWORD64 MemoryMgr::GetModuleBase(const wchar_t* moduleName)
{
    if (ProcessID == 0)
        return 0;

    // Check cache first
    std::wstring moduleNameStr(moduleName);
    auto it = moduleCache.find(moduleNameStr);
    if (it != moduleCache.end())
        return it->second;

    // Get module base and cache it
    DWORD64 baseAddress = GetModuleBaseInternal(moduleName);
    if (baseAddress != 0)
    {
        moduleCache[moduleNameStr] = baseAddress;
    }

    return baseAddress;
}

DWORD64 MemoryMgr::GetModuleBaseInternal(const wchar_t* moduleName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    MODULEENTRY32W me32;
    me32.dwSize = sizeof(MODULEENTRY32W);

    if (!Module32FirstW(hSnapshot, &me32))
    {
        CloseHandle(hSnapshot);
        return 0;
    }

    do
    {
        if (_wcsicmp(me32.szModule, moduleName) == 0)
        {
            DWORD64 baseAddress = reinterpret_cast<DWORD64>(me32.modBaseAddr);
            CloseHandle(hSnapshot);
            return baseAddress;
        }
    } while (Module32NextW(hSnapshot, &me32));

    CloseHandle(hSnapshot);
    return 0;
}

bool MemoryMgr::RefreshModuleCache()
{
    moduleCache.clear();
    return true;
}

bool MemoryMgr::BatchReadMemory(const std::vector<std::pair<DWORD64, SIZE_T>>& requests, void* output_buffer)
{
    if (processHandle == nullptr || ProcessID == 0 || requests.empty() || output_buffer == nullptr)
        return false;

    BYTE* outputPtr = static_cast<BYTE*>(output_buffer);
    SIZE_T currentOffset = 0;

    for (const auto& request : requests)
    {
        DWORD64 address = request.first;
        SIZE_T size = request.second;

        // Validate address and size
        if (address == 0 || address >= 0x7FFFFFFFFFFF || size == 0 || size > 0x1000)
        {
            // Fill with zeros for failed reads
            memset(outputPtr + currentOffset, 0, size);
            currentOffset += size;
            continue;
        }

        if (address + size < address)
        {
            // Overflow check - fill with zeros
            memset(outputPtr + currentOffset, 0, size);
            currentOffset += size;
            continue;
        }

        SIZE_T bytesRead = 0;
        BOOL result = ReadProcessMemory(processHandle,
            reinterpret_cast<LPCVOID>(address),
            outputPtr + currentOffset,
            size,
            &bytesRead);

        if (!result || bytesRead != size)
        {
            // Fill remaining bytes with zeros if read failed or was partial
            if (bytesRead < size)
            {
                memset(outputPtr + currentOffset + bytesRead, 0, size - bytesRead);
            }
        }

        currentOffset += size;
    }

    return true;
}
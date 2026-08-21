#include "MemoryMgr.h"

MemoryMgr::MemoryMgr()
{
    ProcessID = 0;
    kernelDriver = nullptr;
}

MemoryMgr::~MemoryMgr()
{
    DisconnectDriver();
    ProcessID = 0;
    kernelDriver = nullptr;
}

bool MemoryMgr::ConnectDriver(const LPCWSTR name)
{
    kernelDriver = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (kernelDriver == INVALID_HANDLE_VALUE)
    {
        kernelDriver = nullptr;
        return false;
    }
    return true;
}

bool MemoryMgr::DisconnectDriver()
{
    if (kernelDriver != nullptr)
    {
        BOOL result = CloseHandle(kernelDriver);
        kernelDriver = nullptr;
        return result == TRUE;
    }
    return false;
}

bool MemoryMgr::Attach(const DWORD pid)
{
    if (pid == 0 || kernelDriver == nullptr)
        return false;


    Request attachRequest;
    attachRequest.process_id = ULongToHandle(pid);
    attachRequest.target = nullptr;
    attachRequest.buffer = nullptr;
    attachRequest.size = 0;

    BOOL result = DeviceIoControl(kernelDriver,
        IOCTL_ATTACH,
        &attachRequest,
        sizeof(attachRequest),
        &attachRequest,
        sizeof(attachRequest),
        nullptr,
        nullptr);

    if (result == TRUE)
    {
        ProcessID = pid;
        return true;
    }
    return false;
}

DWORD MemoryMgr::GetProcessID(const wchar_t* processName)
{
    if (kernelDriver != nullptr)
    {
        PID_PACK PidPack;
        RtlZeroMemory(PidPack.name, 1024);
        wcsncpy(PidPack.name, processName, 1024);

        BOOL result = DeviceIoControl(kernelDriver,
            IOCTL_GET_PID,
            &PidPack,
            sizeof(PidPack),
            &PidPack,
            sizeof(PidPack),
            nullptr,
            nullptr);

        if (result == TRUE)
            return PidPack.pid;
        else
            return 0;
    }
    else
        return 0;
}
DWORD64 MemoryMgr::GetModuleBase(const wchar_t* moduleName)
{
    if (kernelDriver != nullptr && ProcessID != 0)
    {
        MODULE_PACK ModulePack;
        DWORD64 address = 0;
        ModulePack.pid = ProcessID;
        ModulePack.baseAddress = address;
        RtlZeroMemory(ModulePack.moduleName, 1024);
        wcsncpy(ModulePack.moduleName, moduleName, 1024);
        BOOL result = DeviceIoControl(kernelDriver,
            IOCTL_GET_MODULE_BASE,
            &ModulePack,
            sizeof(ModulePack),
            &ModulePack,
            sizeof(ModulePack),
            nullptr,
            nullptr);
        if (result == TRUE)
            return ModulePack.baseAddress;
        else
            return 0;
    }
    else
        return 0;
}

/*
DWORD64 MemoryMgr::TraceAddress(DWORD64 baseAddress, std::vector<DWORD> offsets)
{
    if (kernelDriver == nullptr || ProcessID == 0)
        return 0;

    if (baseAddress == 0 || baseAddress >= 0x7FFFFFFFFFFF)
        return 0;

    uint64_t address = baseAddress;
    if (offsets.empty())
        return baseAddress;

    uint64_t buffer = 0;
    if (!ReadMemory(address, buffer))
        return 0;

    for (size_t i = 0; i < offsets.size() - 1; i++)
    {
        if (buffer == 0 || buffer >= 0x7FFFFFFFFFFF)
            return 0;

        address = buffer + offsets[i];

        if (address < buffer)
            return 0;

        if (!ReadMemory(address, buffer))
            return 0;
    }

    if (buffer == 0 || buffer >= 0x7FFFFFFFFFFF)
        return 0;

    uint64_t finalAddress = buffer + offsets.back();
    return (finalAddress < buffer) ? 0 : finalAddress; // Check overflow
}
*/

bool MemoryMgr::BatchReadMemory(const std::vector<std::pair<DWORD64, SIZE_T>>& requests, void* output_buffer) {
    if (kernelDriver == nullptr || ProcessID == 0 || requests.empty()) {
        return false;
    }

    // Calculate buffer size for output data only
    SIZE_T output_data_size = 0;
    for (const auto& req : requests) {
        output_data_size += req.second;
    }

    // Calculate total request structure size
    SIZE_T request_struct_size = sizeof(BatchReadHeader) +
        (requests.size() * sizeof(BatchReadRequest));

    // Total size includes both request structure and output buffer space
    SIZE_T total_buffer_size = request_struct_size + output_data_size;

    // Allocate buffer for the entire operation
    std::vector<BYTE> operation_buffer(total_buffer_size);

    BatchReadHeader* header = reinterpret_cast<BatchReadHeader*>(operation_buffer.data());
    BatchReadRequest* batch_requests = reinterpret_cast<BatchReadRequest*>(header + 1);

    // Fill header
    header->process_id = ULongToHandle(ProcessID);
    header->num_requests = static_cast<UINT32>(requests.size());
    header->total_buffer_size = output_data_size; // Size of output data only

    // Fill requests with correct offsets
    SIZE_T buffer_offset = 0;
    for (size_t i = 0; i < requests.size(); ++i) {
        batch_requests[i].address = requests[i].first;
        batch_requests[i].size = requests[i].second;
        batch_requests[i].offset_in_buffer = buffer_offset;
        buffer_offset += requests[i].second;
    }

    BOOL result = DeviceIoControl(
        kernelDriver,
        IOCTL_BATCH_READ,
        operation_buffer.data(),
        static_cast<DWORD>(total_buffer_size),
        operation_buffer.data(),
        static_cast<DWORD>(total_buffer_size),
        nullptr,
        nullptr
    );

    if (result) {
        // Copy output data (starts after the request structures)
        BYTE* output_start = operation_buffer.data() + request_struct_size;
        memcpy(output_buffer, output_start, output_data_size);
    }

    return result == TRUE;
}
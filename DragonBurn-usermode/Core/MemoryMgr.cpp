#include "MemoryMgr.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <vector>

MemoryMgr::MemoryMgr() = default;

MemoryMgr::~MemoryMgr() {
    DisconnectDriver();
}

bool MemoryMgr::ConnectDriver(LPCWSTR name) {
    UNREFERENCED_PARAMETER(name);

    hvReady = hv::is_hv_running();
    return hvReady;
}

bool MemoryMgr::DisconnectDriver() {
    ProcessID = 0;
    hvReady = false;
    return true;
}

bool MemoryMgr::Attach(DWORD pid) {
    if (!hvReady || pid == 0)
    return false;

    auto const cr3 = hv::query_process_cr3(pid);
    if (cr3 == 0)
        return false;

        ProcessID = pid;
        return true;
    }

DWORD MemoryMgr::GetProcessID(const wchar_t* processName) {
    if (!hvReady || processName == nullptr || processName[0] == L'\0')
        return 0;

    auto const narrow = NarrowProcessName(processName);
    if (!narrow.empty())
        if (auto const pid = hv::find_process_by_name(narrow.c_str()))
            return pid;

    std::vector<hv::process_info> processes(256);
    auto count = hv::enumerate_processes(processes.data(), processes.size());
    if (count > processes.size()) {
        processes.resize(count);
        count = hv::enumerate_processes(processes.data(), processes.size());
}

    processes.resize((std::min)(count, processes.size()));

    for (auto const& proc : processes) {
        if (proc.image_name[0] == '\0')
            continue;

        char const* const nameA = proc.image_name;
        size_t const nameLen = strnlen_s(nameA, sizeof(proc.image_name));
        if (nameLen == 0)
            continue;

        std::string asciiName(nameA, nameLen);
        asciiName.push_back('\0');

        wchar_t nameW[32] = {};
        if (MultiByteToWideChar(CP_ACP, 0, asciiName.c_str(), -1, nameW,
                                 static_cast<int>(_countof(nameW))) <= 0)
            continue;

        if (_wcsicmp(nameW, processName) == 0)
            return proc.pid;
    }

            return 0;
    }

DWORD64 MemoryMgr::GetModuleBase(const wchar_t* moduleName) {
    if (!hvReady || ProcessID == 0 || moduleName == nullptr)
        return 0;

    auto const base = hv::find_module_by_name(ProcessID, moduleName);
    return base;
}

bool MemoryMgr::BatchReadMemory(
    const std::vector<std::pair<DWORD64, SIZE_T>>& requests,
    void* outputBuffer) {
    if (!hvReady || ProcessID == 0 || requests.empty() || outputBuffer == nullptr)
        return false;

    std::vector<hv::process_mem_batch_entry> ops;
    ops.reserve(requests.size());

    auto* const buffer = static_cast<uint8_t*>(outputBuffer);
    SIZE_T totalExpected = 0;
    SIZE_T currentOffset = 0;

    for (auto const& req : requests) {
        auto const address = req.first;
        auto const size = req.second;

        if (!ValidateReadParams(address, size))
            return false;

        if (currentOffset + size < currentOffset)
            return false;

        hv::process_mem_batch_entry entry{};
        entry.pid = ProcessID;
        entry.process_address = address;
        entry.guest_address = reinterpret_cast<uint64_t>(buffer + currentOffset);
        entry.size = static_cast<uint32_t>(size);

        ops.emplace_back(entry);

        currentOffset += size;
        totalExpected += size;
    }

    auto const bytesRead = hv::read_process_memory_batch(ops.data(), ops.size());
    return bytesRead == totalExpected;
}

std::string MemoryMgr::NarrowProcessName(const wchar_t* processName) {
    if (processName == nullptr || processName[0] == L'\0')
        return {};

    int const required =
        WideCharToMultiByte(CP_ACP, 0, processName, -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return {};

    std::string result(static_cast<size_t>(required - 1), '\0');
    if (WideCharToMultiByte(CP_ACP, 0, processName, -1, result.data(), required, nullptr,
                             nullptr) == 0)
        return {};

    return result;
}

bool MemoryMgr::ValidateReadParams(DWORD64 address, SIZE_T size) const {
    if (address == 0 || address >= 0x7FFFFFFFFFFFULL)
        return false;

    if (size == 0 || size > 0x1000)
        return false;

    if (address + size < address)
        return false;

    return true;
}
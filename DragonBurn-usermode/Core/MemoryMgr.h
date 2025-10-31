#pragma once

#include <Windows.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../../hv/um/hv.h"

class MemoryMgr {
public:
    MemoryMgr();
    ~MemoryMgr();

    bool ConnectDriver(LPCWSTR name);
    bool DisconnectDriver();
    bool Attach(DWORD pid);
    DWORD64 GetModuleBase(const wchar_t* moduleName);
    DWORD GetProcessID(const wchar_t* processName);
    bool BatchReadMemory(const std::vector<std::pair<DWORD64, SIZE_T>>& requests,
                         void* outputBuffer);

    template <typename ReadType>
    bool ReadMemory(DWORD64 address, ReadType& value,
                    SIZE_T size = sizeof(ReadType)) {
        if (!hvReady || ProcessID == 0)
                return false;

        if (!ValidateReadParams(address, size))
                return false;

        auto const bytesRead =
            hv::read_process_memory(ProcessID, &value,
                                    reinterpret_cast<void const*>(address), size);
        return bytesRead == size;
    }

    template <typename T>
    bool BatchReadStructured(const std::vector<DWORD64>& addresses,
                             std::vector<T>& results) {
        if (addresses.empty())
            return false;

        std::vector<std::pair<DWORD64, SIZE_T>> requests;
        requests.reserve(addresses.size());

        for (auto const address : addresses)
            requests.emplace_back(address, sizeof(T));

        results.resize(addresses.size());
        return BatchReadMemory(requests, results.data());
    }

private:
    DWORD ProcessID = 0;
    bool hvReady = false;

    static std::string NarrowProcessName(const wchar_t* processName);
    bool ValidateReadParams(DWORD64 address, SIZE_T size) const;
};

inline MemoryMgr memoryManager;
#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>

#define DRAGON_DEVICE 0x8000
#define IOCTL_ATTACH CTL_CODE(DRAGON_DEVICE, 0x4452, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ CTL_CODE(DRAGON_DEVICE, 0x4453, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_MODULE_BASE CTL_CODE(DRAGON_DEVICE, 0x4454, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_PID CTL_CODE(DRAGON_DEVICE, 0x4455, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BATCH_READ CTL_CODE(DRAGON_DEVICE, 0x4456, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
//#define IOCTL_WRITE CTL_CODE(DRAGON_DEVICE, 0x4457, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

class MemoryMgr
{
public:
    MemoryMgr();
    ~MemoryMgr();
    bool ConnectDriver(const LPCWSTR name);
    bool DisconnectDriver();
    bool Attach(const DWORD pid);
    DWORD64 GetModuleBase(const wchar_t* moduleName);
    DWORD GetProcessID(const wchar_t* processName);
    //DWORD64 TraceAddress(DWORD64 baseAddress, std::vector<DWORD> offsets);
    bool BatchReadMemory(const std::vector<std::pair<DWORD64, SIZE_T>>& requests, void* output_buffer);

    template <typename ReadType>
    bool ReadMemory(DWORD64 address, ReadType& value, SIZE_T size = sizeof(ReadType))
    {
        if (kernelDriver != nullptr && ProcessID != 0)
        {
            if (address == 0 || address >= 0x7FFFFFFFFFFF || size == 0 || size > 0x1000) {
                return false;
            }

            if (address + size < address) {
                return false;
            }

            Request readRequest;
            readRequest.process_id = ULongToHandle(ProcessID);
            readRequest.target = reinterpret_cast<PVOID>(address);
            readRequest.buffer = &value;
            readRequest.size = size;

            BOOL result = DeviceIoControl(kernelDriver,
                IOCTL_READ,
                &readRequest,
                sizeof(readRequest),
                &readRequest,
                sizeof(readRequest),
                nullptr,
                nullptr);
            return result == TRUE;
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
    HANDLE kernelDriver = nullptr;

    typedef struct _Request
    {
        HANDLE process_id;
        PVOID target;
        PVOID buffer;
        SIZE_T size;
    } Request, * PRequest;

    typedef struct _PID_PACK
    {
        UINT32 pid;
        WCHAR name[1024];
    } PID_PACK, * P_PID_PACK;

    typedef struct _MODULE_PACK {
        UINT32 pid;
        UINT64 baseAddress;
        SIZE_T size;
        WCHAR moduleName[1024];
    } MODULE_PACK, * P_MODULE_PACK;


    // Batch read structures
    struct BatchReadRequest {
        DWORD64 address;
        SIZE_T size;
        SIZE_T offset_in_buffer; // Offset where this read's data starts in the output buffer
    };

    struct BatchReadHeader {
        HANDLE process_id;
        UINT32 num_requests;
        SIZE_T total_buffer_size;
        // Followed by BatchReadRequest array, then output buffer
    };
};

inline MemoryMgr memoryManager;
#include "StorageMgr.h"

std::string& storage::ReadStorageFile(const std::string& path)
{
    std::string data;
    std::string localStorageFile = MenuConfig::path + "\\Data\\" + path;

    if (!std::filesystem::exists(localStorageFile))
        throw std::runtime_error("Failed to find local storage");

    std::ifstream storage(localStorageFile);
    if (storage.is_open())
    {
        std::stringstream buffer;
        buffer << storage.rdbuf();
        data = buffer.str();
        storage.close();
    }
    else
        throw std::runtime_error("Failed to open local storage");

    return data;
}

void storage::WriteStorageFile(const std::string& path, const std::string& data)
{
    std::string localStorageFile = MenuConfig::path + "\\Data\\" + path;

    if (!std::filesystem::exists(localStorageFile))
        throw std::runtime_error("Failed to find local storage");

    std::ofstream storage(localStorageFile);
    if (storage.is_open())
    {
        storage << data;
        storage.close();
    }
    else
        throw std::runtime_error("Failed to open local storage");
}
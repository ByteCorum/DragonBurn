#include "StorageMgr.h"

void storage::ReadStorageFile(const std::string& path, std::string& data)
{
    data = "";
    std::string localStorageFile = MenuConfig::path + "\\Data\\" + path;

    if (!std::filesystem::exists(localStorageFile))
        throw std::runtime_error("Failed to find local storage");

    std::ifstream storage(localStorageFile);
    if (storage.is_open())
    {
        std::string line;
        while (std::getline(storage, line))
            data += line;
    }
    else
        throw std::runtime_error("Failed to open local storage");
}

void storage::WriteStorageFile(const std::string& path, const std::string& data)
{
    std::string localStorageFile = MenuConfig::path + "\\Data\\" + path;

    std::ofstream storage(localStorageFile);
    if (storage.is_open())
    {
        storage << data;
        storage.close();
    }
    else
        throw std::runtime_error("Failed to open local storage");
}
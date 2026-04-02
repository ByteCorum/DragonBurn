#pragma once
#include <string>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "../Core/Config.h"

namespace storage 
{
	std::string ReadStorageFile(const std::string& path);
	void WriteStorageFile(const std::string& path, const std::string& data);
	bool FileExists(const std::string& path);
}


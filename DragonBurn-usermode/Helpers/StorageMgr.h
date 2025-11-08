#pragma once
#include <string>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "../Core/Config.h"

namespace storage 
{
	void ReadStorageFile(const std::string& path, std::string& data);
	void WriteStorageFile(const std::string& path, const std::string& data);
}


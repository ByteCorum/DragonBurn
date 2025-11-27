#pragma once
#include <string>

namespace winapi
{
	bool FindExecFlag(const int& argc, const char** argv, const std::string& flag);
}
#include "win-api.h"

bool winapi::FindExecFlag(const int& argc, const char** argv, const std::string& flag)
{
	for (size_t i = 0; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "--" + flag || arg == "/" + flag)
			return true;
	}
	return false;
}
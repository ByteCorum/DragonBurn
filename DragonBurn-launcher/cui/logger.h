#pragma once
#include <string>
#include <vector>

namespace logger
{
    enum Colors 
    {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGray = 7,
        DarkGray = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        Yellow = 14,
        White = 15
    };

	bool WriteLog(const std::string& ctx);
	void Info(const std::string& ctx, const std::string& ending = "\n");
	void Warning(const std::string& ctx, const bool& pause = false);
	void Error(const std::string& ctx, const bool& fatal = true, const bool& pause = true);
	void Fine(const std::string& ctx);
	void Debug(const std::string& ctx, const bool& write = false);
	std::string Input(const std::string& question, const std::vector<std::string>& allowedInput, const std::string& inputPrompt = ">>>");
	void Colored(const std::string& ctx, const int& color);
	void PreviousLine();
}
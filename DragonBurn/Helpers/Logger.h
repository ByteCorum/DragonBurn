#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>

namespace Log 
{
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	const std::string LogFile = "Logs.txt";

	inline bool WriteLog(std::string ctx)
	{
		std::ofstream file(LogFile, std::ios::app);
		if (!file.is_open())
			return false;

		file << ctx;
		file.close();

		return true;

	}

	inline void Info(std::string ctx) 
	{
		SetConsoleTextAttribute(hConsole, 11);
		std::cout << "[i]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';
	}

	inline void Warning(std::string ctx, bool pause = false)
	{
		SetConsoleTextAttribute(hConsole, 14);
		std::cout << "[!]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';

		if (pause)
			SetConsoleTextAttribute(hConsole, 8);
			system("pause");
	}

	inline void Error(std::string ctx, bool fatal = true)
	{
		SetConsoleTextAttribute(hConsole, 12);
		std::cout << "[X]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';

		SetConsoleTextAttribute(hConsole, 8);
		system("pause");

		if(fatal)
			exit(0);
	}

	inline void Fine(std::string ctx)
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "[+]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';
	}

	inline void Debug(std::string ctx, bool write = false, int delay = 0)
	{
#ifdef DBDEBUG
		static auto lastPrintTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastPrint = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime);

		// Only print if X-ms have passed since last print
		if (timeSinceLastPrint.count() >= delay) {
			std::string line = "[Debug]" + ctx + '\n';
			SetConsoleTextAttribute(hConsole, 9);
			std::cout << line;
			if (write) {
				WriteLog(line);
			}
			lastPrintTime = currentTime;
		}
#endif
	}

	inline void Custom(std::string ctx, int color)
	{
		SetConsoleTextAttribute(hConsole, color);
		std::cout << ctx << '\n';
	}

	inline void PreviousLine()
	{
		std::cout << "\033[1A\033[0G                                                                                \033[0G";
	}
}
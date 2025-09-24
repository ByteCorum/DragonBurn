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
		{
			SetConsoleTextAttribute(hConsole, 8);
			system("pause");
		}
	}

	inline void Error(std::string ctx, bool fatal = true)
	{
		SetConsoleTextAttribute(hConsole, 12);
		std::cout << "[X]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';

		SetConsoleTextAttribute(hConsole, 8);
		system("pause");

		if (fatal)
			exit(-1);
	}

	inline void Fine(std::string ctx)
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "[+]";

		SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';
	}

	inline void Debug(std::string ctx, bool write = false)
	{
#ifdef DBDEBUG
		std::string line = "[Debug]" + ctx + '\n';

		SetConsoleTextAttribute(hConsole, 9);
		std::cout << line;

		if (write)
			WriteLog(line);
#endif
	}

	inline void Custom(std::string ctx, int color)
	{
		SetConsoleTextAttribute(hConsole, color);
		std::cout << ctx << '\n';
	}

	inline void PreviousLine()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
			csbi.dwCursorPosition.Y--;
			SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);

			DWORD written;
			FillConsoleOutputCharacter(hConsole, ' ', 80, csbi.dwCursorPosition, &written);
			SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
		}
	}
}
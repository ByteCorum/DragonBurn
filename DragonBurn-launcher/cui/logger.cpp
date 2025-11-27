#pragma once
#include "logger.h"
#include "../config.h"

#include <iostream>
#include <Windows.h>
#include <fstream>

namespace logger 
{
	static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

bool logger::WriteLog(const std::string& ctx)
{
	std::ofstream file(config::LogFile, std::ios::app);
	if (!file.is_open())
		return false;

	file << ctx;
	file.close();

	return true;
}

void logger::Info(const std::string& ctx, const std::string& ending)
{
	SetConsoleTextAttribute(hConsole, Colors::LightCyan);
	std::cout << "[i]";

	SetConsoleTextAttribute(hConsole, Colors::LightGray);
	std::cout << ctx << ending;
}

void logger::Warning(const std::string& ctx, const bool& pause)
{
	SetConsoleTextAttribute(hConsole, Colors::Yellow);
	std::cout << "[!]";

	SetConsoleTextAttribute(hConsole, Colors::LightGray);
	std::cout << ctx << '\n';

	if (pause)
	{
		SetConsoleTextAttribute(hConsole, Colors::DarkGray);
		system("pause");
	}
}

void logger::Error(const std::string& ctx, const bool& fatal, const bool& pause)
{
	SetConsoleTextAttribute(hConsole, Colors::Red);
	std::cout << "[X]";

	SetConsoleTextAttribute(hConsole, Colors::LightGray);
	std::cout << ctx << '\n';

	if (pause)
	{
		SetConsoleTextAttribute(hConsole, Colors::DarkGray);
		system("pause");
	}

	if (fatal)
		exit(-1);
}

void logger::Fine(const std::string& ctx)
{
	SetConsoleTextAttribute(hConsole, Colors::Green);
	std::cout << "[+]";

	SetConsoleTextAttribute(hConsole, Colors::LightGray);
	std::cout << ctx << '\n';
}

void logger::Debug(const std::string& ctx, const bool& write)
{
	if (!config::debug)
		return;

	SetConsoleTextAttribute(hConsole, Colors::LightBlue);
	std::cout << "[Dbg]";

	SetConsoleTextAttribute(hConsole, Colors::LightGray);
	std::cout << ctx << '\n';

	if (write)
		WriteLog(ctx);
}

std::string logger::Input(const std::string& question, const std::vector<std::string>& allowedInput, const std::string& inputPrompt)
{
	std::string response = "";
	do
	{
		if (!response.empty()) 
		{
			SetConsoleTextAttribute(hConsole, Colors::Red);
			std::cout << "[X]";

			SetConsoleTextAttribute(hConsole, Colors::LightGray);
			std::cout << "Invalid input, pls try again\n";
		}

		SetConsoleTextAttribute(hConsole, Colors::Cyan);
		std::cout << "[?]";

		SetConsoleTextAttribute(hConsole, Colors::LightGray);
		std::cout << question << '\n';

		SetConsoleTextAttribute(hConsole, Colors::DarkGray);
		std::cout << inputPrompt + " ";
		
		SetConsoleTextAttribute(hConsole, Colors::LightGray);
		std::cin >> response;

	} while (std::find(allowedInput.begin(), allowedInput.end(), response) == allowedInput.end());
}

void logger::Colored(const std::string& ctx, const int& color)
{
	SetConsoleTextAttribute(hConsole, color);
	std::cout << ctx << '\n';
}

void logger::PreviousLine()
{
	if (!config::allowLineOperstions)
		return;

	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		csbi.dwCursorPosition.Y--;
		SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);

		DWORD written;
		FillConsoleOutputCharacter(hConsole, ' ', 80, csbi.dwCursorPosition, &written);
		SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
	}
}
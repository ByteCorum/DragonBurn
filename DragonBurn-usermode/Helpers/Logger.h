#pragma once
#include <fstream>
#include <iostream>
#include <string_view>
#include <Windows.h>

namespace Log 
{
        inline const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        inline constexpr std::string_view LogFile = "Logs.txt";

        inline bool WriteLog(std::string_view ctx)
        {
                std::ofstream file(LogFile.data(), std::ios::app | std::ios::binary);
                if (!file.is_open())
                        return false;

                file.write(ctx.data(), static_cast<std::streamsize>(ctx.size()));

                return true;

        }

        inline void Info(std::string_view ctx)
        {
                SetConsoleTextAttribute(hConsole, 11);
                std::cout << "[i]";

                SetConsoleTextAttribute(hConsole, 7);
		std::cout << ctx << '\n';
	}

        inline void Warning(std::string_view ctx, bool pause = false)
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

        inline void Error(std::string_view ctx, bool fatal = true)
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

        inline void Fine(std::string_view ctx)
        {
                SetConsoleTextAttribute(hConsole, 2);
                std::cout << "[+]";

                SetConsoleTextAttribute(hConsole, 7);
                std::cout << ctx << '\n';
        }

        inline void Debug(std::string_view ctx, bool write = false)
        {
#ifdef DBDEBUG
                std::string line;
                line.reserve(8 + ctx.size());
                line.append("[Debug]");
                line.append(ctx);
                line.push_back('\n');

                SetConsoleTextAttribute(hConsole, 9);
                std::cout << line;

                if (write)
			WriteLog(line);
#endif
	}

        inline void Custom(std::string_view ctx, int color)
        {
                SetConsoleTextAttribute(hConsole, color);
                std::cout << ctx << '\n';
        }

        inline void PreviousLine()
        {
                CONSOLE_SCREEN_BUFFER_INFO csbi;

                if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
                        csbi.dwCursorPosition.Y--;
                        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);

                        DWORD written;
                        FillConsoleOutputCharacter(hConsole, ' ', static_cast<DWORD>(csbi.dwSize.X), csbi.dwCursorPosition, &written);
                        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
                }
        }
}

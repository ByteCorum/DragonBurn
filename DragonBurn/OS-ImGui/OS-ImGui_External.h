#pragma once
#include "OS-ImGui_Base.h"

namespace OSImGui
{
	inline HHOOK g_keyboard_hook;
	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	class OSImGui_External : public OSImGui_Base
	{
	private:
		WindowType Type = NEW;
	public:
		void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack);
		void AttachAnotherWindow(std::string DestWindowName, std::string DestWindowClassName, std::function<void()> CallBack);
	private:
		void MainLoop();
		bool UpdateWindowData();
		bool CreateMyWindow();
		bool PeekEndMessage();
	};
}
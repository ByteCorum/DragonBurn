#pragma once
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <sstream>
#include <string>
#include <thread>
#include <time.h>
#include <vector>
#include <Windows.h>

#include "Legitbot.hpp"
#include "..\Resources\Sounds.h"
#include "..\Entity.h"
#include "..\MenuConfig.hpp"
#include "..\OS-ImGui\imgui\imgui.h"
#include "..\Utils\GlobalVars.h"

#pragma comment(lib, "winmm.lib")

namespace Misc
{
	inline bool Zoom = false;

	static inline std::vector<ImColor> colorList = {
		ImColor(255, 0, 0),     // Red
		ImColor(255, 165, 0),   // Orange
		ImColor(255, 255, 0),   // Yellow
		ImColor(0, 128, 0),     // Green
		ImColor(0, 0, 255),     // Purple
	};

	struct Vector3
	{
		float x, y, z;
		Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	};

	// Convert std::string into std::wstring
	static inline std::wstring STR2LPCWSTR(const std::string& str)
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(size, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
		return result;
	}

	static inline ImColor getRandomColor() {
		srand(time(0));
		int index = rand() % colorList.size();
		return colorList[index];
	}

	static inline void CheatText(const char* Cheat, bool config)
	{
		if (config)
			ImGui::Text(Cheat);
	}

	static inline void getCurrentTime(struct tm* ptm) {
		time_t now = time(NULL);
		localtime_s(ptm, &now);
	}

	static inline void StopKeyEvent(int WalkKey, bool* KeyStatus, int StopKey, float duration) {
		if (GetAsyncKeyState(WalkKey) & 0x8000) {
			*KeyStatus = true;
		}
		else {
			if (*KeyStatus) {
				keybd_event(StopKey, MapVirtualKey(StopKey, 0), KEYEVENTF_SCANCODE, 0);
				Sleep(50);
				keybd_event(StopKey, MapVirtualKey(StopKey, 0), KEYEVENTF_KEYUP, 0);
				*KeyStatus = false;
			}
		}
	}

	static inline uintptr_t GetSmokeEntity(int i, uintptr_t EntityListEntry) {
		uintptr_t Entity = EntityListEntry + 0x78 * (i + 1);
		return Entity;
	}

	inline bool AirCheck(const CEntity& Local)
	{
		const bool hasFlagInAir = Local.Pawn.HasFlag(PlayerPawn::Flags::IN_AIR);
		return hasFlagInAir;
	}
	inline bool CrouchCheck(const CEntity& Local)
	{
		const bool hasFlagDucking = Local.Pawn.HasFlag(PlayerPawn::Flags::IN_CROUCH);
		return hasFlagDucking;
	}

	class HitMarker 
	{
	public:
		const static float SIZE;
		const static float GAP;

		HitMarker(float alpha, std::chrono::steady_clock::time_point startTime)
		{
			this->_alpha = alpha;
			this->_startTime = startTime;
		}

		void Draw()
		{
			ImGuiIO& io = ImGui::GetIO();
			ImVec2 center = ImVec2(Gui.Window.Size.x / 2, Gui.Window.Size.y / 2);

			if (this->_alpha > 0.f)
			{
				ImColor col = ImColor(255.f, 255.f, 255.f, this->_alpha);

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x - SIZE, center.y - SIZE), ImVec2(center.x - GAP, center.y - GAP), col & IM_COL32_A_MASK, 2.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x - SIZE, center.y + SIZE), ImVec2(center.x - GAP, center.y + GAP), col & IM_COL32_A_MASK, 2.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x + SIZE, center.y - SIZE), ImVec2(center.x + GAP, center.y - GAP), col & IM_COL32_A_MASK, 2.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x + SIZE, center.y + SIZE), ImVec2(center.x + GAP, center.y + GAP), col & IM_COL32_A_MASK, 2.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x - SIZE, center.y - SIZE), ImVec2(center.x - GAP, center.y - GAP), col, 1.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x - SIZE, center.y + SIZE), ImVec2(center.x - GAP, center.y + GAP), col, 1.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x + SIZE, center.y - SIZE), ImVec2(center.x + GAP, center.y - GAP), col, 1.4f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x + SIZE, center.y + SIZE), ImVec2(center.x + GAP, center.y + GAP), col, 1.4f);
			}
		}

		void Update() 
		{
			if (this->_alpha > 0.f)
			{
				auto now = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->_startTime).count();
				if (duration >= 500.f)
					this->_alpha = 0;
				else
					this->_alpha = 1.f - duration / 500.f;
				Draw();
			}
		}

	private:
		float _alpha;
		std::chrono::steady_clock::time_point _startTime;

	};

	void Watermark(const CEntity& aLocalPlayer) noexcept;
	void HitManager(const CEntity& aLocalPlayer, int& PreviousTotalHits) noexcept;
	//void FastStop() noexcept;// junk
}
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

#include "Aimbot.h"
#include "..\Resources\Sounds.hpp"
#include "..\Game\Entity.h"
#include "..\Core\Config.h"
#include "..\OS-ImGui\imgui\imgui.h"
#include "..\Core\GlobalVars.h"

#pragma comment(lib, "winmm.lib")

namespace Misc
{
	enum KeyboardLayout {
		QWERTY = 0,
		AZERTY = 1,
		QWERTZ = 2,
		OTHER  = -1
	};

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

	

	static inline uintptr_t GetSmokeEntity(int i, uintptr_t EntityListEntry) {
		uintptr_t Entity = EntityListEntry + 0x78 * (i + 1);
		return Entity;
	}

	//inline bool AirCheck(const CEntity& Local)
	//{
	//	const bool hasFlagInAir = Local.Pawn.HasFlag(PlayerPawn::Flags::IN_AIR);
	//	return hasFlagInAir;
	//}
	//inline bool CrouchCheck(const CEntity& Local)
	//{
	//	const bool hasFlagDucking = Local.Pawn.HasFlag(PlayerPawn::Flags::IN_CROUCH);
	//	return hasFlagDucking;
	//}

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

	// New helper function for key events
	static void PerformKeyStop(int opposite_key, int delay_ms);

	// New helper function to check and stop keys
	static void CheckAndStopKey(int key, int opposite_key, std::map<int, bool>& key_released_map, std::chrono::steady_clock::time_point& last_stop_time);
	static inline int DetectKeyboardLayout()
	{
		char layoutName[KL_NAMELENGTH];
		if (GetKeyboardLayoutNameA(layoutName))
		{
			std::string sLayoutName(layoutName);
			std::transform(sLayoutName.begin(), sLayoutName.end(), sLayoutName.begin(), ::toupper);

			// QWERTY family
			if (sLayoutName == "00000409" || sLayoutName == "00000809" ||
				sLayoutName == "00001009" || sLayoutName == "00001809" ||
				sLayoutName == "00004009" || sLayoutName == "00010409" ||
				sLayoutName == "00020409" || sLayoutName == "00030409" ||
				sLayoutName == "00040409" || sLayoutName == "00050409" ||
				sLayoutName == "00000419" || // Russian
				sLayoutName == "00000422")   // Ukrainian
			{
				return QWERTY;
			}

			// AZERTY family
			if (sLayoutName == "0000040C" || sLayoutName == "0000080C" ||
				sLayoutName == "0000100C" || sLayoutName == "0000140C" ||
				sLayoutName == "0000180C")
			{
				return AZERTY;
			}

			// QWERTZ family
			if (sLayoutName == "00000407" || sLayoutName == "00000807" || // German
				sLayoutName == "00000C07" || // Austrian
				sLayoutName == "0000040E" || // Hungarian
				sLayoutName == "0000041B" || // Slovak
				sLayoutName == "00000405")   // Czech
			{
				return QWERTZ;
			}
		}
		return OTHER; // inconnu
	}

	void Watermark(const CEntity&) noexcept;
	void HitManager(CEntity&, int&) noexcept;
	void BunnyHop(const CEntity&) noexcept;
	void CleanTraces();
	void FastStop() noexcept;
}
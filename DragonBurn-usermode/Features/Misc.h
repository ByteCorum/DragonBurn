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
#include "..\Resources\Sounds.h"
#include "..\Game\Entity.h"
#include "..\Core\Config.h"
#include "..\OS-ImGui\imgui\imgui.h"
#include "..\Core\GlobalVars.h"

#pragma comment(lib, "winmm.lib")

namespace Misc
{
	enum class KeyboardLayout {
		QWERTY,
		AZERTY,
		QWERTZ,
		COLEMAK,
		DVORAK,
		UNKNOWN
	};

	struct KeyLayout {
		int forward, backward, left, right;
	};

	static KeyboardLayout Layout = KeyboardLayout::UNKNOWN;

	static std::map<KeyboardLayout, KeyLayout> keyLayouts = {
		{KeyboardLayout::QWERTY, {'W', 'S', 'A', 'D'}},
		{KeyboardLayout::AZERTY, {'Z', 'S', 'Q', 'D'}},
		{KeyboardLayout::QWERTZ, {'W', 'S', 'A', 'D'}},
		{KeyboardLayout::COLEMAK, {'W', 'R', 'A', 'S'}},
		{KeyboardLayout::DVORAK, {',', 'O', 'A', 'E'}}
	};

	inline bool Zoom = false;
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> dis(-40, 40);

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
		uintptr_t Entity = EntityListEntry + 0x70 * (i + 1);
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

	
			static inline KeyboardLayout DetectKeyboardLayout()
	{
		char layoutName[KL_NAMELENGTH];
		if (GetKeyboardLayoutNameA(layoutName))
		{
			std::string sLayoutName(layoutName);
			std::transform(sLayoutName.begin(), sLayoutName.end(), sLayoutName.begin(), ::toupper);

			// QWERTY-based layouts
			// This is a comprehensive list of layouts that are based on the QWERTY physical layout.
			if (sLayoutName == "00000409" || sLayoutName == "00000809" ||
				sLayoutName == "00001009" || sLayoutName == "00001809" ||
				sLayoutName == "00004009" || sLayoutName == "00020409" ||
				sLayoutName == "00050409" || sLayoutName == "00000419" ||
				sLayoutName == "00000422" || sLayoutName == "0000041C" ||
				sLayoutName == "00000401" || sLayoutName == "00010401" ||
				sLayoutName == "0000042B" || sLayoutName == "00000423" ||
				sLayoutName == "00010402" || sLayoutName == "00000804" ||
				sLayoutName == "00000406" || sLayoutName == "00000413" ||
				sLayoutName == "00000425" || sLayoutName == "0000040B" ||
				sLayoutName == "00000408" || sLayoutName == "0000040D" ||
				sLayoutName == "0000040F" || sLayoutName == "00000410" ||
				sLayoutName == "00000411" || sLayoutName == "00000412" ||
				sLayoutName == "00000426" || sLayoutName == "00000427" ||
				sLayoutName == "00000414" || sLayoutName == "00000415" ||
				sLayoutName == "00000416" || sLayoutName == "00000816" ||
				sLayoutName == "00000418" || sLayoutName == "0000040A" ||
				sLayoutName == "0000041D" || sLayoutName == "0000041F" ||
				sLayoutName == "00140C00" || sLayoutName == "0000044D" ||
				sLayoutName == "0000042C" || sLayoutName == "00000445" ||
				sLayoutName == "0000045C" || sLayoutName == "00000404" ||
				sLayoutName == "00000439" || sLayoutName == "00000438" ||
				sLayoutName == "00020437" || sLayoutName == "00000468" ||
				sLayoutName == "0000044B" || sLayoutName == "00000454" ||
				sLayoutName == "0000042F" || sLayoutName == "00090C00" ||
				sLayoutName == "00000448" || sLayoutName == "00000463" ||
				sLayoutName == "00000485" || sLayoutName == "00030C00" ||
				sLayoutName == "0000042A" || sLayoutName == "0000046D" ||
				sLayoutName == "00000429" || sLayoutName == "0002042B" ||
				sLayoutName == "0003042B" || sLayoutName == "0001042B" ||
				sLayoutName == "0001042C" || sLayoutName == "00020445" ||
				sLayoutName == "00010445" || sLayoutName == "00000813" ||
				sLayoutName == "000B0C00" || sLayoutName == "00020402" ||
				sLayoutName == "00040402" || sLayoutName == "00000402" ||
				sLayoutName == "00011009" || sLayoutName == "0000085F" ||
				sLayoutName == "0001045C" || sLayoutName == "00001004" ||
				sLayoutName == "00000C04" || sLayoutName == "00001404" ||
				sLayoutName == "00010405" || sLayoutName == "00020405" ||
				sLayoutName == "00000437" || sLayoutName == "00010408" ||
				sLayoutName == "00020408" || sLayoutName == "00030408" ||
				sLayoutName == "0000046F" || sLayoutName == "00000447" ||
				sLayoutName == "00010439" || sLayoutName == "00000470" ||
				sLayoutName == "00000421" || sLayoutName == "0000085D" ||
				sLayoutName == "0001045D" || sLayoutName == "0000043F" ||
				sLayoutName == "00000453" || sLayoutName == "00000440" ||
				sLayoutName == "0000080A" || sLayoutName == "0000046E" ||
				sLayoutName == "0000044C" || sLayoutName == "0000043A" ||
				sLayoutName == "0001043A" || sLayoutName == "00000481" ||
				sLayoutName == "0000044E" || sLayoutName == "00000450" ||
				sLayoutName == "00000850" || sLayoutName == "00000461" ||
				sLayoutName == "00010415" || sLayoutName == "00000446" ||
				sLayoutName == "00020418" || sLayoutName == "00010418" ||
				sLayoutName == "00010419" || sLayoutName == "0000083B" ||
				sLayoutName == "0000043B" || sLayoutName == "00000C1A" ||
				sLayoutName == "0000081A" || sLayoutName == "0000046C" ||
				sLayoutName == "00000432" || sLayoutName == "0000045B" ||
				sLayoutName == "00000424" || sLayoutName == "0000082E" ||
				sLayoutName == "0000042E" || sLayoutName == "0000045A" ||
				sLayoutName == "00000428" || sLayoutName == "00000449" ||
				sLayoutName == "00000444" || sLayoutName == "0000044A" ||
				sLayoutName == "0000041E" || sLayoutName == "00000451" ||
				sLayoutName == "0001041F" || sLayoutName == "00000442" ||
				sLayoutName == "00000420" || sLayoutName == "00000480" ||
				sLayoutName == "00000843" || sLayoutName == "00000452" ||
				sLayoutName == "00000488" || sLayoutName == "0000046A")
				return KeyboardLayout::QWERTY;

			// Dvorak layouts
			if (sLayoutName == "00010409" || sLayoutName == "00030409" || sLayoutName == "00040409")
				return KeyboardLayout::DVORAK;

			// Colemak layouts
			if (sLayoutName == "00060409")
				return KeyboardLayout::COLEMAK;

			// AZERTY-based layouts
			if (sLayoutName == "0000040C" || sLayoutName == "0000080C" ||
				sLayoutName == "0000100C" || sLayoutName == "0000140C" ||
				sLayoutName == "0000180C" || sLayoutName == "0001080C" ||
				sLayoutName == "00000C0C" || sLayoutName == "00020401")
				return KeyboardLayout::AZERTY;

			// QWERTZ-based layouts
			if (sLayoutName == "00000407" || sLayoutName == "00000807" ||
				sLayoutName == "00000C07" || sLayoutName == "0000040E" ||
				sLayoutName == "0000041B" || sLayoutName == "00000405" ||
				sLayoutName == "0000041A" || sLayoutName == "0000201A" ||
				sLayoutName == "0000082C")
				return KeyboardLayout::QWERTZ;
		}
		return KeyboardLayout::UNKNOWN;
	}

	void Watermark(const CEntity&) noexcept;
	void HitManager(CEntity&, int&) noexcept;
	void BunnyHop(const CEntity&) noexcept;
	void CleanTraces();

	void FastStop() noexcept;
	void KnifeBot(const CEntity& local, const std::vector<CEntity>& entities, int autoKnifeKey = 0) noexcept;
	void ExecuteCommand(const std::string& command) noexcept;
	void ZeusBot(const CEntity& local) noexcept;
	void AntiAFKKickUpdate() noexcept;

	namespace AutoAccept
	{
		void StartAutoAccept();
		void StopAutoAccept();
		bool DetectAcceptButton();
		void ClickAcceptButton(int x, int y);
		bool IsGreenPixel(COLORREF color);
		void UpdateAutoAccept();
	}
}
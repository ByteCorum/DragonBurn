#include "Misc.h"
#include "..\Resources\Language.hpp"
#include <iostream>
#include <Shellapi.h>
#include <filesystem>
#include "..\Core\Cheats.h"
namespace fs = std::filesystem;

namespace System {
	static inline void Key_Click_HWND(HWND hwnd, int vk, BOOL sleep_ = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, WM_KEYDOWN, vk, 0);
		if (sleep_) Sleep(1);
		SendMessage(hwnd, WM_KEYUP, vk, 0);
		if (sleep_) Sleep(1);
	}
	static inline void Key_Con_HWND(HWND hwnd, int vk, BOOL way = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, way ? WM_KEYDOWN : WM_KEYUP, vk, 0);
	}
}

namespace Misc
{
	//bool aKeyPressed = false;
	//bool dKeyPressed = false;
	//bool wKeyPressed = false;
	//bool sKeyPressed = false;
	HitMarker hitMarker(0, std::chrono::steady_clock::now());
	const float HitMarker::SIZE = 10.f;
	const float HitMarker::GAP = 3.f;

	void Watermark(const CEntity& LocalPlayer) noexcept
	{
		if (!MiscCFG::WaterMark || (LocalPlayer.Pawn.TeamID == 0 && !MenuConfig::ShowMenu))
			return;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowPos(MenuConfig::MarkWinPos, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(0.8f);

		ImGui::Begin("Watermark", nullptr, windowFlags);

		if (MenuConfig::MarkWinChengePos)
		{
			ImGui::SetWindowPos("Watermark", MenuConfig::MarkWinPos);
			MenuConfig::MarkWinChengePos = false;
		}

		Vec3 Pos = LocalPlayer.Pawn.Pos;
		int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
		char fpsText[32];
		snprintf(fpsText, sizeof(fpsText), "  FPS: %d", currentFPS);

		ImGui::Text("  DragonBurn");
		ImGui::Text("  Kernel CS2 cheat");
		ImGui::Text("  Velocity: %.2f", LocalPlayer.Pawn.Speed);
		ImGui::Text("%s", fpsText);
		//ImGui::Text("  Pos: %.1f, %.1f, %.1f ", Pos.x, Pos.y, Pos.z);
		ImGui::Text("                                                      ");
		ImGui::Text("  Map: %s", Cheats::GetCurrentMapName().c_str());

		MenuConfig::MarkWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}

	void HitSound() noexcept
	{
		switch (MiscCFG::HitSound)
		{
		case 1:
			PlaySoundA(reinterpret_cast<char*>(neverlose_sound), NULL, SND_ASYNC | SND_MEMORY);
			break;
		case 2:
			PlaySoundA(reinterpret_cast<char*>(skeet_sound), NULL, SND_ASYNC | SND_MEMORY);
			break;
		default:
			break;
		}
	}

	void HitManager(CEntity& LocalPlayer, int& PreviousTotalHits) noexcept
	{
		if ((!MiscCFG::HitSound && !MiscCFG::HitMarker) || LocalPlayer.Controller.TeamID == 0 || MenuConfig::ShowMenu || !LocalPlayer.IsAlive())
			return;

		uintptr_t pBulletServices;
		int totalHits;
		memoryManager.ReadMemory(LocalPlayer.Pawn.Address + Offset.Pawn.BulletServices, pBulletServices);
		memoryManager.ReadMemory(pBulletServices + Offset.Pawn.TotalHit, totalHits);

		if (totalHits != PreviousTotalHits) {
			if (totalHits == 0 && PreviousTotalHits != 0)
			{
				// `totalHits` changed from non-zero to zero, do nothing
			}
			else
			{
				if (MiscCFG::HitSound)
				{
					HitSound();
				}
				if (MiscCFG::HitMarker)
				{
					hitMarker = HitMarker(255.f, std::chrono::steady_clock::now());
					hitMarker.Draw();
				}
			}
		}

		hitMarker.Update();
		PreviousTotalHits = totalHits;
	}

	void BunnyHop(const CEntity& Local) noexcept
	{
		if (!MiscCFG::BunnyHop ||  MenuConfig::ShowMenu || Local.Controller.TeamID == 0)
			return;

		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (hwnd_cs2 == NULL) {
			hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		}

		//int JumpBtn;
		//if (!memoryManager.ReadMemory(gGame.GetJumpBtnAddress(), JumpBtn))
		//	return;

		bool spacePressed = GetAsyncKeyState(VK_SPACE);
		//bool isInAir = AirCheck(Local);

		static DWORD lastJumped = GetTickCount64();
		DWORD currentTick = GetTickCount64();

		if (spacePressed /*&& isInAir*/)
		{
			if (currentTick - lastJumped >= MenuConfig::BunnyHopDelay)
			{
				SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
				SendMessage(hwnd_cs2, WM_KEYDOWN, VK_SPACE, 0);
				lastJumped = currentTick;
			}
		}
		//else if (spacePressed /*&& !isInAir*/)
		//{
		//	SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
		//}
		//else if (!spacePressed)
		//{
		//	SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
		//}
	}

	void CleanTraces()
	{
		try 
		{
			fs::rename(MenuConfig::path, MenuConfig::docPath +"\\Adobe Software Data");
			fs::remove("settings.yml");

			//std::string current_path = fs::current_path().string();
			//std::string current_dir = fs::current_path().parent_path().string();
		}
		catch (...) {}
	}

	//void FastStop() noexcept
	//{
	//	if (!MiscCFG::FastStop)
	//		return;
	//	// Disable when bhopping
	//	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//		return;
	//	// Disable when slow walking
	//	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	//		return;

	//	Misc::StopKeyEvent('A', &aKeyPressed, 'D', 50.f);
	//	Misc::StopKeyEvent('D', &dKeyPressed, 'A', 50.f);
	//	Misc::StopKeyEvent('W', &wKeyPressed, 'S', 50.f);
	//	Misc::StopKeyEvent('S', &sKeyPressed, 'W', 50.f);
	//}

	void ExecuteCommand(const std::string& Command_Str) noexcept
	{
		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (!hwnd_cs2) return;
	
		COPYDATASTRUCT cds;
		cds.cbData = static_cast<DWORD>(Command_Str.size() + 1);
		cds.dwData = 0;
		cds.lpData = (void*)Command_Str.c_str();
		SendMessage(hwnd_cs2, WM_COPYDATA, 0, (LPARAM)&cds);
	
		// Key/mouse fallbacks mirroring original behavior
		if (Command_Str == "+jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, TRUE);
		else if (Command_Str == "-jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, FALSE);
	
		if (Command_Str == "+duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, TRUE);
		else if (Command_Str == "-duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, FALSE);
	
		if (Command_Str == "+lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, TRUE); // 'F'
		else if (Command_Str == "-lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, FALSE);
	
		if (Command_Str == "drop") System::Key_Click_HWND(hwnd_cs2, 0x47, TRUE); // 'G'
	
		if (Command_Str == "+attack") mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack") mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+attack2") mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack2") mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+forward") System::Key_Con_HWND(hwnd_cs2, 0x57, TRUE); // 'W'
		else if (Command_Str == "-forward") System::Key_Con_HWND(hwnd_cs2, 0x57, FALSE);
	
		if (Command_Str == "+back") System::Key_Con_HWND(hwnd_cs2, 0x53, TRUE); // 'S'
		else if (Command_Str == "-back") System::Key_Con_HWND(hwnd_cs2, 0x53, FALSE);
	
		if (Command_Str == "+left") System::Key_Con_HWND(hwnd_cs2, 0x41, TRUE); // 'A'
		else if (Command_Str == "-left") System::Key_Con_HWND(hwnd_cs2, 0x41, FALSE);
	
		if (Command_Str == "+right") System::Key_Con_HWND(hwnd_cs2, 0x44, TRUE); // 'D'
		else if (Command_Str == "-right") System::Key_Con_HWND(hwnd_cs2, 0x44, FALSE);
	
	}
	
	void AutoKnifeExecute(const CEntity& Local, const std::vector<CEntity>& Entities, int autoKnifeKey) noexcept
	{
		// Weapon check: knife only
		if (!(Local.Pawn.WeaponName == "ct_knife" || Local.Pawn.WeaponName == "t_knife"))
			return;
	
		// Optional hotkey gate: if key provided, require it pressed
		if (autoKnifeKey != 0 && !(GetAsyncKeyState(autoKnifeKey) & 0x8000))
			return;
	
		const Vec3 localPos = Local.Pawn.Pos;
		const Vec3 eyePos = Local.Pawn.CameraPos; // local position + view offset
		const Vec2 viewAngles = Local.Pawn.ViewAngle;
	
		auto normalizeAngle = [](float a) -> float {
			while (a > 180.0f) a -= 360.0f;
			while (a < -180.0f) a += 360.0f;
			return a;
		};
	
		auto calcAngleDelta = [&](const Vec3& src, const Vec3& dst, const Vec2& current) -> Vec2 {
			const float dx = dst.x - src.x;
			const float dy = dst.y - src.y;
			const float dz = dst.z - src.z;
			const float hyp = sqrtf(dx * dx + dy * dy);
			Vec2 aim;
			aim.x = -atan2f(dz, hyp) * 57.2957795f;
			aim.y = atan2f(dy, dx) * 57.2957795f;
			Vec2 delta;
			delta.x = normalizeAngle(aim.x - current.x);
			delta.y = normalizeAngle(aim.y - current.y);
			return delta;
		};
	
		for (const auto& E : Entities)
		{
			if (!E.IsAlive())
				continue;
			if (E.Controller.TeamID == Local.Controller.TeamID)
				continue;
	
			const Vec3 enemyPos = E.Pawn.Pos;
			// Use head bone if available
			Vec3 targetPos = enemyPos;
			const auto bone = E.GetBone();
			if (!bone.BonePosList.empty()) {
				size_t headIndex = BONEINDEX::head;
				if (headIndex < bone.BonePosList.size()) {
					targetPos = bone.BonePosList[headIndex].Pos;
				}
			}
	
			// Distance check (world units)
			const float dx = localPos.x - enemyPos.x;
			const float dy = localPos.y - enemyPos.y;
			const float dz = localPos.z - enemyPos.z;
			const float dist = sqrtf(dx * dx + dy * dy + dz * dz);
			if (dist > 70.0f)
				continue;
	
			// Angle delta check
			const Vec2 delta = calcAngleDelta(eyePos, targetPos, viewAngles);
			if (hypotf(delta.x, delta.y) > 40.0f)
				continue;
	
			// Heavy vs light attack condition
			const int enemyHp = E.Pawn.Health;
			const bool facing = fabsf(normalizeAngle(viewAngles.y - E.Pawn.ViewAngle.y)) <= 50.0f;
			if ((enemyHp <= 55 && enemyHp > 30) || facing)
			{
				ExecuteCommand("+attack2");
				Sleep(1);
				ExecuteCommand("-attack2");
			}
			else
			{
				ExecuteCommand("+attack");
				Sleep(1);
				ExecuteCommand("-attack");
			}
		}
	}
	
	void zeusbot(const CEntity& Local, const std::vector<CEntity>& Entities) noexcept
	{
		if (Local.Pawn.WeaponName != "zeus")
			return;
	
		const Vec3 localPos = Local.Pawn.Pos;
	
		for (const auto& E : Entities)
		{
			if (!E.IsAlive())
				continue;
			if (E.Controller.TeamID == Local.Controller.TeamID)
				continue;
	
			const Vec3 enemyPos = E.Pawn.Pos;
			const float dx = localPos.x - enemyPos.x;
			const float dy = localPos.y - enemyPos.y;
			const float dz = localPos.z - enemyPos.z;
			const float dist = sqrtf(dx * dx + dy * dy + dz * dz);
			if (dist > 130.0f)
				continue;
	
			ExecuteCommand("+attack");
			Sleep(1);
			ExecuteCommand("-attack");
			break;
		}
	}
	
	void AntiAFKKickUpdate() noexcept
	{
		if (!MiscCFG::AntiAFKKick) return;
		static DWORD64 s_lastTick = 0;
		DWORD64 now = GetTickCount64();
		if (now - s_lastTick >= 5000) { // fixed 5s
			mouse_event(MOUSEEVENTF_MOVE, 1, 0, 0, 0);
			Sleep(1);
			mouse_event(MOUSEEVENTF_MOVE, -1, 0, 0, 0);
			s_lastTick = now;
		}
	}
}
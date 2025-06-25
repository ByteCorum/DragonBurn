#include "Misc.h"
#include "..\Resources\Language.hpp"
#include <iostream>
#include <Shellapi.h>
#include <filesystem>
namespace fs = std::filesystem;

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
		if ((!MiscCFG::WaterMark || LocalPlayer.Controller.TeamID == 0) && !(MiscCFG::WaterMark && MenuConfig::ShowMenu))
			return;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowPos(MenuConfig::MarkWinPos, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::GetStyle().WindowRounding = 8.0f;
		ImGui::Begin("Watermark", nullptr, windowFlags);

		if (MenuConfig::MarkWinChengePos)
		{
			ImGui::SetWindowPos("Watermark", MenuConfig::MarkWinPos);
			MenuConfig::MarkWinChengePos = false;
		}

		Vec3 Pos = LocalPlayer.Pawn.Pos;

		int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
		char fpsText[32];
		snprintf(fpsText, sizeof(fpsText), " FPS: [%d]", currentFPS);

		ImGui::Text("  DragonBurn");
		ImGui::Text("  Kernel CS2 cheat");
		ImGui::Text("%s", fpsText);
		//ImGui::Text("  Vel: %.2f", LocalPlayer.Pawn.Speed);
		//ImGui::Text("  Pos: %.1f, %.1f, %.1f ", Pos.x, Pos.y, Pos.z);
		ImGui::Text("                                                      ");

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
		{
			return;
		}

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
		bool isInAir = AirCheck(Local);

		if (spacePressed && isInAir)
		{
			//std::this_thread::sleep_for(std::chrono::microseconds(15625));
			SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
			SendMessage(hwnd_cs2, WM_KEYDOWN, VK_SPACE, 0);
		}

		else if (spacePressed && !isInAir)
		{
			SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
		}
		else if (!spacePressed)
		{
			SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
		}
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
}

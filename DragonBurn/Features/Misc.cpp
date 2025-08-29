#include "Misc.h"
#include "..\Resources\Language.hpp"
#include <iostream>
#include <Shellapi.h>
#include <filesystem>
namespace fs = std::filesystem;

namespace Misc
{
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

	void FastStop() noexcept
{
    if (!MiscCFG::FastStop)
        return;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_LSHIFT) & 0x8000)
        return;

    static std::map<int, bool> key_released_map;
    static auto last_stop_time = std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_stop_time).count() < MiscCFG::FastStopDelay + 20)
    {
        return;
    }

    auto check_and_stop = [&](int key, int opposite_key) {
        short key_state = GetAsyncKeyState(key);
        bool previously_released = key_released_map[key];

        if (key_state & 0x8000)
        {
            key_released_map[key] = true;
        }
        else if (previously_released)
        {
            int pressed_keys = 0;
            if (GetAsyncKeyState(MiscCFG::Azerty ? 'Q' : 'A') & 0x8000) pressed_keys++;
            if (GetAsyncKeyState('D') & 0x8000) pressed_keys++;
            if (GetAsyncKeyState(MiscCFG::Azerty ? 'Z' : 'W') & 0x8000) pressed_keys++;
            if (GetAsyncKeyState('S') & 0x8000) pressed_keys++;

            if (pressed_keys == 0)
            {
                last_stop_time = std::chrono::steady_clock::now();
                std::thread([opposite_key]() {
                    keybd_event(opposite_key, MapVirtualKey(opposite_key, 0), 0, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(MiscCFG::FastStopDelay));
                    keybd_event(opposite_key, MapVirtualKey(opposite_key, 0), KEYEVENTF_KEYUP, 0);
                    }).detach();
            }
            key_released_map[key] = false;
        }
    };

    if (MiscCFG::Azerty)
    {
        check_and_stop('Q', 'D');
        check_and_stop('D', 'Q');
        check_and_stop('Z', 'S');
        check_and_stop('S', 'Z');
    }
    else
    {
        check_and_stop('A', 'D');
        check_and_stop('D', 'A');
        check_and_stop('W', 'S');
        check_and_stop('S', 'W');
    }
}
}

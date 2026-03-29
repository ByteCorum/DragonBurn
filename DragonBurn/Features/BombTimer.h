#pragma once
#include <chrono>
#include <iostream>
#include <utility>
#include <sstream>
#include <ctime>
#include <string>
#include "..\Game\Entity.h"
#include "..\Core\Config.h"

namespace bmb
{
	bool isPlanted = false;
	std::time_t plantTime;

	uint64_t currentTimeMillis() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	int getBombSite(bool Planted)
	{
		if (Planted)
		{
			int site;
			uintptr_t cPlantedC4;
			if (!memoryManager.ReadMemory<uintptr_t>(gGame.GetClientDLLAddress() + Offset.PlantedC4, cPlantedC4))
				return 0;
			if (!memoryManager.ReadMemory<uintptr_t>(cPlantedC4, cPlantedC4))
				return 0;
			if (!memoryManager.ReadMemory<int>(cPlantedC4 + Offset.C4.m_nBombSite, site))
				return 0;

			return site;
		}
		else
			return 0;
		
	}

	void RenderWindow(int inGame)
	{
		if ((!MiscCFG::bmbTimer) || (inGame == 0 && !MenuConfig::ShowMenu))
			return;

		uintptr_t bomb;
		bool isBombPlanted;
		bool IsBeingDefused;
		float DefuseTime;
		float defuseRemaining;
		ImColor color = MiscCFG::BombTimerCol;
		auto plantedAddress = gGame.GetClientDLLAddress() + Offset.PlantedC4;

		memoryManager.ReadMemory(plantedAddress, bomb);
		memoryManager.ReadMemory(bomb, bomb);
		memoryManager.ReadMemory(plantedAddress - 0x8, isBombPlanted);

		auto time = currentTimeMillis();

		if (isBombPlanted && !isPlanted && (plantTime == NULL || time - plantTime > 60000))
		{
			isPlanted = true;
			plantTime = time;
		}

		memoryManager.ReadMemory(bomb + Offset.C4.m_bBeingDefused, IsBeingDefused);
		memoryManager.ReadMemory(bomb + Offset.C4.m_flDefuseCountDown, DefuseTime);

		if (!isPlanted && !MenuConfig::ShowMenu)
			return;

		static float windowWidth = 200.0f;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowPos(MenuConfig::BombWinPos, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ windowWidth, 0 }, ImGuiCond_Once);

		ImGui::Begin("Bomb Timer", nullptr, flags);

		if (MenuConfig::BombWinChengePos)
		{
			ImGui::SetWindowPos("Bomb Timer", MenuConfig::BombWinPos);
			MenuConfig::BombWinChengePos = false;
		}

		float remaining = (40000 - (int64_t)time + plantTime) / (float)1000;

		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 180) * 0.5f);
		float barLength = remaining <= 0.0f ? 0.0f : remaining >= 40 ? 1.0f : (remaining / 40.0f);
		
		if (isPlanted && remaining >= 0)
		{
			if (IsBeingDefused && remaining >= 5)
			{
				color = ImColor(32, 178, 170);
			}	
			else if (remaining <= 10)
			{
				color = ImColor(160, 48, 73);
			}
			else
			{
				color = MiscCFG::BombTimerCol;
			}

			std::ostringstream ss;
			ss.precision(3);
			ss << "Bomb on " << (!getBombSite(isBombPlanted) ? "A" : "B") << ": " << std::fixed << std::round(remaining * 1000.0) / 1000.0 << " s";
			Gui.MyText(std::move(ss).str().c_str(), true);
		}
		else {
			Gui.MyText("C4 not planted", true);
			barLength = 0.0f;
		}
		ImGui::TextUnformatted(" ");
		ImGui::SameLine();
		Gui.MyProgressBar(barLength, { 180, 15 }, "", color);

		if (isPlanted && remaining >= 0 && IsBeingDefused)
		{
			ImVec2 pos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(pos.x + 45, pos.y - 27));

			float defuseRemaining = 0.0f;
			DWORD64 globalVars = 0;
			if (memoryManager.ReadMemory<DWORD64>(gGame.GetClientDLLAddress() + Offset.GlobalVars, globalVars) && globalVars)
			{
				globalvars gv{ globalVars };
				if (gv.GetcurrentTime() && gv.g_fCurrentTime > 0.0f)
				{
					float defuseEndTime = 0.0f;
					if (memoryManager.ReadMemory<float>(bomb + Offset.C4.m_flDefuseCountDown, defuseEndTime) && defuseEndTime > 0.0f)
					{
						defuseRemaining = defuseEndTime - gv.g_fCurrentTime;
						if (defuseRemaining < 0.0f || defuseRemaining > 10.0f) defuseRemaining = 0.0f;
					}
				}
			}

			ImFont* font = ImGui::GetFont();
			float oldScale = font->Scale;
			font->Scale = 0.7f;
			ImGui::PushFont(font);
			ImGui::TextColored(ImColor(0, 0, 0, 160), "Defusing: %.3f s", defuseRemaining);
			font->Scale = oldScale;
			ImGui::PopFont();
		}
		if (isPlanted && !isBombPlanted)
		{
			isPlanted = false;
		}
		MenuConfig::BombWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}
}
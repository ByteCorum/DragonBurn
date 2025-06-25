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
		if ((!MiscCFG::bmbTimer || inGame == 0) && !(MiscCFG::bmbTimer && MenuConfig::ShowMenu))
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
		ImGui::GetStyle().WindowRounding = 8.0f;
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
			if (remaining <= 10 && !IsBeingDefused)
			{
				color = ImColor(113, 34, 51);
			}
			else if (IsBeingDefused)
			{
				color = ImColor(32, 178, 170);
			}
			else
			{
				color = MiscCFG::BombTimerCol;
			}

			std::ostringstream ss;
			ss.precision(3);
			ss << "Bomb on " << (!getBombSite(isBombPlanted) ? "A" : "B") << ": " << std::fixed << remaining << " s";
			Gui.MyText(std::move(ss).str().c_str(), true);
		}
		else {
			Gui.MyText("C4 not planted", true);
			barLength = 0.0f;
		}
		ImGui::TextUnformatted(" ");
		ImGui::SameLine();
		Gui.MyProgressBar(barLength, { 180, 15 }, "", color);
		//if (isPlanted && remaining >= 0 && IsBeingDefused)
		//{
		//	ImVec2 progressBarPos = ImGui::GetCursorPos();
		//	ImGui::SetCursorPos(ImVec2(progressBarPos.x + 45, progressBarPos.y - 27));

		//	std::ostringstream defuseSS;
		//	defuseSS.precision(2);
		//	defuseRemaining = plantTime - DefuseTime;
		//	defuseSS << "Defusing: " << std::fixed << defuseRemaining << " s";
		//	ImGui::Text(defuseSS.str().c_str());
		//}
		if (isPlanted && !isBombPlanted)
		{
			isPlanted = false;
		}
		MenuConfig::BombWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}
}
#include "ConfigMenu.hpp"
#include "../MenuConfig.hpp"
#include "ConfigSaver.hpp"
#include "../Features/TriggerBot.h"
#include "../Features/Legitbot.hpp"
#include <filesystem>
#include <string>
#include "../Resources/Language.h"
#include "../Features/RCS.h"

namespace ConfigMenu {
	
	void RenderCFGmenu()
	{
		static char configNameBuffer[128] = "NewConfig";
		static char configAuthorBuffer[128] = "Author";
		static int selectedConfig = -1;

		const std::string configDir = MenuConfig::path;
		static std::vector<std::string> configFiles;
		std::vector<const char*> configFilesCStr;

		configFiles.clear();
		for (const auto& entry : std::filesystem::directory_iterator(configDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".cfg")
			{
				configFiles.push_back(entry.path().filename().string());
			}
		}
		for (const auto& file : configFiles)
		{
			configFilesCStr.push_back(file.c_str());
		}

		float CursorX = 10.f;
		float CurrentCursorX = ImGui::GetCursorPosX();
		float ComponentWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - CursorX * 2;

		ImGui::SetCursorPos(ImVec2(15.f, 24.f));
		ImGui::SeparatorText(Lang::ConfigText.FeatureName);

		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(Lang::ConfigText.MyConfigs);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(ComponentWidth);
		ImGui::ListBox("##ConfigFiles", &selectedConfig, configFilesCStr.data(), configFilesCStr.size());
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		if (ImGui::Button(Lang::ConfigText.Load, { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
		{
			std::string selectedConfigFile = configFiles[selectedConfig];
			std::cout << selectedConfigFile << std::endl;
			MyConfigSaver::LoadConfig(selectedConfigFile);
		}
		ImGui::SameLine();
		if (ImGui::Button(Lang::ConfigText.Save, { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
		{
			std::string selectedConfigFile = configFiles[selectedConfig];
			MyConfigSaver::SaveConfig(selectedConfigFile);
		}

		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		if (ImGui::Button(Lang::ConfigText.Delete, { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
			ImGui::OpenPopup("##reallyDelete");
		if (ImGui::BeginPopup("##reallyDelete"))
		{
			ImGui::TextUnformatted(" Are you sure? ");
			ImGui::TextUnformatted("  ");
			ImGui::SameLine();
			if (ImGui::Button("No", { 40.0f, 0.0f }))
				ImGui::CloseCurrentPopup();
			ImGui::SameLine();
			if (ImGui::Button("Yes", { 40.0f, 0.0f }))
			{
				// Delete
				std::string selectedConfigFile = configFiles[selectedConfig];
				std::string fullPath = configDir + "\\" + selectedConfigFile;
				if (std::remove(fullPath.c_str()) == 0)
				{
					configFiles.erase(configFiles.begin() + selectedConfig);
					selectedConfig = -1;
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(Lang::ConfigText.Reset, { 120.f, 30.f }))
			ImGui::OpenPopup("##reallyReset");
		if (ImGui::BeginPopup("##reallyReset"))
		{
			ImGui::TextUnformatted(" Are you sure? ");
			ImGui::TextUnformatted("  ");
			ImGui::SameLine();
			if (ImGui::Button("No", { 40.0f, 0.0f }))
				ImGui::CloseCurrentPopup();
			ImGui::SameLine();
			if (ImGui::Button("Yes", { 40.0f, 0.0f }))
			{
				ConfigMenu::ResetToDefault();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::NewLine();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SeparatorText(Lang::ConfigText.SeparateLine);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(Lang::ConfigText.ConfigName);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(ComponentWidth + 8);
		ImGui::InputText("###ConfigNameInput", configNameBuffer, sizeof(configNameBuffer));
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(Lang::ConfigText.AuthorName);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(ComponentWidth + 8);
		ImGui::InputText("###AuthorNameInput", configAuthorBuffer, sizeof(configAuthorBuffer));
		ImGui::NewLine();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		if (ImGui::Button(Lang::ConfigText.Create, { 120.f, 30.f }))
		{
			std::string configFileName = std::string(configNameBuffer) + ".cfg";
			MyConfigSaver::SaveConfig(configFileName, std::string(configAuthorBuffer));
		}
		ImGui::SameLine();
		if (ImGui::Button(Lang::ConfigText.OpenFolder, { 120.f, 30.f }))
		{
			Gui.OpenWebpage(configDir.c_str());
		}
	}

	void ResetToDefault() {
		TriggerBot::IgnoreFlash = false;
		TriggerBot::ScopeOnly = false;
		AimControl::IgnoreFlash = false;
		ESPConfig::RenderDistance = 80;
		ESPConfig::ArmorBar = false;
		ESPConfig::ShowArmorNum = false;
		ESPConfig::ShowIsScoped = false;
		ESPConfig::AmmoBar = false;
		ESPConfig::OutLine = true;
		ESPConfig::ShowHealthNum = false;
		ESPConfig::FilledColor = ImColor(255, 255, 255, 255);
		ESPConfig::FilledColor2 = ImColor(255, 255, 255, 255);
		ESPConfig::MultiColor = false;
		ESPConfig::BoxFilledVisColor = ImColor(0, 0, 255, 255);
		ESPConfig::FilledVisBox = false;
		//MiscCFG::SpecList = false;
		MiscCFG::BombTimerCol = ImColor(255, 120, 0, 255);
		MiscCFG::bmbTimer = false;
		ESPConfig::VisibleColor = ImColor(255, 196, 0, 255);
		ESPConfig::VisibleCheck = false;
		MenuConfig::AirJump = false;
		MenuConfig::WindowStyle = 0;
		ESPConfig::BoxAlpha = 0.0f;
		ESPConfig::ShowPreview = true;
		ESPConfig::ShowHeadBox = false;
		ESPConfig::HeadBoxStyle = 0;
		ESPConfig::HeadBoxColor = ImColor(255, 255, 255, 255);
		ESPConfig::ShowDistance = false;
		ESPConfig::ShowBoneESP = true;
		ESPConfig::ShowBoxESP = true;
		ESPConfig::ShowHealthBar = true;
		ESPConfig::ShowWeaponESP = false;
		ESPConfig::ShowEyeRay = false;
		ESPConfig::ShowPlayerName = true;
		ESPConfig::BoxRounding = 0.0f;
		MenuConfig::AimBot = false;
		MenuConfig::AimToggleMode = false;
		MenuConfig::AimPosition = 0;
		MenuConfig::AimPositionIndex = BONEINDEX::head;
		MenuConfig::BoxType = 0;
		MenuConfig::HealthBarType = 0;
		ESPConfig::BoneColor = ImColor(0, 255, 255, 255);
		ESPConfig::BoxColor = ImColor(255, 80, 0, 255);
		ESPConfig::EyeRayColor = ImVec4(255, 0, 0, 255);
		MenuConfig::ShowMenu = true;
		RadarCFG::ShowRadar = false;
		RadarCFG::RadarRange = 150;
		RadarCFG::ShowRadarCrossLine = true;
		RadarCFG::RadarCrossLineColor = ImColor(220, 220, 220, 255);
		RadarCFG::RadarType = 2;
		RadarCFG::RadarPointSizeProportion = 1.f;
		RadarCFG::RadarBgAlpha = 0.1f;
		RadarCFG::Proportion = 3300;
		MenuConfig::TriggerBot = false;
		MenuConfig::TriggerAlways = false;
		MenuConfig::TeamCheck = true;
		MenuConfig::BypassOBS = false;
		MenuConfig::VisibleCheck = true;
		MenuConfig::ShowHeadShootLine = false;
		MenuConfig::HeadShootLineColor = ImColor(255, 255, 255, 200);
		MenuConfig::AimBotHotKey = 0;
		AimControl::SetHotKey(MenuConfig::AimBotHotKey);
		ESPConfig::ShowLineToEnemy = false;
		MenuConfig::FovLineSize = 60.0f;
		TriggerBot::TriggerDelay = 90;
		TriggerBot::FakeShotDelay = 500;
		RCS::RCSBullet = 1;
		MenuConfig::TriggerHotKey = 0;
		TriggerBot::SetHotKey(MenuConfig::TriggerHotKey);
		RCS::RCSScale = ImVec2(1.2f, 1.4f);
		AimControl::ScopeOnly = false;
		AimControl::AutoShot = false;
		MenuConfig::FovLineColor = ImVec4(55, 55, 55, 220);
		ESPConfig::LineToEnemyColor = ImVec4(255, 255, 255, 220);
		CrosshairsCFG::ShowCrossHair = false;
		CrosshairsCFG::CrossHairColor = ImColor(0, 255, 0, 255);
		CrosshairsCFG::CrossHairSize = 75;
		CrosshairsCFG::drawDot = true;
		CrosshairsCFG::tStyle = false;
		CrosshairsCFG::HorizontalLength = 6;
		CrosshairsCFG::VerticalLength = 6;
		CrosshairsCFG::drawOutLine = true;
		CrosshairsCFG::Gap = 8;
		CrosshairsCFG::drawCrossline = true;
		CrosshairsCFG::drawCircle = false;
		MenuConfig::TargetingCrosshairs = false;
		CrosshairsCFG::TargetedColor = ImColor(255, 0, 0, 255);
		CrosshairsCFG::CircleRadius = 3.f;
		CrosshairsCFG::DynamicGap = false;
		CrosshairsCFG::DotSize = 1.0f;
		CrosshairsCFG::Thickness = 1;
		CrosshairsCFG::TeamCheck = true;
		MiscCFG::WorkInSpec = true;
		MiscCFG::WaterMark = false;
		MiscCFG::HitSound = 0;
		MiscCFG::FastStop = false;

		ESPConfig::ESPenabled = false;

		ESPConfig::DrawFov = false;
		MenuConfig::FovCircleColor = ImColor(255, 255, 255, 255);

		MenuConfig::MaxRenderFPS = 1200;
	}
}

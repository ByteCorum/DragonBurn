#include "ConfigMenu.h"
#include "../Core/Config.h"
#include "ConfigSaver.h"
#include "../Features/TriggerBot.h"
#include "../Features/Aimbot.h"
#include <filesystem>
#include <string>
#include "../Resources/Language.h"
#include "../Features/RCS.h"
#include "../Helpers/KeyManager.h"

namespace ConfigMenu {
	
	void RenderCFGmenu()
	{
		ImGui::Columns(2, nullptr, false);
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
		ImGui::GradientText(Text::Config::FeatureName.c_str());

		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(Text::Config::MyConfigs.c_str());
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(ComponentWidth);
		ImGui::ListBox("##ConfigFiles", &selectedConfig, configFilesCStr.data(), configFilesCStr.size());
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		if (ImGui::Button(Text::Config::Load.c_str(), { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
		{
			std::string selectedConfigFile = configFiles[selectedConfig];
			MyConfigSaver::LoadConfig(selectedConfigFile);
		}
		ImGui::SameLine();
		if (ImGui::Button(Text::Config::Save.c_str(), { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
		{
			std::string selectedConfigFile = configFiles[selectedConfig];
			MyConfigSaver::SaveConfig(selectedConfigFile);
		}

		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		if (ImGui::Button(Text::Config::Delete.c_str(), { 120.f, 30.f }) && selectedConfig >= 0 && selectedConfig < configFiles.size())
			ImGui::OpenPopup("##reallyDelete");
		if (ImGui::BeginPopup("##reallyDelete"))
		{
			ImGui::Text("   Are you sure?   ");
			ImGui::Text(" ");
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
		if (ImGui::Button(Text::Config::Reset.c_str(), { 120.f, 30.f }))
			ImGui::OpenPopup("##reallyReset");
		if (ImGui::BeginPopup("##reallyReset"))
		{
			ImGui::Text("   Are you sure?   ");
			ImGui::Text(" ");
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

		ImGui::NextColumn();
		CurrentCursorX = ImGui::GetCursorPosX();
		ImGui::SetCursorPos(ImVec2(CurrentCursorX + CursorX, 24.f));
		ImGui::GradientText(Text::Config::SeparateLine.c_str());
		ImGui::TextDisabled(Text::Config::ConfigName.c_str());
		ImGui::SetNextItemWidth(ComponentWidth + 8);
		ImGui::InputText("###ConfigNameInput", configNameBuffer, sizeof(configNameBuffer));
		ImGui::TextDisabled(Text::Config::AuthorName.c_str());
		ImGui::SetNextItemWidth(ComponentWidth + 8);
		ImGui::InputText("###AuthorNameInput", configAuthorBuffer, sizeof(configAuthorBuffer));
		ImGui::NewLine();
		if (ImGui::Button(Text::Config::Create.c_str(), { 120.f, 30.f }))
		{
			std::string configFileName = std::string(configNameBuffer) + ".cfg";
			MyConfigSaver::SaveConfig(configFileName, std::string(configAuthorBuffer));
		}
		ImGui::SameLine();
		if (ImGui::Button(Text::Config::OpenFolder.c_str(), { 120.f, 30.f }))
		{
			Gui.OpenWebpage(configDir.c_str());
		}
		ImGui::Columns(1);
	}

	void ResetToDefault() {
		TriggerBot::IgnoreFlash = false;
		TriggerBot::StopedOnly = false;
		TriggerBot::ScopeOnly = true;
		AimControl::IgnoreFlash = false;
		AimControl::HumanizeVar = true;
		AimControl::HumanizationStrength = 5;

		ESPConfig::ArmorBar = false;
		ESPConfig::ShowArmorNum = false;
		ESPConfig::ShowIsScoped = true;
		ESPConfig::ShowIsBlind = false;
		ESPConfig::FlashCheck = false;
		ESPConfig::AmmoBar = false;
		ESPConfig::OutLine = true;
		ESPConfig::ShowHealthNum = false;
		ESPConfig::FilledColor = ImColor(59, 71, 148, 128);
		ESPConfig::FilledColor2 = ImColor(59, 71, 148, 128);
		ESPConfig::MultiColor = false;
		ESPConfig::BoxFilledVisColor = ImColor(0, 98, 98, 128);
		ESPConfig::FilledVisBox = false;

		MiscCFG::SpecList = false;
		MiscCFG::BombTimerCol = ImColor(131, 137, 150, 255);
		MiscCFG::bmbTimer = true;
		MiscCFG::FastStop = false;
		MiscCFG::FastStopDelay = 100;

		ESPConfig::VisibleColor = ImColor(59, 71, 148, 180);
		ESPConfig::VisibleCheck = false;

		MenuConfig::WindowStyle = 0;
		ESPConfig::ShowPreview = true;
		ESPConfig::ShowHeadBox = true;
		ESPConfig::HeadBoxColor = ImColor(131, 137, 150, 180);

		ESPConfig::ShowDistance = false;
		ESPConfig::ShowBoneESP = true;
		ESPConfig::HotKey = VK_F6;
		ESPConfig::ShowBoxESP = true;
		ESPConfig::ShowHealthBar = true;
		ESPConfig::ShowWeaponESP = true;
		ESPConfig::ShowEyeRay = false;
		ESPConfig::ShowPlayerName = true;
		ESPConfig::BoxRounding = 5.0f;
		ESPConfig::OutOfFOVRadiusFactor = 0.45f;
		ESPConfig::ShowOutOfFOVArrow = false;
		ESPConfig::OutOfFOVArrowColor = ImColor(59, 71, 148, 128);

		LegitBotConfig::AimBot = true;
		LegitBotConfig::AimToggleMode = false;
		LegitBotConfig::AimPosition = 0;
		LegitBotConfig::AimPositionIndex = BONEINDEX::head;
		LegitBotConfig::HitboxUpdated = false;

		ESPConfig::BoxType = 0;
		ESPConfig::BoneColor = ImColor(131, 137, 150, 180);
		ESPConfig::BoxColor = ImColor(59, 71, 148, 180);
		ESPConfig::EyeRayColor = ImVec4(0, 98, 98, 255);

		MenuConfig::ShowMenu = true;
		MenuConfig::WorkInSpec = true;

		RadarCFG::ShowRadar = false;
		RadarCFG::RadarRange = 125;
		RadarCFG::ShowRadarCrossLine = false;
		RadarCFG::RadarCrossLineColor = ImColor(131, 137, 150, 180);
		RadarCFG::RadarType = 2;
		RadarCFG::RadarPointSizeProportion = 1.f;
		RadarCFG::RadarBgAlpha = 0.1f;
		RadarCFG::Proportion = 2700.f;

		LegitBotConfig::TriggerBot = true;
		LegitBotConfig::TriggerAlways = false;
		TriggerBot::HotKey = 6;
		TriggerBot::TTDtimeout = false;
		Text::Trigger::HotKey = KeyMgr::GetKeyName(TriggerBot::HotKey);

		MenuConfig::TeamCheck = true;
		MenuConfig::BypassOBS = false;
		LegitBotConfig::VisibleCheck = true;

		MiscCFG::ShowHeadShootLine = false;
		MiscCFG::HeadShootLineColor = ImColor(131, 137, 150, 200);

		MiscCFG::AutoAccept = false;

		ESPConfig::EnemySound = false;
		ESPConfig::EnemySoundColor = ImColor(255, 255, 255, 255);

        MiscCFG::AutoKnife = false;
        MiscCFG::AutoKnifeDistance = 70.0f;
        MiscCFG::AutoZeus = false;
        MiscCFG::AutoZeusDistance = 130.0f;

        MiscCFG::AntiAFKKick = false;

		AimControl::HotKey = 1;
		Text::Aimbot::HotKey = KeyMgr::GetKeyName(AimControl::HotKey);
		AimControl::AimFov = 10;
		AimControl::AimFovMin = 0.4f;
		AimControl::Smooth = 5.0f;

		ESPConfig::ShowLineToEnemy = false;
		LegitBotConfig::FovLineSize = 60.0f;
		TriggerBot::TriggerDelay = 10;
		TriggerBot::ShotDuration = 400;

		RCS::RCSBullet = 1;
		LegitBotConfig::RCS = true;

		RCS::RCSScale = ImVec2(1.4f, 1.4f);
		AimControl::onlyAuto = false;
		AimControl::ScopeOnly = true;
		AimControl::AimBullet = 1;

		LegitBotConfig::FovLineColor = ImVec4(0, 98, 98, 220);
		ESPConfig::LineToEnemyColor = ImVec4(59, 71, 148, 180);

		//CrosshairsCFG::ShowCrossHair = false;
		//CrosshairsCFG::CrossHairColor = ImColor(0, 255, 0, 255);
		//CrosshairsCFG::CrossHairSize = 75;
		//CrosshairsCFG::drawDot = true;
		//CrosshairsCFG::tStyle = false;
		//CrosshairsCFG::HorizontalLength = 6;
		//CrosshairsCFG::VerticalLength = 6;
		//CrosshairsCFG::drawOutLine = true;
		//CrosshairsCFG::Gap = 8;
		//CrosshairsCFG::drawCrossline = true;
		//CrosshairsCFG::drawCircle = false;
		//CrosshairsCFG::TargetingCrosshairs = false;
		//CrosshairsCFG::TargetedColor = ImColor(255, 0, 0, 255);
		//CrosshairsCFG::CircleRadius = 3.f;
		//CrosshairsCFG::DynamicGap = false;
		//CrosshairsCFG::DotSize = 1.0f;
		//CrosshairsCFG::Thickness = 1;
		//CrosshairsCFG::TeamCheck = true;

		MiscCFG::WaterMark = true;
		MiscCFG::BunnyHop = false;
		MiscCFG::HitSound = 0;
		MiscCFG::HitMarker = false;
		MiscCFG::SniperCrosshair = true;
		MiscCFG::SniperCrosshairColor = ImColor(32, 178, 170, 255);

		ESPConfig::ESPenabled = true;
		ESPConfig::DrawFov = false;

		LegitBotConfig::FovCircleColor = ImColor(131, 137, 150, 180);

		MenuConfig::MarkWinPos = ImVec2(ImGui::GetIO().DisplaySize.x - 300.0f, 100.f);
		MenuConfig::RadarWinPos = ImVec2(25.f, 25.f);
		MenuConfig::SpecWinPos = ImVec2(10.0f, ImGui::GetIO().DisplaySize.y / 2 - 200);
		MenuConfig::BombWinPos = ImVec2((ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 80.0f);

		MenuConfig::MarkWinChengePos = true;
		MenuConfig::BombWinChengePos = true;
		MenuConfig::RadarWinChengePos = true;
		MenuConfig::SpecWinChengePos = true;

		MenuConfig::HotKey = VK_END;

	}
}

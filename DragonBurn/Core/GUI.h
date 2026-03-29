#pragma once
#include "..\Core\Config.h"
#include "..\Core\Render.h"
#include "..\Features\Aimbot.h"
#include "..\Features\Radar.h"
#include "..\Features\Misc.h"
#include "..\Features\TriggerBot.h"
#include "..\Config\ConfigMenu.h"
#include "..\Config\ConfigSaver.h"

#include "..\Resources\Language.hpp"
#include "..\Resources\Images.hpp"
#include "../Helpers/KeyManager.h"

#include "../Features/ESP.h"

ID3D11ShaderResourceView* Logo = NULL;
ID3D11ShaderResourceView* MenuButton1 = NULL;
ID3D11ShaderResourceView* MenuButton2 = NULL;
ID3D11ShaderResourceView* MenuButton3 = NULL;
ID3D11ShaderResourceView* MenuButton4 = NULL;
ID3D11ShaderResourceView* MenuButton1Pressed = NULL;
ID3D11ShaderResourceView* MenuButton2Pressed = NULL;
ID3D11ShaderResourceView* MenuButton3Pressed = NULL;
ID3D11ShaderResourceView* MenuButton4Pressed = NULL;
ID3D11ShaderResourceView* HitboxImage = NULL;

bool Button1Pressed = true;
bool Button2Pressed = false;
bool Button3Pressed = false;
bool Button4Pressed = false;

int LogoW = 0, LogoH = 0;
int buttonW = 0;
int buttonH = 0;
int hitboxW = 0, hitboxH = 0;

// checkbox for hitbox
bool checkbox1 = true;
bool checkbox2 = false;
bool checkbox3 = false;
bool checkbox4 = false;
bool checkbox5 = false;

namespace GUI
{
	void LoadDefaultConfig()
	{
		if (!MenuConfig::defaultConfig)
			return;

		MyConfigSaver::LoadConfig("default.cfg");

		MenuConfig::defaultConfig = false;
	}

	inline void InitHitboxList()
	{
		if (LegitBotConfig::HitboxUpdated)
			return;
		auto HitboxList = AimControl::HitboxList;

		auto it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::head);
		if (it != HitboxList.end())
			checkbox1 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::neck_0);
		if (it != HitboxList.end())
			checkbox2 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::spine_1);
		if (it != HitboxList.end())
			checkbox3 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::spine_2);
		if (it != HitboxList.end())
			checkbox4 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::pelvis);
		if (it != HitboxList.end())
			checkbox5 = true;

		LegitBotConfig::HitboxUpdated = true;
	}
	void addHitbox(int BoneIndex)
	{
		AimControl::HitboxList.push_back(BoneIndex);
	}
	void removeHitbox(int BoneIndex)
	{
		for (auto it = AimControl::HitboxList.begin(); it != AimControl::HitboxList.end(); ++it) {
			if (*it == BoneIndex) {
				AimControl::HitboxList.erase(it);
				break;
			}
		}
	}

	void LoadImages()
	{
		if (Logo == NULL)
		{
			// Updater::CheckForUpdates();
			Gui.LoadTextureFromMemory(Images::Logo, sizeof Images::Logo, &Logo, &LogoW, &LogoH);
			Gui.LoadTextureFromMemory(Images::AimbotButton, sizeof Images::AimbotButton, &MenuButton1, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::VisualButton, sizeof Images::VisualButton, &MenuButton2, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::MiscButton, sizeof Images::MiscButton, &MenuButton3, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::ConfigButton, sizeof Images::ConfigButton, &MenuButton4, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::PreviewImg, sizeof Images::PreviewImg, &HitboxImage, &hitboxW, &hitboxH);
			Gui.LoadTextureFromMemory(Images::AimbotButtonPressed, sizeof Images::AimbotButtonPressed, &MenuButton1Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::VisualButtonPressed, sizeof Images::VisualButtonPressed, &MenuButton2Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::MiscButtonPressed, sizeof Images::MiscButtonPressed, &MenuButton3Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::ConfigButtonPressed, sizeof Images::ConfigButtonPressed, &MenuButton4Pressed, &buttonW, &buttonH);

			MenuConfig::MarkWinPos = ImVec2(ImGui::GetIO().DisplaySize.x - 300.0f, 100.f);
			MenuConfig::RadarWinPos = ImVec2(25.f, 25.f);
			MenuConfig::SpecWinPos = ImVec2(10.0f, ImGui::GetIO().DisplaySize.y / 2 - 200);
			MenuConfig::BombWinPos = ImVec2((ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 80.0f);
		}
	}

	// Components Settings
	// ########################################
	void AlignRight(float ContentWidth)
	{
		float ColumnContentWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x;
		float checkboxPosX = ImGui::GetColumnOffset() + ColumnContentWidth - ContentWidth;
		ImGui::SetCursorPosX(checkboxPosX);
	}
	void PutSwitch(const char* string, float CursorX, float ContentWidth, bool* v, bool ColorEditor = false, const char* lable = NULL, float col[4] = NULL, const char* Tip = NULL)
	{
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float CurrentCursorY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(string);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine();
		ImGui::SetCursorPosY(CurrentCursorY - 2);
		if (ColorEditor) {
			AlignRight(ContentWidth + ImGui::GetFrameHeight() +7);
			ImGui::ColorEdit4(lable, col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			ImGui::SameLine();
		}
		else {
			AlignRight(ContentWidth);
		}
		
		Gui.SwitchButton(string, v);
		ImGui::PopID();
	}
	void PutColorEditor(const char* text, const char* lable, float CursorX, float ContentWidth, float col[4], const char* Tip = NULL)
	{
		ImGui::PushID(text);
		float CurrentCursorX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(text);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine();
		AlignRight(ContentWidth + ImGui::GetFrameHeight() + 8);
		ImGui::ColorEdit4(lable, col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		ImGui::PopID();
	}
	void PutSliderFloat(const char* string, float CursorX, float* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
	{
		// if there is no fucking ID, all the sliders would be fucking forced to sync when you click on one of them ;3
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float SliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - CursorX - 15;
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(string);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine();
		ImGui::TextDisabled(format, *v);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(SliderWidth);
		Gui.SliderScalarEx2("", ImGuiDataType_Float, v, p_min, p_max, "", ImGuiSliderFlags_None);
		ImGui::PopID();
	}
	void PutSliderInt(const char* string, float CursorX, int* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
	{
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float SliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - CursorX-15;
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(string);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine();
		ImGui::TextDisabled(format, *v);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(SliderWidth);
		Gui.SliderScalarEx2("", ImGuiDataType_Float, v, p_min, p_max, "", ImGuiSliderFlags_None);
		ImGui::PopID();
	}
	// ########################################

	void DrawGui()
	{
		LoadImages();
		ImTextureID ImageID;
		ImVec2 LogoSize, LogoPos;

		ImageID = (void*)Logo;
		LogoSize = ImVec2(LogoW, LogoH);
		LogoPos = MenuConfig::WCS.LogoPos;

		ImColor BorderColor = ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border));

		char TempText[256];
		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowPos({ (ImGui::GetIO().DisplaySize.x - MenuConfig::WCS.MainWinSize.x) / 2.0f, (ImGui::GetIO().DisplaySize.y - MenuConfig::WCS.MainWinSize.y) / 2.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(MenuConfig::WCS.MainWinSize);
		ImGui::Begin("DragonBurn", nullptr, Flags);
		{
			ImGui::SetCursorPos(LogoPos);
			ImGui::Image(ImageID, LogoSize);
			if (ImGui::IsItemClicked()) {
				Gui.OpenWebpage("https://github.com/ByteCorum/DragonBurn");
			}
			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(MenuConfig::WCS.LogoPos.x + ImGui::GetWindowPos().x, MenuConfig::WCS.LogoPos.y + ImGui::GetWindowPos().y),
				ImVec2(MenuConfig::WCS.LogoPos.x + LogoW + ImGui::GetWindowPos().x, MenuConfig::WCS.LogoPos.y + LogoH + ImGui::GetWindowPos().y),
				BorderColor, 0.f, ImDrawFlags_RoundCornersNone | ImDrawCornerFlags_Top | ImDrawCornerFlags_Bot, 1.f, true);

			ImGui::SetCursorPos(MenuConfig::WCS.Button1Pos);
			if (!Button1Pressed)
				ImGui::Image((void*)MenuButton1, ImVec2(buttonW, buttonH));
			if (Button1Pressed)
				ImGui::Image((void*)MenuButton1Pressed, ImVec2(buttonW, buttonH));
			if (ImGui::IsItemClicked()) 
			{
				MenuConfig::WCS.MenuPage = 0;
				Button1Pressed = true;
				Button2Pressed = false;
				Button3Pressed = false;
				Button4Pressed = false;
			}
			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(MenuConfig::WCS.Button1Pos.x + ImGui::GetWindowPos().x, MenuConfig::WCS.Button1Pos.y + ImGui::GetWindowPos().y),
				ImVec2(MenuConfig::WCS.Button1Pos.x + buttonW + ImGui::GetWindowPos().x, MenuConfig::WCS.Button1Pos.y + buttonH + ImGui::GetWindowPos().y),
				BorderColor, 0.f, ImDrawFlags_RoundCornersNone | ImDrawCornerFlags_Top | ImDrawCornerFlags_Bot, 1.f, true);

			ImGui::SetCursorPos(MenuConfig::WCS.Button2Pos);
			if (!Button2Pressed)
				ImGui::Image((void*)MenuButton2, ImVec2(buttonW, buttonH));
			if (Button2Pressed)
				ImGui::Image((void*)MenuButton2Pressed, ImVec2(buttonW, buttonH));
			if (ImGui::IsItemClicked())
			{
				MenuConfig::WCS.MenuPage = 1;
				Button1Pressed = false;
				Button2Pressed = true;
				Button3Pressed = false;
				Button4Pressed = false;
			}
			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(MenuConfig::WCS.Button2Pos.x + ImGui::GetWindowPos().x, MenuConfig::WCS.Button2Pos.y + ImGui::GetWindowPos().y),
				ImVec2(MenuConfig::WCS.Button2Pos.x + buttonW + ImGui::GetWindowPos().x, MenuConfig::WCS.Button2Pos.y + buttonH + ImGui::GetWindowPos().y),
				BorderColor, 0.f, ImDrawFlags_RoundCornersNone | ImDrawCornerFlags_Top | ImDrawCornerFlags_Bot, 1.f, true);

			ImGui::SetCursorPos(MenuConfig::WCS.Button3Pos);
			if (!Button3Pressed)
				ImGui::Image((void*)MenuButton3, ImVec2(buttonW, buttonH));
			if (Button3Pressed)
				ImGui::Image((void*)MenuButton3Pressed, ImVec2(buttonW, buttonH));
			if (ImGui::IsItemClicked())
			{
				MenuConfig::WCS.MenuPage = 2;
				Button1Pressed = false;
				Button2Pressed = false;
				Button3Pressed = true;
				Button4Pressed = false;
			}
			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(MenuConfig::WCS.Button3Pos.x + ImGui::GetWindowPos().x, MenuConfig::WCS.Button3Pos.y + ImGui::GetWindowPos().y),
				ImVec2(MenuConfig::WCS.Button3Pos.x + buttonW + ImGui::GetWindowPos().x, MenuConfig::WCS.Button3Pos.y + buttonH + ImGui::GetWindowPos().y),
				BorderColor, 0.f, ImDrawFlags_RoundCornersNone | ImDrawCornerFlags_Top | ImDrawCornerFlags_Bot, 1.f, true);

			ImGui::SetCursorPos(MenuConfig::WCS.Button4Pos);
			if (!Button4Pressed)
				ImGui::Image((void*)MenuButton4, ImVec2(buttonW, buttonH));
			if (Button4Pressed)
				ImGui::Image((void*)MenuButton4Pressed, ImVec2(buttonW, buttonH));
			if (ImGui::IsItemClicked())
			{
				MenuConfig::WCS.MenuPage = 3;
				Button1Pressed = false;
				Button2Pressed = false;
				Button3Pressed = false;
				Button4Pressed = true;
			}
			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(MenuConfig::WCS.Button4Pos.x + ImGui::GetWindowPos().x, MenuConfig::WCS.Button4Pos.y + ImGui::GetWindowPos().y),
				ImVec2(MenuConfig::WCS.Button4Pos.x + buttonW + ImGui::GetWindowPos().x, MenuConfig::WCS.Button4Pos.y + buttonH + ImGui::GetWindowPos().y),
				BorderColor, 0.f, ImDrawFlags_RoundCornersNone | ImDrawCornerFlags_Top | ImDrawCornerFlags_Bot, 1.f, true);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

			ImGui::SetCursorPos(MenuConfig::WCS.ChildPos);
			
			ImGui::BeginChild("Page", MenuConfig::WCS.ChildSize, false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Text("   DragonBurn");
				ImGui::Separator();
				if (MenuConfig::WCS.MenuPage == 1)
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetCursorPos(ImVec2(15.f, 24.f));
					ImGui::GradientText("ESP");
					static const float MinRounding = 0.f, MaxRouding = 5.f;
					static const float MinFovFactor = 0.f, MaxFovFactor = 1.f;
					PutSwitch(Text::ESP::Enable.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ESPenabled);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
					ImGui::TextDisabled(Text::ESP::HotKeyList.c_str());
					ImGui::SameLine();
					AlignRight(70.f);
					if (ImGui::Button(Text::ESP::HotKey.c_str(), { 70.f, 25.f }))
					{
						std::thread([&]() {
							KeyMgr::GetPressedKey(ESPConfig::HotKey, &Text::ESP::HotKey);
							}).detach();
					}

					if (ESPConfig::ESPenabled)
					{

						PutSwitch(Text::ESP::Box.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoxESP, true, "###BoxCol", reinterpret_cast<float*>(&ESPConfig::BoxColor));
						if (ESPConfig::ShowBoxESP)
						{
							PutSwitch(Text::ESP::Outline.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::OutLine);
							ImGui::TextDisabled(Text::ESP::BoxType.c_str());
							ImGui::SameLine();
							AlignRight(160.f);
							ImGui::SetNextItemWidth(160.f);
							ImGui::Combo("###BoxType", &ESPConfig::BoxType, "Normal\0Corner\0");
							PutSliderFloat(Text::ESP::BoxRounding.c_str(), 10.f, &ESPConfig::BoxRounding, &MinRounding, &MaxRouding, "%.1f");
						}
						PutSwitch(Text::ESP::FilledBox.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FilledBox, true, "###FilledBoxCol", reinterpret_cast<float*>(&ESPConfig::FilledColor));
						if (ESPConfig::FilledBox)
							PutSwitch(Text::ESP::MultiColor.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::MultiColor, true, "###MultiCol", reinterpret_cast<float*>(&ESPConfig::FilledColor2));
						PutSwitch(Text::ESP::HeadBox.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHeadBox, true, "###HeadBoxCol", reinterpret_cast<float*>(&ESPConfig::HeadBoxColor));
						PutSwitch(Text::ESP::Skeleton.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoneESP, true, "###BoneCol", reinterpret_cast<float*>(&ESPConfig::BoneColor));
						PutSwitch(Text::ESP::SnapLine.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowLineToEnemy, true, "###LineCol", reinterpret_cast<float*>(&ESPConfig::LineToEnemyColor));
						if (ESPConfig::ShowLineToEnemy) 
						{
							ImGui::TextDisabled(Text::ESP::LinePosList.c_str());
							ImGui::SameLine();
							AlignRight(160.f);
							ImGui::SetNextItemWidth(160.f);
							ImGui::Combo("###LinePos", &ESPConfig::LinePos, "Top\0Center\0Bottom\0");
						}
						PutSwitch(Text::ESP::EyeRay.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowEyeRay, true, "###LineCol", reinterpret_cast<float*>(&ESPConfig::EyeRayColor));
						PutSwitch(Text::ESP::OutOfFOVArrow.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowOutOfFOVArrow, true, "###OutFOVCol", reinterpret_cast<float*>(&ESPConfig::OutOfFOVArrowColor));
						if(ESPConfig::ShowOutOfFOVArrow)
							PutSliderFloat(Text::ESP::OutOfFOVRadius.c_str(), .5f, &ESPConfig::OutOfFOVRadiusFactor, &MinFovFactor, &MaxFovFactor, "%.1f");

						PutSwitch(Text::ESP::SoundEsp.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::EnemySound, true, "###EnemySoundCol", reinterpret_cast<float*>(&ESPConfig::EnemySoundColor));
						PutSwitch(Text::ESP::HealthBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthBar);
						if (ESPConfig::ShowHealthBar)
							PutSwitch(Text::ESP::HealthNum.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthNum);
						PutSwitch(Text::ESP::ShowArmorBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ArmorBar);
						if (ESPConfig::ArmorBar)
							PutSwitch(Text::ESP::ArmorNum.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowArmorNum);
						PutSwitch(Text::ESP::Weapon.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponESP);
						PutSwitch(Text::ESP::Ammo.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::AmmoBar);
						PutSwitch(Text::ESP::Distance.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowDistance);
						PutSwitch(Text::ESP::PlayerName.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowPlayerName);
						PutSwitch(Text::ESP::ScopedESP.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowIsScoped);
						PutSwitch(Text::ESP::FlashedESP.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowIsBlind);
						PutSwitch(Text::ESP::FlashCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FlashCheck);
						PutSwitch(Text::ESP::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::VisibleCheck, true, "###VisibleCol", reinterpret_cast<float*>(&ESPConfig::VisibleColor));
					}
					ImGui::NewLine();
					
					ImGui::NextColumn();
					ImGui::SetCursorPosY(24.f);
					ImGui::GradientText("ESP Preview");
					// ESP::RenderPreview({ ImGui::GetColumnWidth(), ImGui::GetCursorPosY() }, { ImGui::GetCursorPosX() - ImGui::GetColumnWidth() * 0.65f, ImGui::GetCursorPosY() - ImGui::GetFrameHeight() });
					ESP::RenderPreview({ ImGui::GetColumnWidth(), ImGui::GetCursorPosY() });
					ImGui::Dummy({ 0.f, ImGui::GetFrameHeight() * 9 });

					ImGui::GradientText("External Radar");
					static const float RadarPointSizeProportionMin = 0.2f, RadarPointSizeProportionMax = 2.f;
					static const float ProportionMin = 500.f, ProportionMax = 15000.f;
					static const float RadarRangeMin = 100.f, RadarRangeMax = 300.f;
					static const float AlphaMin = 0.f, AlphaMax = 1.f;
					PutSwitch(Text::Radar::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::ShowRadar);
					if (RadarCFG::ShowRadar)
					{
						PutSwitch(Text::Radar::CustomCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::customRadar);
						
						if (RadarCFG::customRadar)
						{
							PutSwitch(Text::Radar::CrossLine.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::ShowRadarCrossLine);
							PutSliderFloat(Text::Radar::SizeSlider.c_str(), 5.f, &RadarCFG::RadarPointSizeProportion, &RadarPointSizeProportionMin, &RadarPointSizeProportionMax, "%.1f");
							PutSliderFloat(Text::Radar::ProportionSlider.c_str(), 5.f, &RadarCFG::Proportion, &ProportionMin, &ProportionMax, "%.1f");
							PutSliderFloat(Text::Radar::RangeSlider.c_str(), 5.f, &RadarCFG::RadarRange, &RadarRangeMin, &RadarRangeMax, "%.1f");
							PutSliderFloat(Text::Radar::AlphaSlider.c_str(), 5.f, &RadarCFG::RadarBgAlpha, &AlphaMin, &AlphaMax, "%.1f");
						}
					}
					
					//ImGui::NewLine();
					//ImGui::GradientText("Crosshairs");
					//float DotMin = 1.f, DotMax = 50.f;
					//int LengthMin = 1, LengthMax = 100;
					//int GapMin = 1, GapMax = 50;
					//int ThickMin = 1, ThickMax = 20;
					//float CircleRmin = 1.f, CircleRmax = 50.f;
					//PutSwitch(Lang::CrosshairsText.Toggle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::ShowCrossHair, true, "###CrosshairsCol", reinterpret_cast<float*>(&CrosshairsCFG::CrossHairColor));
					//if (CrosshairsCFG::ShowCrossHair)
					//{
					//	PutSwitch(Lang::CrosshairsText.Dot, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawDot);
					//	if (CrosshairsCFG::drawDot)
					//		PutSliderFloat(Lang::CrosshairsText.DotSizeSlider, 5.f, &CrosshairsCFG::DotSize, &DotMin, &DotMax, "%.f px");
					//	PutSwitch(Lang::CrosshairsText.Outline, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawOutLine);
					//	PutSwitch(Lang::CrosshairsText.Crossline, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawCrossline);
					//	if (CrosshairsCFG::drawCrossline)
					//	{
					//		PutSliderInt(Lang::CrosshairsText.hLengthSlider, 5.f, &CrosshairsCFG::HorizontalLength, &LengthMin, &LengthMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.vLengthSilder, 5.f, &CrosshairsCFG::VerticalLength, &LengthMin, &LengthMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.GapSlider, 5.f, &CrosshairsCFG::Gap, &GapMin, &GapMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.ThicknessSlider, 5.f, &CrosshairsCFG::Thickness, &ThickMin, &ThickMax, "%d px");
					//	}
					//	PutSwitch(Lang::CrosshairsText.tStyle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::tStyle);
					//	PutSwitch(Lang::CrosshairsText.Circle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawCircle);
					//	if (CrosshairsCFG::drawCircle)
					//		PutSliderFloat(Lang::CrosshairsText.RadiusSlider, 5.f, &CrosshairsCFG::CircleRadius, &CircleRmin, &CircleRmax, "%.f px");
					//	PutSwitch(Lang::CrosshairsText.TargetCheck, 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::TargetingCrosshairs, true, "###CircleCol", reinterpret_cast<float*>(&CrosshairsCFG::TargetedColor));
					//	PutSwitch(Lang::CrosshairsText.TeamCheck, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::TeamCheck);
					//}
					
					//ImGui::Columns(1);
				}
				
				if (MenuConfig::WCS.MenuPage == 0)
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetCursorPos(ImVec2(15.f, 24.f));
					ImGui::GradientText("Aimbot");

					static const float FovMin = 0.f, FovMax = 30.f, MinFovMax = 1.f;
					static const int BulletMin = 0, BulletMax = 5;
					static const float SmoothMin = 0.f, SmoothMax = 10.f;
					static const int MinHumanize = 0;
					static const int MaxHumanize = 15;
					PutSwitch(Text::Aimbot::Enable.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::AimBot);
					if (LegitBotConfig::AimBot)
					{

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
						ImGui::TextDisabled(Text::Aimbot::HotKeyList.c_str());
						ImGui::SameLine();
						AlignRight(70.f);
						if (ImGui::Button(Text::Aimbot::HotKey.c_str(), { 70.f, 25.f }))
						{
							std::thread([&]() {
								KeyMgr::GetPressedKey(AimControl::HotKey, &Text::Aimbot::HotKey);
								}).detach();
						}
						PutSliderInt(Text::Aimbot::BulletSlider.c_str(), 10.f, &AimControl::AimBullet, &BulletMin, &BulletMax, "%d", Text::Aimbot::StartBulletTip.c_str());
						PutSwitch(Text::Aimbot::Toggle.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::AimToggleMode, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
						PutSwitch(Text::Aimbot::DrawFov.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::DrawFov, true, "###FOVcol", reinterpret_cast<float*>(&LegitBotConfig::FovCircleColor));
						PutSwitch(Text::Aimbot::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::VisibleCheck, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
						PutSwitch(Text::Aimbot::OnlyAuto.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &AimControl::onlyAuto, false, NULL, NULL, Text::Aimbot::OnlyAutoTip.c_str());
						PutSwitch(Text::Aimbot::IgnoreFlash.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &AimControl::IgnoreFlash, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
						PutSwitch(Text::Aimbot::ScopeOnly.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &AimControl::ScopeOnly);

						PutSwitch(Text::Aimbot::HumanizeVar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &AimControl::HumanizeVar, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
						if(AimControl::HumanizeVar)
							PutSliderInt(Text::Aimbot::HumanizationStrength.c_str(), 10.f, &AimControl::HumanizationStrength, &MinHumanize, &MaxHumanize, "%d");

						PutSliderFloat(Text::Aimbot::FovSlider.c_str(), 10.f, &AimControl::AimFov, &AimControl::AimFovMin, &FovMax, "%.1f");
						PutSliderFloat(Text::Aimbot::FovMinSlider.c_str(), 10.f, &AimControl::AimFovMin, &FovMin, &MinFovMax, "%.2f");
						PutSliderFloat(Text::Aimbot::SmoothSlider.c_str(), 10.f, &AimControl::Smooth, &SmoothMin, &SmoothMax, "%.1f", Text::Aimbot::OnlyAutoTip.c_str());
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
						ImGui::TextDisabled(Text::Aimbot::BoneList.c_str());

						ImVec2 StartPos = ImGui::GetCursorScreenPos();
						ImGui::Image((void*)HitboxImage, ImVec2(hitboxW, hitboxH));

						ImGui::GetWindowDrawList()->AddLine(ImVec2(StartPos.x + 130, StartPos.y + 20), ImVec2(StartPos.x + 205, StartPos.y + 20), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.8f); // Head
						ImGui::SetCursorScreenPos(ImVec2(StartPos.x + 203, StartPos.y + 10)); 
						if (ImGui::Checkbox("###Head", &checkbox1))
						{
							if (checkbox1) {
								addHitbox(BONEINDEX::head);
							}
							else {
								removeHitbox(BONEINDEX::head);
							}
						}
						ImGui::GetWindowDrawList()->AddLine(ImVec2(StartPos.x + 129, StartPos.y + 56), ImVec2(StartPos.x + 59, StartPos.y + 56), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.8f); // Neck
						ImGui::SetCursorScreenPos(ImVec2(StartPos.x + 39, StartPos.y + 45));
						if (ImGui::Checkbox("###Neck", &checkbox2))
						{
							if (checkbox2) {
								addHitbox(BONEINDEX::neck_0);
							}
							else {
								removeHitbox(BONEINDEX::neck_0);
							}
						}
						ImGui::GetWindowDrawList()->AddLine(ImVec2(StartPos.x + 120, StartPos.y + 80), ImVec2(StartPos.x + 195, StartPos.y + 80), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.8f); // Chest
						ImGui::SetCursorScreenPos(ImVec2(StartPos.x + 193, StartPos.y + 70));
						if (ImGui::Checkbox("###Chest", &checkbox3))
						{
							if (checkbox3) {
								addHitbox(BONEINDEX::spine_1);
							}
							else {
								removeHitbox(BONEINDEX::spine_1);
							}
						}
						ImGui::GetWindowDrawList()->AddLine(ImVec2(StartPos.x + 119, StartPos.y + 120), ImVec2(StartPos.x + 44, StartPos.y + 120), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.8f);
						ImGui::SetCursorScreenPos(ImVec2(StartPos.x + 24, StartPos.y + 109));
						if (ImGui::Checkbox("###Stomache", &checkbox4))
						{
							if (checkbox4) {
								addHitbox(BONEINDEX::spine_2);
							}
							else {
								removeHitbox(BONEINDEX::spine_2);
							}
						}
						ImGui::GetWindowDrawList()->AddLine(ImVec2(StartPos.x + 119, StartPos.y + 150), ImVec2(StartPos.x + 195, StartPos.y + 150), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.8f);
						ImGui::SetCursorScreenPos(ImVec2(StartPos.x + 193, StartPos.y + 139));
						if (ImGui::Checkbox("###Pelvis", &checkbox5))
						{
							if (checkbox4) {
								addHitbox(BONEINDEX::pelvis);
							}
							else {
								removeHitbox(BONEINDEX::pelvis);
							}
						}
						//ImGui::SetCursorScreenPos(ImVec2(StartPos.x, StartPos.y + hitboxH));
					}
					ImGui::NextColumn();
					ImGui::SetCursorPosY(24.f);
					ImGui::GradientText("RCS");
					static const float recoilMin = 0.f, recoilMax = 2.f;
					static const int RCSBulletMin = 0, RCSBulletMax = 5;
					PutSwitch(Text::RCS::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::RCS);
					if (LegitBotConfig::RCS)
					{
						PutSliderInt(Text::RCS::BulletSlider.c_str(), 5.f, &RCS::RCSBullet, &RCSBulletMin, &RCSBulletMax, "%d");
						PutSliderFloat(Text::RCS::Yaw.c_str(), 5.f, &RCS::RCSScale.x, &recoilMin, &recoilMax, "%.2f");
						PutSliderFloat(Text::RCS::Pitch.c_str(), 5.f, &RCS::RCSScale.y, &recoilMin, &recoilMax, "%.2f");
						float scalex = (2.22 - RCS::RCSScale.x) *.5f;
						float scaley = (2.12 - RCS::RCSScale.y) *.5f;//Simulate reasonable error values
						ImVec2 BulletPos = ImGui::GetCursorScreenPos();

						// Example Preview
						ImVec2 BulletPos0, BulletPos1, BulletPos2, BulletPos3, BulletPos4, BulletPos5, BulletPos6, BulletPos7, BulletPos8, BulletPos9, BulletPos10, BulletPos11, BulletPos12, BulletPos13, BulletPos14, BulletPos15;
						BulletPos.y += 123 * scaley;
						BulletPos0.x = BulletPos.x + 125; BulletPos0.y = BulletPos.y + 5;
						BulletPos1.x = BulletPos0.x - 3 * scalex; BulletPos1.y = BulletPos0.y - 5 * scaley;
						BulletPos2.x = BulletPos1.x + 2 * scalex; BulletPos2.y = BulletPos1.y - 10 * scaley;
						BulletPos3.x = BulletPos2.x + 4 * scalex; BulletPos3.y = BulletPos2.y - 11 * scaley;
						BulletPos4.x = BulletPos3.x - 3 * scalex; BulletPos4.y = BulletPos3.y - 31 * scaley;
						BulletPos5.x = BulletPos4.x - 1 * scalex; BulletPos5.y = BulletPos4.y - 20 * scaley;
						BulletPos6.x = BulletPos5.x - 2 * scalex; BulletPos6.y = BulletPos5.y - 17 * scaley;
						BulletPos7.x = BulletPos6.x - 15 * scalex; BulletPos7.y = BulletPos6.y - 9 * scaley;
						BulletPos8.x = BulletPos7.x + 7 * scalex; BulletPos8.y = BulletPos7.y - 8 * scaley;
						BulletPos9.x = BulletPos8.x + 33 * scalex; BulletPos9.y = BulletPos8.y + 2 * scaley;
						BulletPos10.x = BulletPos9.x + 1 * scalex; BulletPos10.y = BulletPos9.y - 16 * scaley;
						BulletPos11.x = BulletPos10.x - 9 * scalex; BulletPos11.y = BulletPos10.y + 20 * scaley;
						BulletPos12.x = BulletPos11.x - 3 * scalex; BulletPos12.y = BulletPos11.y - 9 * scaley;
						BulletPos13.x = BulletPos12.x + 15 * scalex; BulletPos13.y = BulletPos12.y - 5 * scaley;
						BulletPos14.x = BulletPos13.x + 10 * scalex; BulletPos14.y = BulletPos13.y - 4 * scaley;
						
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos0, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos1, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos2, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos3, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos4, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos5, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos6, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos7, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos8, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos9, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos10, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos11, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos12, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos13, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
						ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos14, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));

						ImGui::SetCursorScreenPos(ImVec2(BulletPos.x, BulletPos.y + 10));
					}

					ImGui::NewLine();
					ImGui::GradientText("Triggerbot");
					static const int DelayMin = 0, DelayMax = 300;
					static const int DurationMin = 0, DurationMax = 1000;

					PutSwitch(Text::Trigger::Enable.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::TriggerBot);
					if (LegitBotConfig::TriggerBot)
					{
						if (!LegitBotConfig::TriggerAlways)
						{
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.f);
							ImGui::TextDisabled(Text::Trigger::HotKeyList.c_str());
							ImGui::SameLine();
							AlignRight(70.f);
							if (ImGui::Button(Text::Trigger::HotKey.c_str(), {70.f, 25.f}))
							{
								std::thread([&]() {
									KeyMgr::GetPressedKey(TriggerBot::HotKey, &Text::Trigger::HotKey);
									}).detach();
							}
						}
						PutSwitch(Text::Trigger::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::TriggerAlways, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
						PutSwitch(Text::Trigger::ScopeOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::ScopeOnly);
						PutSwitch(Text::Trigger::IgnoreFlash.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::IgnoreFlash, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
						PutSwitch(Text::Trigger::StopOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::StopedOnly);
						PutSwitch(Text::Trigger::TTDtimeout.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::TTDtimeout, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
						PutSliderInt(Text::Trigger::DelaySlider.c_str(), 5.f, &TriggerBot::TriggerDelay, &DelayMin, &DelayMax, "%d ms", Text::Trigger::DelayTip.c_str());
						PutSliderInt(Text::Trigger::FakeShotSlider.c_str(), 5.f, &TriggerBot::ShotDuration, &DurationMin, &DurationMax, "%d ms");
					}

					ImGui::Columns(1);
				}

				if (MenuConfig::WCS.MenuPage == 2)
				{
					static const int FovMin = 60, FovMax = 140;
					static const int NightMin = 0, NightMax = 150;
					static const float FlashMin = 0.f, FlashMax = 255.f;
					ImGui::Columns(2, nullptr, false);
					ImGui::SetCursorPos(ImVec2(15.f, 24.f));
					ImGui::GradientText("Misc");
					PutSwitch(Text::Misc::bmbTimer.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::bmbTimer, true, "###bmbTimerCol", reinterpret_cast<float*>(&MiscCFG::BombTimerCol));
					PutSwitch(Text::Misc::SpecList.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SpecList);
					PutSwitch(Text::Misc::Watermark.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::WaterMark);
					PutSwitch(Text::Misc::HeadshotLine.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowHeadShootLine);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
					ImGui::TextDisabled(Text::Misc::HitSound.c_str());
					ImGui::SameLine();
					AlignRight(160.f);
					ImGui::SetNextItemWidth(160.f);
					ImGui::Combo("###HitSounds", &MiscCFG::HitSound, "None\0Neverlose\0Skeet\0");
					PutSwitch(Text::Misc::HitMerker.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::HitMarker);
					PutSwitch(Text::Misc::BunnyHop.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::BunnyHop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
					PutSwitch(Text::Misc::FastStop.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::FastStop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
					PutSwitch(Text::Misc::SniperCrosshair.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SniperCrosshair, true, "###sniperCrosshair", reinterpret_cast<float*>(&MiscCFG::SniperCrosshairColor));
					PutSwitch("Auto Accept", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoAccept);
                    PutSwitch("Knife bot", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoKnife);
                    PutSwitch("Zeus bot", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoZeus);
                    PutSwitch("Anti-afk", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AntiAFKKick);

					ImGui::NextColumn();
					ImGui::SetCursorPosY(24.f);
					ImGui::GradientText("Global Settings");
					ImGui::TextDisabled(Text::Misc::MenuKey.c_str());
					ImGui::SameLine();
					AlignRight(70.f);
					if (ImGui::Button(Text::Misc::HotKey.c_str(), { 70.f, 25.f }))
					{
						std::thread([&]() {
							KeyMgr::GetPressedKey(MenuConfig::HotKey, &Text::Misc::HotKey);
							}).detach();
					}
					PutSwitch(Text::Misc::SpecCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::WorkInSpec);
					PutSwitch(Text::Misc::TeamCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::TeamCheck);
					PutSwitch(Text::Misc::AntiRecord.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::BypassOBS);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.f);

					ImGui::NewLine();
					if (ImGui::Button("Source Code", { 125.f, 25.f }))
						Gui.OpenWebpage("https://github.com/ByteCorum/DragonBurn");
					ImGui::SameLine();
					if (ImGui::Button("Contact Author", { 125.f, 25.f }))
						Gui.OpenWebpage("https://discord.gg/5WcvdzFybD");
					if (ImGui::Button("Unhook", { 125.f, 25.f }))
						Init::Client::Exit();
					ImGui::SameLine();
					if (ImGui::Button("Clear Traces", { 125.f, 25.f }))
					{
						Misc::CleanTraces();
						Init::Client::Exit();
					}


					ImGui::Columns(1);
				}

				if (MenuConfig::WCS.MenuPage == 3)
				{
					ConfigMenu::RenderCFGmenu();
				}
				ImGui::NewLine();
			} ImGui::EndChild();
		} ImGui::End();

		LoadDefaultConfig();
	}
}

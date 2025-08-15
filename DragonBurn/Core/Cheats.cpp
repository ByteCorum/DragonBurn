//
//______                            ______                  
//|  _  \                           | ___ \                 
//| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
//| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
//| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
//|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
//                  __/ |                                   
//                 |___/                                    
//https://github.com/ByteCorum/DragonBurn

#include <string>
#include <thread>
#include <future>
#include <iostream>

#include "Cheats.h"
#include "Render.h"
#include "../Core/Config.h"

#include "../Core/Init.h"

#include "../Features/ESP.h"
#include "../Core/GUI.h"
#include "../Features/RCS.H"
#include "../Features/BombTimer.h"
#include "../Features/SpectatorList.h"
#include "../Helpers/Logger.h"

int PreviousTotalHits = 0;

void RenderCrosshair(ImDrawList*, const CEntity&);

void RadarSetting(Base_Radar&);

void Menu();
void Visual(const CEntity&);
void Radar(Base_Radar, const CEntity&);
void Trigger(const CEntity&);
void AIM(const CEntity&, std::vector<Vec3>);
void MiscFuncs(CEntity&);

void Cheats::Run()
{	
	Menu();

	if (!Init::Client::isGameWindowActive() && !MenuConfig::ShowMenu)
		return;

	// Update matrix
	if(!memoryManager.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix,64))
		return;

	// Update EntityList Entry
	gGame.UpdateEntityListEntry();

	DWORD64 LocalControllerAddress = 0;
	DWORD64 LocalPawnAddress = 0;

	if (!memoryManager.ReadMemory(gGame.GetLocalControllerAddress(), LocalControllerAddress))
		return;
	if (!memoryManager.ReadMemory(gGame.GetLocalPawnAddress(), LocalPawnAddress))
		return;

	// LocalEntity
	CEntity LocalEntity, ServerEntity;
	static int LocalPlayerControllerIndex = 1;
	LocalEntity.UpdateClientData();
	if (!LocalEntity.UpdateController(LocalControllerAddress))
		return;
	if (!LocalEntity.UpdatePawn(LocalPawnAddress) && !MenuConfig::WorkInSpec)
		return;

	// HealthBar Map
	static std::map<DWORD64, Render::HealthBar> HealthBarMap;

	// AimBot data
	float DistanceToSight = 0;
	float MaxAimDistance = 100000;
	Vec3  HeadPos{ 0,0,0 };
	Vec2  Angles{ 0,0 };
	std::vector<Vec3> AimPosList;

	// Radar Data
	Base_Radar GameRadar;
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
		RadarSetting(GameRadar);

	for (int i = 0; i < 64; i++)
	{
		CEntity Entity;
		DWORD64 EntityAddress = 0;
		if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (i + 1) * 0x78, EntityAddress))
			continue;
		if (EntityAddress == LocalEntity.Controller.Address)
		{
			LocalPlayerControllerIndex = i;
			continue;
		}
		if (!Entity.UpdateController(EntityAddress))
			continue;

		if (!Entity.UpdatePawn(Entity.Pawn.Address))
			continue;

		//speclist
		SpecList::GetSpectatorList(Entity, LocalEntity);

		if (MenuConfig::TeamCheck && Entity.Controller.TeamID == LocalEntity.Controller.TeamID)
			continue;

		if (!Entity.IsAlive())
			continue;

		// Add entity to radar
		if (RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0)
			GameRadar.AddPoint(LocalEntity.Pawn.Pos, LocalEntity.Pawn.ViewAngle.y, Entity.Pawn.Pos, ImColor(237, 85, 106, 200), RadarCFG::RadarType, Entity.Pawn.ViewAngle.y);

		if (!Entity.IsInScreen())
			continue;

		//update Bone select
		if (AimControl::HitboxList.size() != 0)
		{
			for (int i = 0; i < AimControl::HitboxList.size(); i++)
			{
				Vec3 TempPos;
				DistanceToSight = Entity.GetBone().BonePosList[AimControl::HitboxList[i]].ScreenPos.DistanceTo({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });

				if (DistanceToSight < MaxAimDistance)
				{
					MaxAimDistance = DistanceToSight;

					if (!LegitBotConfig::VisibleCheck ||
						Entity.Pawn.bSpottedByMask & (DWORD64(1) << (LocalPlayerControllerIndex)) ||
						LocalEntity.Pawn.bSpottedByMask & (DWORD64(1) << (i)))
					{
						TempPos = Entity.GetBone().BonePosList[AimControl::HitboxList[i]].Pos;
						if (AimControl::HitboxList[i] == BONEINDEX::head)
							TempPos.z -= 1.f;

						AimPosList.push_back(TempPos);
					}
				}
			}
		}

		if (ESPConfig::ESPenabled)
		{
			ImVec4 Rect = ESP::GetBoxRect(Entity, ESPConfig::BoxType);
			int distance = static_cast<int>(Entity.Pawn.Pos.DistanceTo(LocalEntity.Pawn.Pos) / 100);

			if (MenuConfig::RenderDistance == 0 || (distance <= MenuConfig::RenderDistance && MenuConfig::RenderDistance > 0))
			{
				ESP::RenderPlayerESP(LocalEntity, Entity, Rect, LocalPlayerControllerIndex, i);
				Render::DrawDistance(LocalEntity, Entity, Rect);

				// Draw HealthBar
				if (ESPConfig::ShowHealthBar)
				{
					ImVec2 HealthBarPos = { Rect.x - 6.f,Rect.y };
					ImVec2 HealthBarSize = { 4 ,Rect.w };
					Render::DrawHealthBar(EntityAddress, 100, Entity.Pawn.Health, HealthBarPos, HealthBarSize);
				}

				// Draw Ammo
				// When player is using knife or nade, Ammo = -1.
				if (ESPConfig::AmmoBar && Entity.Pawn.Ammo != -1)
				{
					ImVec2 AmmoBarPos = { Rect.x, Rect.y + Rect.w + 2 };
					ImVec2 AmmoBarSize = { Rect.z,4 };
					Render::DrawAmmoBar(EntityAddress, Entity.Pawn.MaxAmmo, Entity.Pawn.Ammo, AmmoBarPos, AmmoBarSize);
				}

				// Draw Armor
				// It is meaningless to render a empty bar
				if (ESPConfig::ArmorBar && Entity.Pawn.Armor > 0)
				{
					bool HasHelmet;
					ImVec2 ArmorBarPos;
					memoryManager.ReadMemory(Entity.Controller.Address + Offset.PlayerController.HasHelmet, HasHelmet);
					if (ESPConfig::ShowHealthBar)
						ArmorBarPos = { Rect.x - 10.f,Rect.y };
					else
						ArmorBarPos = { Rect.x - 6.f,Rect.y };
					ImVec2 ArmorBarSize = { 4.f,Rect.w };
					Render::DrawArmorBar(EntityAddress, 100, Entity.Pawn.Armor, HasHelmet, ArmorBarPos, ArmorBarSize);
				}
			}
		}
	}

	Visual(LocalEntity);
	Radar(GameRadar, LocalEntity);
	Trigger(LocalEntity);
	AIM(LocalEntity, AimPosList);
	MiscFuncs(LocalEntity);


	int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
	Log::Debug("currentFPS: " + std::to_string(currentFPS));
	if (currentFPS > MenuConfig::RenderFPS)
	{
		int FrameWait = round(1000.0f / MenuConfig::RenderFPS);
		std::this_thread::sleep_for(std::chrono::milliseconds(FrameWait));
	}
}

void Menu() 
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> difference = now - timepoint;
	SHORT keyState = GetAsyncKeyState(MenuConfig::HotKey);
	if (keyState & 0x8000) {
		keyWasPressed = true;
	}
	if (keyWasPressed && !(keyState & 0x8000) && difference.count() >= 1000)
	{
		MenuConfig::ShowMenu = !MenuConfig::ShowMenu;
		std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
		keyWasPressed = false;
	}

	if (MenuConfig::ShowMenu)
	{
		GUI::DrawGui();
	}

	GUI::InitHitboxList();
}

void Visual(const CEntity& LocalEntity)
{
	// Fov line
	Render::DrawFov(LocalEntity, LegitBotConfig::FovLineSize, LegitBotConfig::FovLineColor, 1);

	// HeadShoot Line
	Render::HeadShootLine(LocalEntity, MiscCFG::HeadShootLineColor);

	Misc::AirCheck(LocalEntity);

	RenderCrosshair(ImGui::GetBackgroundDrawList(), LocalEntity);
}

void Radar(Base_Radar Radar, const CEntity& LocalEntity)
{
	// Radar render
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
	{
		Radar.Render();

		MenuConfig::RadarWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}
}

void Trigger(const CEntity& LocalEntity)
{
	// TriggerBot
	if (LegitBotConfig::TriggerBot && (GetAsyncKeyState(TriggerBot::HotKey) || LegitBotConfig::TriggerAlways))
		TriggerBot::Run(LocalEntity);
}

void AIM(const CEntity& LocalEntity, std::vector<Vec3> AimPosList)
{
	// Aimbot
	DWORD lastTick = 0;
	DWORD currentTick = GetTickCount64();
	if (LegitBotConfig::AimBot) 
	{
		Render::DrawFovCircle(LocalEntity);

		if (LegitBotConfig::AimAlways || GetAsyncKeyState(AimControl::HotKey)) {
			if (AimPosList.size() != 0) {
				AimControl::AimBot(LocalEntity, LocalEntity.Pawn.CameraPos, AimPosList);
			}
		}

		if (LegitBotConfig::AimToggleMode && (GetAsyncKeyState(AimControl::HotKey) & 0x8000) && currentTick - lastTick >= 200) {
			AimControl::switchToggle();
			lastTick = currentTick;
		}
	}

	if (!LegitBotConfig::AimBot)
		RCS::RecoilControl(LocalEntity);

}

void MiscFuncs(CEntity& LocalEntity)
{
	Misc::HitManager(LocalEntity, PreviousTotalHits);
	Misc::BunnyHop(LocalEntity);
	SpecList::SpectatorWindowList(LocalEntity);
	bmb::RenderWindow(LocalEntity.Controller.TeamID);
	Misc::Watermark(LocalEntity);
	//Misc::FastStop();
}

void RadarSetting(Base_Radar& Radar)
{
	// Radar window
	ImGui::SetNextWindowBgAlpha(RadarCFG::RadarBgAlpha);
	ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize({ RadarCFG::RadarRange * 2,RadarCFG::RadarRange * 2 });
	ImGui::SetWindowPos(MenuConfig::RadarWinPos, ImGuiCond_Once);

	if (MenuConfig::RadarWinChengePos)
	{
		ImGui::SetWindowPos("Radar", MenuConfig::RadarWinPos);
		MenuConfig::RadarWinChengePos = false;
	}

	if (!RadarCFG::customRadar)
	{
		RadarCFG::ShowRadarCrossLine = false;
		RadarCFG::Proportion = 2700.f;
		RadarCFG::RadarPointSizeProportion = 1.f;
		RadarCFG::RadarRange = 125.f;
		RadarCFG::RadarBgAlpha = 0.1f;
	}


	// Radar.SetPos({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });
	Radar.SetDrawList(ImGui::GetWindowDrawList());
	Radar.SetPos({ ImGui::GetWindowPos().x + RadarCFG::RadarRange, ImGui::GetWindowPos().y + RadarCFG::RadarRange });
	Radar.SetProportion(RadarCFG::Proportion);
	Radar.SetRange(RadarCFG::RadarRange);
	Radar.SetSize(RadarCFG::RadarRange * 2);
	Radar.SetCrossColor(RadarCFG::RadarCrossLineColor);

	Radar.ArcArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.ArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.CircleSize *= RadarCFG::RadarPointSizeProportion;

	Radar.ShowCrossLine = RadarCFG::ShowRadarCrossLine;
	Radar.Opened = true;
}

void RenderCrosshair(ImDrawList* drawList, const CEntity& LocalEntity)
{
	//if (!CrosshairsCFG::ShowCrossHair || LocalEntity.Controller.TeamID == 0)
	//	return;

	bool isScoped;
	memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);

	std::string curWeapon = TriggerBot::GetWeapon(LocalEntity);
	if (!MiscCFG::SniperCrosshair || LocalEntity.Controller.TeamID == 0 || !TriggerBot::CheckScopeWeapon(curWeapon) || isScoped || MenuConfig::ShowMenu)
		return;

	Render::DrawCrossHair(drawList, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), MiscCFG::SniperCrosshairColor);

	//if (CrosshairsCFG::isAim && MenuConfig::TargetingCrosshairs)
		//Render::DrawCrossHair(drawList, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGui::ColorConvertFloat4ToU32(CrosshairsCFG::TargetedColor));
	//else
		//Render::DrawCrossHair(drawList, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGui::ColorConvertFloat4ToU32(CrosshairsCFG::CrossHairColor));
}
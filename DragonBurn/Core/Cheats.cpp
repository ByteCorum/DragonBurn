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

    // Read matrix once
    if (!memoryManager.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix, sizeof(gGame.View.Matrix)))
        return;

    gGame.UpdateEntityListEntry();

    DWORD64 localControllerAddr = 0, localPawnAddr = 0;
    if (!memoryManager.ReadMemory(gGame.GetLocalControllerAddress(), localControllerAddr) ||
        !memoryManager.ReadMemory(gGame.GetLocalPawnAddress(), localPawnAddr))
        return;

    CEntity localEntity;
    static int localPlayerControllerIndex = 1;
    localEntity.UpdateClientData();

    if (!localEntity.UpdateController(localControllerAddr))
        return;
    if (!localEntity.UpdatePawn(localPawnAddr) && !MenuConfig::WorkInSpec)
        return;

    // Prepare radar if needed
    Base_Radar gameRadar;
    const bool radarActive = RadarCFG::ShowRadar && (localEntity.Controller.TeamID != 0 || MenuConfig::ShowMenu);
    if (radarActive)
        RadarSetting(gameRadar);

    // AimBot data
    std::vector<Vec3> aimPosList;
    aimPosList.reserve(16);
    float MaxAimDistance = 100000;

    // Entity loop
    for (int i = 0; i < 64; ++i)
    {
        DWORD64 entityAddr = 0;
        if (!memoryManager.ReadMemory(gGame.GetEntityListEntry() + (i + 1) * 0x78, entityAddr))
            continue;

        if (entityAddr == localEntity.Controller.Address)
        {
            localPlayerControllerIndex = i;
            continue;
        }

        CEntity entity;
        if (!entity.UpdateController(entityAddr) || !entity.UpdatePawn(entity.Pawn.Address))
            continue;

        SpecList::GetSpectatorList(entity, localEntity);

        if (MenuConfig::TeamCheck && entity.Controller.TeamID == localEntity.Controller.TeamID)
            continue;
        if (!entity.IsAlive())
            continue;

        // Radar
        if (radarActive)
            gameRadar.AddPoint(localEntity.Pawn.Pos, localEntity.Pawn.ViewAngle.y, entity.Pawn.Pos,
                ImColor(237, 85, 106, 200), RadarCFG::RadarType, entity.Pawn.ViewAngle.y);

        if (!entity.IsInScreen())
            continue;

        // AimBot bone selection
        if (!AimControl::HitboxList.empty())
        {
            for (int boneIndex : AimControl::HitboxList)
            {
                auto& bonePosData = entity.GetBone().BonePosList[boneIndex];
                float distToSight = bonePosData.ScreenPos.DistanceTo({ Gui.Window.Size.x / 2, Gui.Window.Size.y / 2 });

                if (distToSight < MaxAimDistance)
                {
                    if (!LegitBotConfig::VisibleCheck ||
                        (entity.Pawn.bSpottedByMask & (1ULL << localPlayerControllerIndex)) ||
                        (localEntity.Pawn.bSpottedByMask & (1ULL << i)))
                    {
                        Vec3 aimPos = bonePosData.Pos;
                        if (boneIndex == BONEINDEX::head)
                            aimPos.z -= 1.f;

                        aimPosList.push_back(aimPos);
                        MaxAimDistance = distToSight;
                    }
                }
            }
        }

        // ESP
        if (ESPConfig::ESPenabled)
        {
            ImVec4 rect = ESP::GetBoxRect(entity, ESPConfig::BoxType);
            int distance = static_cast<int>(entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) / 100);

            if (MenuConfig::RenderDistance == 0 || distance <= MenuConfig::RenderDistance)
            {
                ESP::RenderPlayerESP(localEntity, entity, rect, localPlayerControllerIndex, i);
                Render::DrawDistance(localEntity, entity, rect);

                // Health Bar
                if (ESPConfig::ShowHealthBar)
                {
                    Render::DrawHealthBar(entityAddr, 100, entity.Pawn.Health,
                        { rect.x - 6.f, rect.y }, { 4, rect.w });
                }

                // Ammo Bar
                if (ESPConfig::AmmoBar && entity.Pawn.Ammo != -1)
                {
                    Render::DrawAmmoBar(entityAddr, entity.Pawn.Ammo + entity.Pawn.ShotsFired, entity.Pawn.Ammo,
                        { rect.x, rect.y + rect.w + 2 }, { rect.z, 4 });
                }

                // Armor Bar
                if (ESPConfig::ArmorBar && entity.Pawn.Armor > 0)
                {
                    bool hasHelmet = false;
                    memoryManager.ReadMemory(entity.Controller.Address + Offset.PlayerController.HasHelmet, hasHelmet);

                    ImVec2 armorBarPos = ESPConfig::ShowHealthBar ?
                        ImVec2{ rect.x - 10.f, rect.y } : ImVec2{ rect.x - 6.f, rect.y };

                    Render::DrawArmorBar(entityAddr, 100, entity.Pawn.Armor, hasHelmet, armorBarPos, { 4.f, rect.w });
                }
            }
        }
    }

    // Post-loop features
    Visual(localEntity);
    if (radarActive) Radar(gameRadar, localEntity);
    Trigger(localEntity);
    AIM(localEntity, aimPosList);
    MiscFuncs(localEntity);

    // Frame limiting
    int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
    if (currentFPS > MenuConfig::RenderFPS)
    {
        int frameWait = static_cast<int>(1000.0f / MenuConfig::RenderFPS);
        std::this_thread::sleep_for(std::chrono::milliseconds(frameWait));
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
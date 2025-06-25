#include "Offsets.h"
#include <fstream>
#include <filesystem>

Offsets::Offsets() {}

Offsets::~Offsets() {}

void Offsets::SetOffsets(const std::string& offsetsData, const std::string& buttonsData, const std::string& client_dllData)
{
    json offsetsJson = json::parse(offsetsData);
    json buttonsJson = json::parse(buttonsData);
    json client_dllJson = json::parse(client_dllData)["client.dll"]["classes"];

    this->EntityList = offsetsJson["client.dll"]["dwEntityList"];
    this->Matrix = offsetsJson["client.dll"]["dwViewMatrix"];
    this->ViewAngle = offsetsJson["client.dll"]["dwViewAngles"];
    this->LocalPlayerController = offsetsJson["client.dll"]["dwLocalPlayerController"];
    this->LocalPlayerPawn = offsetsJson["client.dll"]["dwLocalPlayerPawn"];
    this->GlobalVars = offsetsJson["client.dll"]["dwGlobalVars"];
    this->PlantedC4 = offsetsJson["client.dll"]["dwPlantedC4"];
    this->InputSystem = offsetsJson["inputsystem.dll"]["dwInputSystem"];
    this->Sensitivity = offsetsJson["client.dll"]["dwSensitivity"];

    this->Buttons.Attack = buttonsJson["client.dll"]["attack"];
    this->Buttons.Jump = buttonsJson["client.dll"]["jump"];
    this->Buttons.Right = buttonsJson["client.dll"]["right"];
    this->Buttons.Left = buttonsJson["client.dll"]["left"];

    this->Entity.IsAlive = client_dllJson["CCSPlayerController"]["fields"]["m_bPawnIsAlive"];
    this->Entity.PlayerPawn = client_dllJson["CCSPlayerController"]["fields"]["m_hPlayerPawn"];
    this->Entity.iszPlayerName = client_dllJson["CBasePlayerController"]["fields"]["m_iszPlayerName"];

    this->Pawn.BulletServices = client_dllJson["C_CSPlayerPawn"]["fields"]["m_pBulletServices"];
    this->Pawn.CameraServices = client_dllJson["C_BasePlayerPawn"]["fields"]["m_pCameraServices"];
    this->Pawn.pClippingWeapon = client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_pClippingWeapon"];
    this->Pawn.isScoped = client_dllJson["C_CSPlayerPawn"]["fields"]["m_bIsScoped"];
    this->Pawn.isDefusing = client_dllJson["C_CSPlayerPawn"]["fields"]["m_bIsDefusing"];
    this->Pawn.TotalHit = client_dllJson["CCSPlayer_BulletServices"]["fields"]["m_totalHitsOnServer"];
    this->Pawn.Pos = client_dllJson["C_BasePlayerPawn"]["fields"]["m_vOldOrigin"];
    this->Pawn.CurrentArmor = client_dllJson["C_CSPlayerPawn"]["fields"]["m_ArmorValue"];
    this->Pawn.MaxHealth = client_dllJson["C_BaseEntity"]["fields"]["m_iMaxHealth"];
    this->Pawn.CurrentHealth = client_dllJson["C_BaseEntity"]["fields"]["m_iHealth"];
    this->Pawn.GameSceneNode = client_dllJson["C_BaseEntity"]["fields"]["m_pGameSceneNode"];
    this->Pawn.BoneArray = 0x1F0;
    this->Pawn.angEyeAngles = client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_angEyeAngles"];
    this->Pawn.vecLastClipCameraPos = client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_vecLastClipCameraPos"];
    this->Pawn.iShotsFired = client_dllJson["C_CSPlayerPawn"]["fields"]["m_iShotsFired"];
    this->Pawn.flFlashDuration = client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_flFlashDuration"];
    this->Pawn.aimPunchAngle = client_dllJson["C_CSPlayerPawn"]["fields"]["m_aimPunchAngle"];
    this->Pawn.aimPunchCache = client_dllJson["C_CSPlayerPawn"]["fields"]["m_aimPunchCache"];
    this->Pawn.iIDEntIndex = client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_iIDEntIndex"];
    this->Pawn.iTeamNum = client_dllJson["C_BaseEntity"]["fields"]["m_iTeamNum"];
    this->Pawn.iFovStart = client_dllJson["CCSPlayerBase_CameraServices"]["fields"]["m_iFOVStart"];
    this->Pawn.fFlags = client_dllJson["C_BaseEntity"]["fields"]["m_fFlags"];
    this->Pawn.bSpottedByMask = DWORD(client_dllJson["C_CSPlayerPawn"]["fields"]["m_entitySpottedState"]) + DWORD(client_dllJson["EntitySpottedState_t"]["fields"]["m_bSpottedByMask"]);
    this->Pawn.AbsVelocity = client_dllJson["C_BaseEntity"]["fields"]["m_vecAbsVelocity"];
    this->Pawn.m_bWaitForNoAttack = client_dllJson["C_CSPlayerPawn"]["fields"]["m_bWaitForNoAttack"];

    this->GlobalVar.RealTime = 0x00;
    this->GlobalVar.FrameCount = 0x04;
    this->GlobalVar.MaxClients = 0x10;
    this->GlobalVar.IntervalPerTick = 0x14;
    this->GlobalVar.CurrentTime = 0x2C;
    this->GlobalVar.CurrentTime2 = 0x30;
    this->GlobalVar.TickCount = 0x40;
    this->GlobalVar.IntervalPerTick2 = 0x44;
    this->GlobalVar.CurrentNetchan = 0x0048;
    this->GlobalVar.CurrentMap = 0x0180;
    this->GlobalVar.CurrentMapName = 0x0188;

    this->PlayerController.m_steamID = client_dllJson["CBasePlayerController"]["fields"]["m_steamID"];
    this->PlayerController.m_hPawn = client_dllJson["CBasePlayerController"]["fields"]["m_hPawn"];
    this->PlayerController.m_pObserverServices = client_dllJson["C_BasePlayerPawn"]["fields"]["m_pObserverServices"];
    this->PlayerController.m_hObserverTarget = client_dllJson["CPlayer_ObserverServices"]["fields"]["m_hObserverTarget"];
    this->PlayerController.m_hController = client_dllJson["C_BasePlayerPawn"]["fields"]["m_hController"];
    this->PlayerController.PawnArmor = client_dllJson["CCSPlayerController"]["fields"]["m_iPawnArmor"];
    this->PlayerController.HasDefuser = client_dllJson["CCSPlayerController"]["fields"]["m_bPawnHasDefuser"];
    this->PlayerController.HasHelmet = client_dllJson["CCSPlayerController"]["fields"]["m_bPawnHasHelmet"];

    this->EconEntity.AttributeManager = client_dllJson["C_EconEntity"]["fields"]["m_AttributeManager"];

    this->WeaponBaseData.WeaponDataPTR = DWORD(client_dllJson["C_BaseEntity"]["fields"]["m_nSubclassID"]) + 0x08;
    this->WeaponBaseData.szName = client_dllJson["CCSWeaponBaseVData"]["fields"]["m_szName"];
    this->WeaponBaseData.Clip1 = client_dllJson["C_BasePlayerWeapon"]["fields"]["m_iClip1"];
    this->WeaponBaseData.MaxClip = client_dllJson["CBasePlayerWeaponVData"]["fields"]["m_iMaxClip1"];
    this->WeaponBaseData.Item = client_dllJson["C_AttributeContainer"]["fields"]["m_Item"];
    this->WeaponBaseData.ItemDefinitionIndex = client_dllJson["C_EconItemView"]["fields"]["m_iItemDefinitionIndex"];

    this->C4.m_bBeingDefused = client_dllJson["C_PlantedC4"]["fields"]["m_bBeingDefused"];
    this->C4.m_flDefuseCountDown = client_dllJson["C_PlantedC4"]["fields"]["m_flDefuseCountDown"];
    this->C4.m_nBombSite = client_dllJson["C_PlantedC4"]["fields"]["m_nBombSite"];
}

bool Offsets::LoadOffsetsFromFile(const std::string& filename)
{
    try {
        if (!std::filesystem::exists(filename)) {
            return false;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        json cachedData;
        file >> cachedData;
        file.close();

        // Load the cached data into SetOffsets
        SetOffsets(
            cachedData["offsets"].dump(),
            cachedData["buttons"].dump(),
            cachedData["client_dll"].dump()
        );

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool Offsets::SaveOffsetsToFile(const std::string& filename, const std::string& offsetsData,
    const std::string& buttonsData, const std::string& client_dllData)
{
    try {
        json combinedData;
        combinedData["offsets"] = json::parse(offsetsData);
        combinedData["buttons"] = json::parse(buttonsData);
        combinedData["client_dll"] = json::parse(client_dllData);

        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << combinedData.dump(4); // Pretty print with 4 spaces
        file.close();

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool Offsets::CompareOffsets(const std::string& filename, const std::string& offsetsData,
    const std::string& buttonsData, const std::string& client_dllData)
{
    try {
        if (!std::filesystem::exists(filename)) {
            return false; // No file to compare with
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        json cachedData;
        file >> cachedData;
        file.close();

        // Parse new data
        json newOffsets = json::parse(offsetsData);
        json newButtons = json::parse(buttonsData);
        json newClientDll = json::parse(client_dllData);

        // Compare each section
        bool offsetsMatch = (cachedData["offsets"] == newOffsets);
        bool buttonsMatch = (cachedData["buttons"] == newButtons);
        bool clientDllMatch = (cachedData["client_dll"] == newClientDll);

        return (offsetsMatch && buttonsMatch && clientDllMatch);
    }
    catch (const std::exception& e) {
        return false;
    }
}

int Offsets::UpdateOffsets()
{
    const std::string offsetsUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json";
    const std::string buttonsUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json";
    const std::string client_dllUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json";
    const std::string cacheFilename = "offsets_cache.json";

    std::string offsetsData, buttonsData, client_dllData;

    // Check if we have internet connection
    if (!Web::CheckConnection()) {
        // No internet connection, try to load from cache
        if (LoadOffsetsFromFile(cacheFilename)) {
            return 3; // Successfully loaded from cache (offline mode)
        }
        else {
            return 0; // No internet and no cache file
        }
    }

    // Try to fetch data from online sources
    if (!Web::Get(offsetsUrl, offsetsData) || !Web::Get(buttonsUrl, buttonsData) || !Web::Get(client_dllUrl, client_dllData)) {
        // Failed to fetch online data, try to load from cache
        if (LoadOffsetsFromFile(cacheFilename)) {
            return 3; // Successfully loaded from cache (fallback)
        }
        else {
            return 1; // Failed to fetch online and no cache available
        }
    }

    // Successfully fetched online data, now compare with cache
    if (CompareOffsets(cacheFilename, offsetsData, buttonsData, client_dllData)) {
        // Data is the same, just load from memory (already have the online data)
        SetOffsets(offsetsData, buttonsData, client_dllData);
        return 4; // Offsets are up to date (no changes)
    }

    // Data is different or no cache exists, update cache and load new data
    SetOffsets(offsetsData, buttonsData, client_dllData);

    if (SaveOffsetsToFile(cacheFilename, offsetsData, buttonsData, client_dllData)) {
        return 2; // Successfully updated offsets and saved to cache
    }
    else {
        return 5; // Updated offsets but failed to save cache
    }
}
#include "Offsets.h"
#include "../Core/Init.h"
#include "../Helpers/StorageMgr.h"
#include <sstream>
#include <vector>
#include <iostream>

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

json getJsonByPath(const json& j, const std::vector<std::string>& path) {
    json current = j;
    for (const auto& key : path) {
        if (!current.is_object() || !current.contains(key)) {
            return json();
        }
        current = current[key];
    }
    return current;
}

Offsets::Offsets() {}

Offsets::~Offsets() {}

void Offsets::SetOffsets(const std::string& offsetsData, const std::string& buttonsData, const std::string& client_dllData)
{
    try {
        json offsetsJson = json::parse(offsetsData);
        json buttonsJson = json::parse(buttonsData);
        json client_dllJson = json::parse(client_dllData);

        json client_classes = json::object();
        if (client_dllJson.contains("client.dll") &&
            client_dllJson["client.dll"].is_object() &&
            client_dllJson["client.dll"].contains("classes") &&
            client_dllJson["client.dll"]["classes"].is_object()) {
            client_classes = client_dllJson["client.dll"]["classes"];
        }

        auto safeGetDWORD = [](const json& j, const std::vector<std::string>& path, DWORD defaultValue = 0) -> DWORD {
            json value = getJsonByPath(j, path);
            if (value.is_number()) {
                return value.get<DWORD>();
            }
            return defaultValue;
            };

        this->EntityList = safeGetDWORD(offsetsJson, { "client.dll", "dwEntityList" });
        this->Matrix = safeGetDWORD(offsetsJson, { "client.dll", "dwViewMatrix" });
        this->ViewAngle = safeGetDWORD(offsetsJson, { "client.dll", "dwViewAngles" });
        this->LocalPlayerController = safeGetDWORD(offsetsJson, { "client.dll", "dwLocalPlayerController" });
        this->LocalPlayerPawn = safeGetDWORD(offsetsJson, { "client.dll", "dwLocalPlayerPawn" });
        this->GlobalVars = safeGetDWORD(offsetsJson, { "client.dll", "dwGlobalVars" });
        this->PlantedC4 = safeGetDWORD(offsetsJson, { "client.dll", "dwPlantedC4" });

        if (offsetsJson.contains("inputsystem.dll") &&
            offsetsJson["inputsystem.dll"].is_object() &&
            offsetsJson["inputsystem.dll"].contains("dwInputSystem")) {
            this->InputSystem = safeGetDWORD(offsetsJson, { "inputsystem.dll", "dwInputSystem" });
        }
        else {
            this->InputSystem = 0;
        }

        this->Sensitivity = safeGetDWORD(offsetsJson, { "client.dll", "dwSensitivity" });
        this->Sensitivity_sensitivity = safeGetDWORD(offsetsJson, { "client.dll", "dwSensitivity_sensitivity" });

        if (buttonsJson.contains("client.dll") && buttonsJson["client.dll"].is_object()) {
            this->Buttons.Attack = safeGetDWORD(buttonsJson, { "client.dll", "attack" });
            this->Buttons.Jump = safeGetDWORD(buttonsJson, { "client.dll", "jump" });
            this->Buttons.Right = safeGetDWORD(buttonsJson, { "client.dll", "right" });
            this->Buttons.Left = safeGetDWORD(buttonsJson, { "client.dll", "left" });
        }

        if (client_classes.contains("CCSPlayerController")) {
            json ccsPlayerController = client_classes["CCSPlayerController"];
            if (ccsPlayerController.contains("fields") && ccsPlayerController["fields"].is_object()) {
                this->Entity.IsAlive = safeGetDWORD(ccsPlayerController["fields"], { "m_bPawnIsAlive" });
                this->Entity.PlayerPawn = safeGetDWORD(ccsPlayerController["fields"], { "m_hPlayerPawn" });
            }
        }

        if (client_classes.contains("CBasePlayerController")) {
            json cBasePlayerController = client_classes["CBasePlayerController"];
            if (cBasePlayerController.contains("fields") && cBasePlayerController["fields"].is_object()) {
                this->Entity.iszPlayerName = safeGetDWORD(cBasePlayerController["fields"], { "m_iszPlayerName" });
            }
        }

        auto getPawnField = [&](const std::string& className, const std::string& fieldName) -> DWORD {
            if (client_classes.contains(className)) {
                json cls = client_classes[className];
                if (cls.contains("fields") && cls["fields"].is_object() &&
                    cls["fields"].contains(fieldName)) {
                    return safeGetDWORD(cls["fields"], { fieldName });
                }
            }
            return 0;
            };

        this->Pawn.BulletServices = getPawnField("C_CSPlayerPawn", "m_pBulletServices");
        this->Pawn.CameraServices = getPawnField("C_BasePlayerPawn", "m_pCameraServices");
        this->Pawn.pClippingWeapon = getPawnField("C_CSPlayerPawn", "m_pClippingWeapon");
        this->Pawn.isScoped = getPawnField("C_CSPlayerPawn", "m_bIsScoped");
        this->Pawn.isDefusing = getPawnField("C_CSPlayerPawn", "m_bIsDefusing");
        this->Pawn.TotalHit = getPawnField("CCSPlayer_BulletServices", "m_totalHitsOnServer");
        this->Pawn.Pos = getPawnField("C_BasePlayerPawn", "m_vOldOrigin");
        this->Pawn.CurrentArmor = getPawnField("C_CSPlayerPawn", "m_ArmorValue");
        this->Pawn.MaxHealth = getPawnField("C_BaseEntity", "m_iMaxHealth");
        this->Pawn.CurrentHealth = getPawnField("C_BaseEntity", "m_iHealth");
        this->Pawn.GameSceneNode = getPawnField("C_BaseEntity", "m_pGameSceneNode");

        DWORD modelState = getPawnField("CSkeletonInstance", "m_modelState");
        this->Pawn.BoneArray = modelState + 0x80;

        this->Pawn.angEyeAngles = getPawnField("C_CSPlayerPawn", "m_angEyeAngles");
        this->Pawn.vecLastClipCameraPos = getPawnField("C_CSPlayerPawn", "m_vecLastClipCameraPos");
        this->Pawn.iShotsFired = getPawnField("C_CSPlayerPawn", "m_iShotsFired");
        this->Pawn.flFlashDuration = getPawnField("C_CSPlayerPawnBase", "m_flFlashDuration");
        this->Pawn.aimPunchAngle = getPawnField("C_CSPlayerPawn", "m_aimPunchAngle");
        this->Pawn.aimPunchCache = getPawnField("C_CSPlayerPawn", "m_aimPunchCache");
        this->Pawn.iIDEntIndex = getPawnField("C_CSPlayerPawn", "m_iIDEntIndex");
        this->Pawn.iTeamNum = getPawnField("C_BaseEntity", "m_iTeamNum");
        this->Pawn.iFovStart = getPawnField("CCSPlayerBase_CameraServices", "m_iFOVStart");
        this->Pawn.fFlags = getPawnField("C_BaseEntity", "m_fFlags");
        this->Pawn.AbsVelocity = getPawnField("C_BaseEntity", "m_vecAbsVelocity");
        this->Pawn.m_bWaitForNoAttack = getPawnField("C_CSPlayerPawn", "m_bWaitForNoAttack");
        this->Pawn.m_pWeaponServices = getPawnField("C_BasePlayerPawn", "m_pWeaponServices");
        this->Pawn.m_flEmitSoundTime = getPawnField("C_CSPlayerPawn", "m_flEmitSoundTime");

        if (client_classes.contains("C_CSPlayerPawn") &&
            client_classes["C_CSPlayerPawn"].contains("fields")) {
            json csPlayerPawnFields = client_classes["C_CSPlayerPawn"]["fields"];
            if (csPlayerPawnFields.contains("m_entitySpottedState") &&
                client_classes.contains("EntitySpottedState_t") &&
                client_classes["EntitySpottedState_t"].contains("fields")) {

                DWORD entitySpottedState = safeGetDWORD(csPlayerPawnFields, { "m_entitySpottedState" });
                DWORD spottedByMask = safeGetDWORD(client_classes["EntitySpottedState_t"]["fields"], { "m_bSpottedByMask" });
                this->Pawn.bSpottedByMask = entitySpottedState + spottedByMask;
            }
            else {
                this->Pawn.bSpottedByMask = 0;
            }
        }

        this->GlobalVar.RealTime = 0x00;
        this->GlobalVar.FrameCount = 0x04;
        this->GlobalVar.MaxClients = 0x10;
        this->GlobalVar.IntervalPerTick = 0x14;
        this->GlobalVar.CurrentTime = 0x30;
        this->GlobalVar.CurrentTime2 = 0x38;
        this->GlobalVar.TickCount = 0x48;
        this->GlobalVar.IntervalPerTick2 = 0x44;
        this->GlobalVar.CurrentNetchan = 0x0048;
        this->GlobalVar.CurrentMap = 0x0180;
        this->GlobalVar.CurrentMapName = 0x0188;

        this->PlayerController.m_nTickBase = getPawnField("CBasePlayerController", "m_nTickBase");
        this->PlayerController.m_steamID = getPawnField("CBasePlayerController", "m_steamID");
        this->PlayerController.m_hPawn = getPawnField("CBasePlayerController", "m_hPawn");
        this->PlayerController.m_pObserverServices = getPawnField("C_BasePlayerPawn", "m_pObserverServices");
        this->PlayerController.m_hObserverTarget = getPawnField("CPlayer_ObserverServices", "m_hObserverTarget");
        this->PlayerController.m_hController = getPawnField("C_BasePlayerPawn", "m_hController");
        this->PlayerController.PawnArmor = getPawnField("CCSPlayerController", "m_iPawnArmor");
        this->PlayerController.HasDefuser = getPawnField("CCSPlayerController", "m_bPawnHasDefuser");
        this->PlayerController.HasHelmet = getPawnField("CCSPlayerController", "m_bPawnHasHelmet");

        this->EconEntity.AttributeManager = getPawnField("C_EconEntity", "m_AttributeManager");

        DWORD subclassID = getPawnField("C_BaseEntity", "m_nSubclassID");
        this->WeaponBaseData.WeaponDataPTR = subclassID + 0x08;
        this->WeaponBaseData.szName = getPawnField("CCSWeaponBaseVData", "m_szName");
        this->WeaponBaseData.Clip1 = getPawnField("C_BasePlayerWeapon", "m_iClip1");
        this->WeaponBaseData.MaxClip = getPawnField("CBasePlayerWeaponVData", "m_iMaxClip1");
        this->WeaponBaseData.Item = getPawnField("C_AttributeContainer", "m_Item");
        this->WeaponBaseData.ItemDefinitionIndex = getPawnField("C_EconItemView", "m_iItemDefinitionIndex");
        this->WeaponBaseData.hMyWeapons = getPawnField("CPlayer_WeaponServices", "m_hMyWeapons");

        this->C4.m_bBeingDefused = getPawnField("C_PlantedC4", "m_bBeingDefused");
        this->C4.m_flDefuseCountDown = getPawnField("C_PlantedC4", "m_flDefuseCountDown");
        this->C4.m_nBombSite = getPawnField("C_PlantedC4", "m_nBombSite");

    }
    catch (const json::exception& e) {
        std::cerr << "[ERROR] JSON parsing error in SetOffsets: " << e.what() << std::endl;
        std::cerr << "[ERROR] Error id: " << e.id << std::endl;
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Error in SetOffsets: " << e.what() << std::endl;
        throw;
    }
}

void Offsets::UpdateOffsets()
{
    std::string offsets, buttons, client_dll;

    try {
        std::string infoStr = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/info.json");
        json infoJson = json::parse(infoStr);

        if (!infoJson.contains("build_number") || !infoJson["build_number"].is_number()) {
            throw std::runtime_error("Invalid info.json: build_number not found or not a number");
        }

        std::string gameBuildNum = std::to_string(infoJson["build_number"].get<int>());

        json GamaDataStorage = json::object();
        try {
            std::string storageContent = storage::ReadStorageFile("gamedata.json");
            if (!storageContent.empty()) {
                GamaDataStorage = json::parse(storageContent);
            }
        }
        catch (const std::exception& e) {
            GamaDataStorage = json::object();
        }

        bool needUpdate = false;

        if (!GamaDataStorage.contains("build-number") ||
            !GamaDataStorage["build-number"].is_string() ||
            GamaDataStorage["build-number"].get<std::string>() != gameBuildNum) {
            needUpdate = true;
        }
        else {
            try {
                offsets = storage::ReadStorageFile("offsets.json");
                buttons = storage::ReadStorageFile("buttons.json");
                client_dll = storage::ReadStorageFile("client_dll.json");

                if (offsets.empty() || buttons.empty() || client_dll.empty()) {
                    throw std::runtime_error("One or more offset files are empty");
                }

                json::parse(offsets);
                json::parse(buttons);
                json::parse(client_dll);

            }
            catch (const std::exception& e) {
                needUpdate = true;
            }
        }

        if (needUpdate) {
            offsets = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/offsets.json");
            buttons = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/buttons.json");
            client_dll = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/client_dll.json");

            if (offsets.empty() || buttons.empty() || client_dll.empty()) {
                throw std::runtime_error("Failed to download offsets from GitHub (empty response)");
            }

            storage::WriteStorageFile("offsets.json", offsets);
            storage::WriteStorageFile("buttons.json", buttons);
            storage::WriteStorageFile("client_dll.json", client_dll);

            GamaDataStorage["build-number"] = gameBuildNum;
            storage::WriteStorageFile("gamedata.json", GamaDataStorage.dump(4));
        }

        SetOffsets(offsets, buttons, client_dll);

    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to update offsets: " << e.what() << std::endl;

        try {
            offsets = storage::ReadStorageFile("offsets.json");
            buttons = storage::ReadStorageFile("buttons.json");
            client_dll = storage::ReadStorageFile("client_dll.json");

            if (!offsets.empty() && !buttons.empty() && !client_dll.empty()) {
                SetOffsets(offsets, buttons, client_dll);
            }
            else {
                throw std::runtime_error("Cached offsets are also unavailable");
            }
        }
        catch (...) {
            throw std::runtime_error("All attempts to load offsets failed");
        }
    }
}
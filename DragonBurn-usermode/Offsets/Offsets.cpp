#include "Offsets.h"
#include "../Core/Init.h"
#include "../Helpers/StorageMgr.h"

Offsets::Offsets() {}

Offsets::~Offsets() {}

void Offsets::SetOffsets(const std::string& offsetsData, const std::string& buttonsData, const std::string& client_dllData)
{
    try {
        json offsetsJson = json::parse(offsetsData);
        json buttonsJson = json::parse(buttonsData);
        json client_dllJson = json::parse(client_dllData)["client.dll"]["classes"];

        auto get_offset = [&](json& j, const std::string& key) -> DWORD {
            if (j.contains(key) && !j[key].is_null())
                return j[key].get<DWORD>();
            return 0;
        };

        auto get_class_field = [&](const std::string& className, const std::string& fieldName) -> DWORD {
            if (client_dllJson.contains(className) && 
                client_dllJson[className].contains("fields") && 
                client_dllJson[className]["fields"].contains(fieldName)) {
                return client_dllJson[className]["fields"][fieldName].get<DWORD>();
            }
            return 0;
        };

        this->EntityList = offsetsJson["client.dll"]["dwEntityList"];
        this->Matrix = offsetsJson["client.dll"]["dwViewMatrix"];
        this->ViewAngle = offsetsJson["client.dll"]["dwViewAngles"];
        this->LocalPlayerController = offsetsJson["client.dll"]["dwLocalPlayerController"];
        this->LocalPlayerPawn = offsetsJson["client.dll"]["dwLocalPlayerPawn"];
        this->GlobalVars = offsetsJson["client.dll"]["dwGlobalVars"];
        this->PlantedC4 = offsetsJson["client.dll"]["dwPlantedC4"];
        this->InputSystem = offsetsJson["inputsystem.dll"]["dwInputSystem"];
        this->Sensitivity = offsetsJson["client.dll"]["dwSensitivity"];
        this->Sensitivity_sensitivity = offsetsJson["client.dll"]["dwSensitivity_sensitivity"];

        this->Buttons.Attack = buttonsJson["client.dll"]["attack"];
        this->Buttons.Jump = buttonsJson["client.dll"]["jump"];
        this->Buttons.Right = buttonsJson["client.dll"]["right"];
        this->Buttons.Left = buttonsJson["client.dll"]["left"];

        this->Entity.IsAlive = get_class_field("CCSPlayerController", "m_bPawnIsAlive");
        this->Entity.PlayerPawn = get_class_field("CCSPlayerController", "m_hPlayerPawn");
        this->Entity.iszPlayerName = get_class_field("CBasePlayerController", "m_iszPlayerName");

        this->Pawn.BulletServices = get_class_field("C_CSPlayerPawn", "m_pBulletServices");
        this->Pawn.CameraServices = get_class_field("C_BasePlayerPawn", "m_pCameraServices");
        this->Pawn.pClippingWeapon = get_class_field("C_CSPlayerPawn", "m_pClippingWeapon");
        this->Pawn.isScoped = get_class_field("C_CSPlayerPawn", "m_bIsScoped");
        this->Pawn.isDefusing = get_class_field("C_CSPlayerPawn", "m_bIsDefusing");
        this->Pawn.TotalHit = get_class_field("CCSPlayer_BulletServices", "m_totalHitsOnServer");
        this->Pawn.Pos = get_class_field("C_BasePlayerPawn", "m_vOldOrigin");
        this->Pawn.CurrentArmor = get_class_field("C_CSPlayerPawn", "m_ArmorValue");
        this->Pawn.MaxHealth = get_class_field("C_BaseEntity", "m_iMaxHealth");
        this->Pawn.CurrentHealth = get_class_field("C_BaseEntity", "m_iHealth");
        this->Pawn.GameSceneNode = get_class_field("C_BaseEntity", "m_pGameSceneNode");
        this->Pawn.BoneArray = get_class_field("CSkeletonInstance", "m_modelState") + 0x80;
        this->Pawn.angEyeAngles = get_class_field("C_CSPlayerPawn", "m_angEyeAngles");
        this->Pawn.vecLastClipCameraPos = get_class_field("C_CSPlayerPawn", "m_vecLastClipCameraPos");
        this->Pawn.iShotsFired = get_class_field("C_CSPlayerPawn", "m_iShotsFired");
        this->Pawn.flFlashDuration = get_class_field("C_CSPlayerPawnBase", "m_flFlashDuration");
        this->Pawn.aimPunchAngle = get_class_field("C_CSPlayerPawn", "m_aimPunchAngle");
        this->Pawn.aimPunchCache = get_class_field("C_CSPlayerPawn", "m_aimPunchCache");
        this->Pawn.iIDEntIndex = get_class_field("C_CSPlayerPawn", "m_iIDEntIndex");
        this->Pawn.iTeamNum = get_class_field("C_BaseEntity", "m_iTeamNum");
        this->Pawn.iFovStart = get_class_field("CCSPlayerBase_CameraServices", "m_iFOVStart");
        this->Pawn.fFlags = get_class_field("C_BaseEntity", "m_fFlags");
        this->Pawn.bSpottedByMask = get_class_field("C_CSPlayerPawn", "m_entitySpottedState") + get_class_field("EntitySpottedState_t", "m_bSpottedByMask");
        this->Pawn.AbsVelocity = get_class_field("C_BaseEntity", "m_vecAbsVelocity");
        this->Pawn.m_bWaitForNoAttack = get_class_field("C_CSPlayerPawn", "m_bWaitForNoAttack");
        this->Pawn.m_pWeaponServices = get_class_field("C_BasePlayerPawn", "m_pWeaponServices");
        this->Pawn.m_flEmitSoundTime = get_class_field("C_CSPlayerPawn", "m_flEmitSoundTime");

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

        this->PlayerController.m_nTickBase = get_class_field("CBasePlayerController", "m_nTickBase");
        this->PlayerController.m_steamID = get_class_field("CBasePlayerController", "m_steamID");
        this->PlayerController.m_hPawn = get_class_field("CBasePlayerController", "m_hPawn");
        this->PlayerController.m_pObserverServices = get_class_field("C_BasePlayerPawn", "m_pObserverServices");
        this->PlayerController.m_hObserverTarget = get_class_field("CPlayer_ObserverServices", "m_hObserverTarget");
        this->PlayerController.m_hController = get_class_field("C_BasePlayerPawn", "m_hController");
        this->PlayerController.PawnArmor = get_class_field("CCSPlayerController", "m_iPawnArmor");
        this->PlayerController.HasDefuser = get_class_field("CCSPlayerController", "m_bPawnHasDefuser");
        this->PlayerController.HasHelmet = get_class_field("CCSPlayerController", "m_bPawnHasHelmet");

        this->EconEntity.AttributeManager = get_class_field("C_EconEntity", "m_AttributeManager");

        this->WeaponBaseData.WeaponDataPTR = get_class_field("C_BaseEntity", "m_nSubclassID") + 0x08;
        this->WeaponBaseData.szName = get_class_field("CCSWeaponBaseVData", "m_szName");
        this->WeaponBaseData.Clip1 = get_class_field("C_BasePlayerWeapon", "m_iClip1");
        this->WeaponBaseData.MaxClip = get_class_field("CBasePlayerWeaponVData", "m_iMaxClip1");
        this->WeaponBaseData.Item = get_class_field("C_AttributeContainer", "m_Item");
        this->WeaponBaseData.ItemDefinitionIndex = get_class_field("C_EconItemView", "m_iItemDefinitionIndex");
        this->WeaponBaseData.hMyWeapons = get_class_field("CPlayer_WeaponServices", "m_hMyWeapons");

        this->C4.m_bBeingDefused = get_class_field("C_PlantedC4", "m_bBeingDefused");
        this->C4.m_flDefuseCountDown = get_class_field("C_PlantedC4", "m_flDefuseCountDown");
        this->C4.m_nBombSite = get_class_field("C_PlantedC4", "m_nBombSite");
    }
    catch (const std::exception& e) {
        std::cout << "[X] Offset parsing error: " << e.what() << std::endl;
    }
}

void Offsets::UpdateOffsets()
{
    std::string offsets, buttons, client_dll;
    
    bool forceUpdate = false;
    std::string dataPath = MenuConfig::path + "\\Data\\";
    
    try
    {
        if (!storage::FileExists("offsets.json") || !storage::FileExists("buttons.json") || !storage::FileExists("client_dll.json"))
        {
            forceUpdate = true;
        }
        else
        {
            // Check if files are older than 12 hours
            auto now = std::filesystem::file_time_type::clock::now();
            auto lastWrite = std::filesystem::last_write_time(dataPath + "offsets.json");
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - lastWrite).count();
            
            if (age >= 12)
                forceUpdate = true;
        }
    }
    catch (...)
    {
        forceUpdate = true;
    }

    try
    {
        if (!forceUpdate)
        {
            offsets = storage::ReadStorageFile("offsets.json");
            buttons = storage::ReadStorageFile("buttons.json");
            client_dll = storage::ReadStorageFile("client_dll.json");
        }
        else
        {
            throw std::runtime_error("force update");
        }
    }
    catch (...)
    {
        std::cout << "[i] Downloading latest offsets from cloud..." << std::endl;
        
        offsets = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json");
        buttons = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json");
        client_dll = Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json");

        storage::WriteStorageFile("offsets.json", offsets);
        storage::WriteStorageFile("buttons.json", buttons);
        storage::WriteStorageFile("client_dll.json", client_dll);
        
        std::cout << "[+] Offsets updated successfully" << std::endl;
    }
    SetOffsets(offsets, buttons, client_dll);
}

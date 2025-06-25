#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <string>
#include <future>
#include <iostream>
#include <filesystem>
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
#include "../Core/GlobalVars.h"
#include "Threading.h"

namespace Cheats {

    // Global instances
    std::unique_ptr<CheatManager> g_cheatManager = nullptr;
    std::unique_ptr<MapManager> g_mapManager = nullptr;
    std::unique_ptr<globalvars> g_globalVars = nullptr;

    // Static variables
    static bool s_keyWasPressed = false;
    static std::chrono::time_point<std::chrono::system_clock> s_lastKeyPressTime;

    //=============================================================================
    // CheatManager
    //=============================================================================

    CheatManager::CheatManager()
        : m_initialized(false)
        , m_previousTotalHits(0)
        , m_localPlayerControllerIndex(1) {
    }

    CheatManager::~CheatManager() {
        Shutdown();
    }

    bool CheatManager::Initialize() {
        if (m_initialized) {
            return true;
        }

        Log::Debug("Initializing CheatManager");

        g_mapManager = std::make_unique<MapManager>();
        g_globalVars = std::make_unique<globalvars>();

        m_initialized = true;
        return true;
    }

    void CheatManager::Shutdown() {
        if (!m_initialized) {
            return;
        }

        Log::Debug("Shutting down CheatManager");

        if (g_mapManager) {
            g_mapManager->UnloadMap();
            g_mapManager.reset();
        }

        if (g_globalVars) {  
            g_globalVars.reset();
        }

        m_initialized = false;
    }

    void CheatManager::Update() {
        if (!m_initialized) {
            return;
        }

        MenuManager::Update();

        if (!Utils::IsGameWindowActive() && !MenuConfig::ShowMenu) {
            return;
        }

        UpdateGameState();
        ProcessEntities();
        RenderFeatures();

        Utils::LimitFrameRate(MenuConfig::RenderFPS);
    }

    void CheatManager::UpdateGameState() {
        // Update matrix
        if (!memoryManager.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix, 64)) {
            return;
        }

        if (g_globalVars) {
            g_globalVars->UpdateGlobalvars();
        }

        // Update EntityList Entry
        gGame.UpdateEntityListEntry();
    }

    void CheatManager::ProcessEntities() {
        DWORD64 localControllerAddress = 0;
        DWORD64 localPawnAddress = 0;

        if (!memoryManager.ReadMemory(gGame.GetLocalControllerAddress(), localControllerAddress) ||
            !memoryManager.ReadMemory(gGame.GetLocalPawnAddress(), localPawnAddress)) {
            return;
        }

        // Update local entity
        CEntity localEntity;
        localEntity.UpdateClientData();

        if (!localEntity.UpdateControllerBatch(localControllerAddress) ||
            (!localEntity.UpdatePawnBatch(localPawnAddress) && !MenuConfig::WorkInSpec)) {
            return;
        }

        // Handle map loading
        if (!Utils::IsInServer()) {
            g_mapManager->UnloadMap();
            return;
        }

        std::string currentMap = Utils::GetCurrentMapName();
        if (!currentMap.empty()) {
            g_mapManager->LoadMap(currentMap.c_str());
        }

        // Process entities
        auto processedEntities = EntityProcessor::ProcessEntities(
            localEntity, *g_mapManager, m_localPlayerControllerIndex);

        // Collect aim positions
        std::vector<Vec3> aimPositions;
        float maxAimDistance = 100000.0f;

        // Setup radar
        Base_Radar gameRadar;
        if ((RadarCFG::ShowRadar && localEntity.Controller.TeamID != 0) ||
            (RadarCFG::ShowRadar && MenuConfig::ShowMenu)) {
            RadarManager::Initialize(gameRadar);
        }

        TriggerBot::g_HasValidTarget = false;

        // Process each entity result
        for (const auto& result : processedEntities) {
            // Add to radar
            if (RadarCFG::ShowRadar && localEntity.Controller.TeamID != 0) {
                gameRadar.AddPoint(
                    localEntity.Pawn.Pos, localEntity.Pawn.ViewAngle.y,
                    result.entity.Pawn.Pos, ImColor(237, 85, 106, 200),
                    RadarCFG::RadarType, result.entity.Pawn.ViewAngle.y);
            }

            // Handle spectator list
            SpecList::GetSpectatorList(result.entity, localEntity);

            // Handle TriggerBot
            TriggerBot::CheckForValidHitbox(localEntity, result.entity, result.isVisible);

            // Collect aim positions
            if (result.isValidForAiming && result.distanceToSight < maxAimDistance) {
                maxAimDistance = result.distanceToSight;
                aimPositions.push_back(result.aimPosition);
            }

            // Render ESP
            if (result.hasESPData) {
                RenderManager::RenderEntityESP(localEntity, result, m_localPlayerControllerIndex);
            }
        }

        // Update feature managers
        VisualManager::Update(localEntity);
        RadarManager::Update(gameRadar, localEntity);
        AimManager::Update(localEntity, aimPositions);
        MiscManager::Update(localEntity, m_previousTotalHits);

        TriggerBot::Run(localEntity);
    }

    void CheatManager::RenderFeatures() {
        if (MenuConfig::ShowMenu) {
            GUI::DrawGui();
        }
        GUI::InitHitboxList();
    }

    //=============================================================================
    // MapManager 
    //=============================================================================

    MapManager::MapManager()
        : m_mapLoaded(false)
        , m_mapLoadAttempted(false) {
    }

    MapManager::~MapManager() {
        UnloadMap();
    }

    bool MapManager::LoadMap(const char* mapName) {
        if (!mapName) {
            Log::Debug("LoadMap failed: mapName is NULL");
            return false;
        }

        std::string mapNameStr = ExtractMapName(mapName);

        if (m_mapLoadAttempted && m_currentMapName == mapNameStr) {
            return m_mapLoaded;
        }

        Log::Debug("Attempting to load map: " + mapNameStr);

        UnloadMap();

        std::string foundPath;
        if (FindMapFile(mapNameStr, foundPath)) {
            m_mapLoader.load_map(foundPath.c_str());
            m_currentMapName = mapNameStr;
            m_mapLoaded = true;
            m_mapLoadAttempted = true;
            Log::Debug("Successfully loaded tri file for: " + mapNameStr);
            return true;
        }
        else {
            m_mapLoaded = false;
            m_mapLoadAttempted = true;
            m_currentMapName = mapNameStr;
            Log::Debug("No tri file found for: " + mapNameStr + " - using mSpottedMask fallback");
            return false;
        }
    }

    void MapManager::UnloadMap() {
        if (m_mapLoaded) {
            m_mapLoader.unload();
            m_mapLoaded = false;
        }
        m_currentMapName.clear();
        m_mapLoadAttempted = false;
    }

    bool MapManager::IsVisible(const Vec3& source, const Vec3& destination) const {
        if (!m_mapLoaded) {
            return false;
        }
        return m_mapLoader.is_visible(*(Vector*)&source, *(Vector*)&destination);
    }

    std::string MapManager::ExtractMapName(const std::string& fullPath) const {
        std::string result = fullPath;

        // Remove path
        size_t lastSlash = result.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            result = result.substr(lastSlash + 1);
        }

        // Remove extension
        size_t lastDot = result.find_last_of('.');
        if (lastDot != std::string::npos) {
            result = result.substr(0, lastDot);
        }

        return result;
    }

    std::string MapManager::ExtractCoreName(const std::string& mapName) const {
        std::string coreName = mapName;
        std::transform(coreName.begin(), coreName.end(), coreName.begin(), ::tolower);

        if (coreName.substr(0, 3) == "de_") {
            coreName = coreName.substr(3);
        }

        return coreName;
    }

    bool MapManager::FindMapFile(const std::string& mapName, std::string& foundPath) const {
        std::string inputCoreName = ExtractCoreName(mapName);

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA("maps\\*.tri", &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            Log::Debug("Could not open maps directory or no .tri files found");
            return false;
        }

        bool found = false;
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }

            const char* fileName = findData.cFileName;
            if (!fileName || fileName[0] == '\0') {
                continue;
            }

            std::string fileNameStr = fileName;
            size_t dotPos = fileNameStr.find_last_of('.');
            std::string fileNameWithoutExt = (dotPos != std::string::npos) ?
                fileNameStr.substr(0, dotPos) : fileNameStr;

            std::string fileCoreName = ExtractCoreName(fileNameWithoutExt);

            // Check for partial matching
            bool inputContainsFile = inputCoreName.find(fileCoreName) != std::string::npos;
            bool fileContainsInput = fileCoreName.find(inputCoreName) != std::string::npos;

            if (inputContainsFile || fileContainsInput) {
                foundPath = "maps\\" + fileNameWithoutExt;
                found = true;
                break;
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
        return found;
    }

    //=============================================================================
    // MenuManager 
    //=============================================================================

    void MenuManager::Update() {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> difference = now - s_lastKeyPressTime;

        if (CheckHotkey() && difference.count() >= 1000) {
            MenuConfig::ShowMenu = !MenuConfig::ShowMenu;
            s_lastKeyPressTime = std::chrono::system_clock::now();
            s_keyWasPressed = false;
        }
    }

    void MenuManager::Render() {
        if (MenuConfig::ShowMenu) {
            GUI::DrawGui();
        }
        GUI::InitHitboxList();
    }

    bool MenuManager::CheckHotkey() {
        SHORT keyState = GetAsyncKeyState(MenuConfig::HotKey);
        if (keyState & 0x8000) {
            s_keyWasPressed = true;
            return false;
        }

        if (s_keyWasPressed && !(keyState & 0x8000)) {
            return true;
        }

        return false;
    }

    //=============================================================================
    // VisualManager 
    //=============================================================================

    void VisualManager::Update(const CEntity& localEntity) {
        // Draw FOV line
        Render::DrawFov(localEntity, LegitBotConfig::FovLineSize, LegitBotConfig::FovLineColor, 1);

        // Draw headshot line
        Render::HeadShootLine(localEntity, MiscCFG::HeadShootLineColor);

        // Air check
        Misc::AirCheck(localEntity);

        // Render crosshair
        RenderCrosshair(ImGui::GetBackgroundDrawList(), localEntity);
    }

    void VisualManager::RenderCrosshair(ImDrawList* drawList, const CEntity& localEntity) {
        if (!ShouldRenderCrosshair(localEntity)) {
            return;
        }

        ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
        Render::DrawCrossHair(drawList, screenCenter, MiscCFG::SniperCrosshairColor);
    }

    bool VisualManager::ShouldRenderCrosshair(const CEntity& localEntity) {
        if (!MiscCFG::SniperCrosshair || localEntity.Controller.TeamID == 0 || MenuConfig::ShowMenu) {
            return false;
        }

        bool isScoped = false;
        memoryManager.ReadMemory<bool>(localEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);

        if (isScoped) {
            return false;
        }

        std::string currentWeapon = TriggerBot::GetWeapon(localEntity);
        return TriggerBot::CheckScopeWeapon(currentWeapon);
    }

    //=============================================================================
    // RadarManager 
    //=============================================================================

    void RadarManager::Initialize(Base_Radar& radar) {
        ConfigureRadarSettings(radar);
    }

    void RadarManager::Update(Base_Radar& radar, const CEntity& localEntity) {
        if ((RadarCFG::ShowRadar && localEntity.Controller.TeamID != 0) ||
            (RadarCFG::ShowRadar && MenuConfig::ShowMenu)) {
            Render(radar, localEntity);
        }
    }

    void RadarManager::Render(Base_Radar& radar, const CEntity& localEntity) {
        radar.Render();
        MenuConfig::RadarWinPos = ImGui::GetWindowPos();
        ImGui::End();
    }

    void RadarManager::ConfigureRadarSettings(Base_Radar& radar) {
        // Setup radar window
        ImGui::SetNextWindowBgAlpha(RadarCFG::RadarBgAlpha);
        ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowSize({ RadarCFG::RadarRange * 2, RadarCFG::RadarRange * 2 });
        ImGui::SetWindowPos(MenuConfig::RadarWinPos, ImGuiCond_Once);

        if (MenuConfig::RadarWinChengePos) {
            ImGui::SetWindowPos("Radar", MenuConfig::RadarWinPos);
            MenuConfig::RadarWinChengePos = false;
        }

        // Apply default settings if not custom
        if (!RadarCFG::customRadar) {
            RadarCFG::ShowRadarCrossLine = false;
            RadarCFG::Proportion = 2700.0f;
            RadarCFG::RadarPointSizeProportion = 1.0f;
            RadarCFG::RadarRange = 125.0f;
            RadarCFG::RadarBgAlpha = 0.1f;
        }

        // Configure radar properties
        radar.SetDrawList(ImGui::GetWindowDrawList());
        radar.SetPos({
            ImGui::GetWindowPos().x + RadarCFG::RadarRange,
            ImGui::GetWindowPos().y + RadarCFG::RadarRange
            });
        radar.SetProportion(RadarCFG::Proportion);
        radar.SetRange(RadarCFG::RadarRange);
        radar.SetSize(RadarCFG::RadarRange * 2);
        radar.SetCrossColor(RadarCFG::RadarCrossLineColor);

        radar.ArcArrowSize *= RadarCFG::RadarPointSizeProportion;
        radar.ArrowSize *= RadarCFG::RadarPointSizeProportion;
        radar.CircleSize *= RadarCFG::RadarPointSizeProportion;

        radar.ShowCrossLine = RadarCFG::ShowRadarCrossLine;
        radar.Opened = true;
    }

    //=============================================================================
    // AimManager 
    //=============================================================================

    DWORD AimManager::s_lastToggleTick = 0;

    void AimManager::Update(const CEntity& localEntity,std::vector<Vec3>& aimPositions) {
        if (!LegitBotConfig::AimBot) {
            ProcessRecoilControl(localEntity);
            return;
        }

        Render::DrawFovCircle(localEntity);

        if (ShouldAim() && !aimPositions.empty()) {
            AimControl::AimBot(localEntity, localEntity.Pawn.CameraPos, aimPositions);
        }

        if (LegitBotConfig::AimToggleMode) {
            HandleAimToggle();
        }
    }

    bool AimManager::ShouldAim() {
        return LegitBotConfig::AimAlways || GetAsyncKeyState(AimControl::HotKey);
    }

    void AimManager::HandleAimToggle() {
        DWORD currentTick = GetTickCount64();
        if ((GetAsyncKeyState(AimControl::HotKey) & 0x8000) &&
            currentTick - s_lastToggleTick >= 200) {
            AimControl::switchToggle();
            s_lastToggleTick = currentTick;
        }
    }

    void AimManager::ProcessRecoilControl(const CEntity& localEntity) {
        RCS::RecoilControl(localEntity);
    }

    //=============================================================================
    // MiscManager 
    //=============================================================================

    void MiscManager::Update(CEntity& localEntity, int& previousTotalHits) {
        ProcessHitManager(localEntity, previousTotalHits);
        ProcessMovementFeatures(localEntity);
        ProcessUIFeatures(localEntity);
    }

    void MiscManager::ProcessHitManager(CEntity& localEntity, int& previousTotalHits) {
        Misc::HitManager(localEntity, previousTotalHits);
    }

    void MiscManager::ProcessMovementFeatures(CEntity& localEntity) {
        Misc::BunnyHop(localEntity);
    }

    void MiscManager::ProcessUIFeatures(CEntity& localEntity) {
        SpecList::SpectatorWindowList(localEntity);
        bmb::RenderWindow(localEntity.Controller.TeamID);
        Misc::Watermark(localEntity);
    }

    //=============================================================================
    // EntityProcessor 
    //=============================================================================

    std::vector<EntityProcessResult> EntityProcessor::ProcessEntities(
        const CEntity& localEntity,
        const MapManager& mapManager,
        int localPlayerControllerIndex) {

        Threading::ThreadSafeVector<EntityProcessResult> validEntities;

        auto entityData = CollectEntityAddresses(localEntity, localPlayerControllerIndex);  // Changed name

        // Parallel processing of entities
        Threading::parallelForIndex(0, entityData.size(), [&](size_t i) {
            const auto& [entityIndex, entity] = entityData[i];  // Changed to entity

            EntityProcessResult result = ProcessSingleEntity(
                entityIndex, entity, localEntity, mapManager, localPlayerControllerIndex);  // Changed parameter

            if (result.entityIndex != -1) {
                validEntities.push_back(result);
            }
            });

        return validEntities.get();
    }

    std::vector<std::pair<int, CEntity>> EntityProcessor::CollectEntityAddresses(
        const CEntity& localEntity,
        int& localPlayerControllerIndex) {

        std::vector<std::pair<int, CEntity>> entityData;
        entityData.reserve(64);

        int maxClients = 64;

        for (int entityIndex = 0; entityIndex < maxClients; ++entityIndex) {
            DWORD64 entityAddress = 0;
            if (!memoryManager.ReadMemory<DWORD64>(
                gGame.GetEntityListEntry() + (entityIndex + 1) * 0x78, entityAddress)) {
                continue;
            }

            if (entityAddress == localEntity.Controller.Address) {
                localPlayerControllerIndex = entityIndex;
                continue;
            }

            // Create and fully populate entity here
            CEntity entity;
            if (entity.UpdateControllerBatch(entityAddress) &&
                entity.UpdatePawnBatch(entity.Pawn.Address)) {
                entityData.emplace_back(entityIndex, entity);
            }
        }

        return entityData;
    }

    EntityProcessResult EntityProcessor::ProcessSingleEntity(
        int entityIndex,
        const CEntity& entity,
        const CEntity& localEntity,
        const MapManager& mapManager,
        int localPlayerControllerIndex) {

        EntityProcessResult result;

        // Copy the already fully-populated entity
        result.entity = entity;

        // Continue with validation
        if (!IsEntityValid(result.entity, localEntity)) {
            return result; // Invalid result
        }

        // Set basic info
        result.entityIndex = entityIndex;

        // Check visibility
        result.isVisible = CheckVisibility(
            result.entity, localEntity, mapManager, entityIndex, localPlayerControllerIndex);

        // Process aiming
        ProcessAiming(result.entity, result.isVisible, result);

        // Process ESP
        ProcessESP(result.entity, localEntity, result);

        return result;
    }

    bool EntityProcessor::IsEntityValid(CEntity& entity, const CEntity& localEntity) {
        if (MenuConfig::TeamCheck && entity.Controller.TeamID == localEntity.Controller.TeamID) {
            return false;
        }

        return entity.IsAlive() && entity.IsInScreen();
    }

    bool EntityProcessor::CheckVisibility(
        const CEntity& entity,
        const CEntity& localEntity,
        const MapManager& mapManager,
        int entityIndex,
        int localPlayerControllerIndex) {

        if (mapManager.IsMapLoaded()) {
            Vec3 source = localEntity.Pawn.CameraPos;
            Vec3 destination = entity.Pawn.CameraPos;
            return mapManager.IsVisible(source, destination);
        }
        else {
            // Fallback to spotted mask
            return (entity.Pawn.bSpottedByMask & (DWORD64(1) << localPlayerControllerIndex)) ||
                (localEntity.Pawn.bSpottedByMask & (DWORD64(1) << entityIndex));
        }
    }

    void EntityProcessor::ProcessAiming(
        const CEntity& entity,
        bool isVisible,
        EntityProcessResult& result) {

        if (AimControl::HitboxList.empty()) {
            return;
        }

        float minDistance = FLT_MAX;
        Vec3 bestAimPos{ 0, 0, 0 };

        ImVec2 screenCenter{ Gui.Window.Size.x / 2, Gui.Window.Size.y / 2 };

        for (size_t i = 0; i < AimControl::HitboxList.size(); ++i) {
            int hitboxID = AimControl::HitboxList[i];

            float distanceToSight = entity.GetBone().BonePosList[hitboxID].ScreenPos.DistanceTo(
                { screenCenter.x, screenCenter.y });

            if (distanceToSight < minDistance) {
                minDistance = distanceToSight;

                if (!LegitBotConfig::VisibleCheck || isVisible) {
                    Vec3 tempPos = entity.GetBone().BonePosList[hitboxID].Pos;

                    if (hitboxID == BONEINDEX::head) {
                        tempPos.z -= 1.0f;
                    }

                    bestAimPos = tempPos;
                    result.isValidForAiming = true;
                }
            }
        }

        result.distanceToSight = minDistance;
        result.aimPosition = bestAimPos;
    }

    void EntityProcessor::ProcessESP(
        const CEntity& entity,
        const CEntity& localEntity,
        EntityProcessResult& result) {

        if (!ESPConfig::ESPenabled) {
            return;
        }

        result.espRect = ESP::GetBoxRect(entity, ESPConfig::BoxType);
        result.distance = static_cast<int>(entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) / 100);
        result.hasESPData = true;
    }

    //=============================================================================
    // RenderManager 
    //=============================================================================

    void RenderManager::RenderEntityESP(
        const CEntity& localEntity,
        const EntityProcessResult& result,
        int localPlayerControllerIndex) {

        if (!ShouldRenderEntity(result)) {
            return;
        }

        // Main ESP rendering
        ESP::RenderPlayerESP(localEntity, result.entity, result.espRect,
            localPlayerControllerIndex, 0, result.isVisible);

        // Distance rendering
        Render::DrawDistance(localEntity, result.entity, result.espRect);

        // Additional bars
        RenderHealthBar(result);
        RenderAmmoBar(result);
        RenderArmorBar(result);
    }

    bool RenderManager::ShouldRenderEntity(const EntityProcessResult& result) {
        return result.hasESPData &&
            (MenuConfig::RenderDistance == 0 ||
                (result.distance <= MenuConfig::RenderDistance && MenuConfig::RenderDistance > 0));
    }

    void RenderManager::RenderHealthBar(const EntityProcessResult& result) {
        if (!ESPConfig::ShowHealthBar) {
            return;
        }

        ImVec2 healthBarPos = { result.espRect.x - 6.0f, result.espRect.y };
        ImVec2 healthBarSize = { 4, result.espRect.w };

        Render::DrawHealthBar(result.entity.Controller.Address, 100,
            result.entity.Pawn.Health, healthBarPos, healthBarSize);
    }

    void RenderManager::RenderAmmoBar(const EntityProcessResult& result) {
        if (!ESPConfig::AmmoBar || result.entity.Pawn.Ammo == -1) {
            return;
        }

        ImVec2 ammoBarPos = { result.espRect.x, result.espRect.y + result.espRect.w + 2 };
        ImVec2 ammoBarSize = { result.espRect.z, 4 };

        Render::DrawAmmoBar(result.entity.Controller.Address, result.entity.Pawn.MaxAmmo,
            result.entity.Pawn.Ammo, ammoBarPos, ammoBarSize);
    }

    void RenderManager::RenderArmorBar(const EntityProcessResult& result) {
        if (!ESPConfig::ArmorBar || result.entity.Pawn.Armor <= 0) {
            return;
        }

        bool hasHelmet = false;
        memoryManager.ReadMemory(result.entity.Controller.Address + Offset.PlayerController.HasHelmet, hasHelmet);

        ImVec2 armorBarPos;
        if (ESPConfig::ShowHealthBar) {
            armorBarPos = { result.espRect.x - 10.0f, result.espRect.y };
        }
        else {
            armorBarPos = { result.espRect.x - 6.0f, result.espRect.y };
        }

        ImVec2 armorBarSize = { 4.0f, result.espRect.w };

        Render::DrawArmorBar(result.entity.Controller.Address, 100, result.entity.Pawn.Armor,
            hasHelmet, armorBarPos, armorBarSize);
    }

    //=============================================================================
    // Utility Functions
    //=============================================================================

    namespace Utils {

        void LimitFrameRate(int targetFPS) {
            int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
            if (currentFPS > targetFPS) {
                int frameWait = static_cast<int>(std::round(1000000.0f / targetFPS));
                std::this_thread::sleep_for(std::chrono::microseconds(frameWait));
            }
        }

        bool IsGameWindowActive() {
            return Init::Client::isGameWindowActive();
        }

        bool IsInServer() {
            return g_globalVars && g_globalVars->UpdateGlobalvars() && g_globalVars->GetCurrentMap();
        }

        std::string GetCurrentMapName() {
            if (!g_globalVars || !g_globalVars->g_cCurrentMap) {
                return "";
            }

            char currentMap[256] = { 0 };
            if (!memoryManager.ReadMemory(reinterpret_cast<DWORD64>(g_globalVars->g_cCurrentMap),
                currentMap, sizeof(currentMap) - 1)) {
                return "";
            }

            currentMap[255] = '\0';
            return std::string(currentMap);
        }

    } // namespace Utils

    //=============================================================================
    // Main Interface Functions
    //=============================================================================

    void RunTri() {
        if (!g_cheatManager) {
            g_cheatManager = std::make_unique<CheatManager>();
            g_cheatManager->Initialize();
        }

        g_cheatManager->Update();
    }

    void Run() {
        // Legacy function - redirect to RunTri for now
        RunTri();
    }

    void RunMT() {
        // Multi-threaded version - redirect to RunTri for now
        RunTri();
    }

} // namespace Cheats

//=============================================================================
// FontManager 
//=============================================================================

void FontManager::Initialize() {
    // Currently not used
}

void FontManager::Shutdown() {
    normal15px = nullptr;
}
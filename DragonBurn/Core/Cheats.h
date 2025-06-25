#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Features/Aimbot.h"
#include "../Features/Radar.h"
#include "../Features/TriggerBot.h"
#include "../Features/Misc.h"
#include "../TriVisCheck/ray_trace.h"


// //Forward declarations
//class ImDrawList;
//struct ImVec2;
//struct ImVec4;

namespace Cheats {
    // Main execution functions
    void RunMT();
    void Run();
    void RunTri();

    // Core system components
    
    class CheatManager {
    public:
        CheatManager();
        ~CheatManager();

        bool Initialize();
        void Shutdown();
        void Update();

    private:
        void UpdateGameState();
        void ProcessEntities();
        void RenderFeatures();

        bool m_initialized;
        int m_previousTotalHits;
        int m_localPlayerControllerIndex;
    };

    // Map management
    class MapManager {
    public:
        MapManager();
        ~MapManager();

        bool LoadMap(const char* mapName);
        void UnloadMap();
        bool IsMapLoaded() const { return m_mapLoaded; }
        bool IsVisible(const Vec3& source, const Vec3& destination) const;

    private:
        std::string ExtractMapName(const std::string& fullPath) const;
        std::string ExtractCoreName(const std::string& mapName) const;
        bool FindMapFile(const std::string& mapName, std::string& foundPath) const;

        map_loader m_mapLoader;
        std::string m_currentMapName;
        bool m_mapLoaded;
        bool m_mapLoadAttempted;
    };

    // Entity processing result structure
    struct EntityProcessResult {
        int entityIndex;
        CEntity entity;
        bool isVisible;
        bool isValidForAiming;
        Vec3 aimPosition;
        float distanceToSight;
        ImVec4 espRect;
        int distance;
        bool hasESPData;

        EntityProcessResult()
            : entityIndex(-1), isVisible(false), isValidForAiming(false)
            , distanceToSight(FLT_MAX), distance(0), hasESPData(false) {
        }
    };

    // Feature managers
    class MenuManager {
    public:
        static void Update();
        static void Render();

    private:
        static bool CheckHotkey();
    };

    class VisualManager {
    public:
        static void Update(const CEntity& localEntity);
        static void RenderCrosshair(ImDrawList* drawList, const CEntity& localEntity);

    private:
        static bool ShouldRenderCrosshair(const CEntity& localEntity);
    };

    class RadarManager {
    public:
        static void Initialize(Base_Radar& radar);
        static void Update(Base_Radar& radar, const CEntity& localEntity);
        static void Render(Base_Radar& radar, const CEntity& localEntity);

    private:
        static void ConfigureRadarSettings(Base_Radar& radar);
    };

    class AimManager {
    public:
        static void Update(const CEntity& localEntity,std::vector<Vec3>& aimPositions);

    private:
        static bool ShouldAim();
        static void HandleAimToggle();
        static void ProcessRecoilControl(const CEntity& localEntity);

        static DWORD s_lastToggleTick;
    };

    class MiscManager {
    public:
        static void Update(CEntity& localEntity, int& previousTotalHits);

    private:
        static void ProcessHitManager(CEntity& localEntity, int& previousTotalHits);
        static void ProcessMovementFeatures(CEntity& localEntity);
        static void ProcessUIFeatures(CEntity& localEntity);
    };

    // Entity processing
    class EntityProcessor {
    public:
        static std::vector<EntityProcessResult> ProcessEntities(
            const CEntity& localEntity,
            const MapManager& mapManager,
            int localPlayerControllerIndex
        );

    private:
        static std::vector<std::pair<int, CEntity>> CollectEntityAddresses(
            const CEntity& localEntity,
            int& localPlayerControllerIndex
        );

        static EntityProcessResult ProcessSingleEntity(
            int entityIndex,
            const CEntity& entityAddress,
            const CEntity& localEntity,
            const MapManager& mapManager,
            int localPlayerControllerIndex
        );

        static bool IsEntityValid(CEntity& entity, const CEntity& localEntity);
        static bool CheckVisibility(
            const CEntity& entity,
            const CEntity& localEntity,
            const MapManager& mapManager,
            int entityIndex,
            int localPlayerControllerIndex
        );

        static void ProcessAiming(
            const CEntity& entity,
            bool isVisible,
            EntityProcessResult& result
        );

        static void ProcessESP(
            const CEntity& entity,
            const CEntity& localEntity,
            EntityProcessResult& result
        );
    };

    // Rendering
    class RenderManager {
    public:
        static void RenderEntityESP(
            const CEntity& localEntity,
            const EntityProcessResult& result,
            int localPlayerControllerIndex
        );

    private:
        static void RenderHealthBar(const EntityProcessResult& result);
        static void RenderAmmoBar(const EntityProcessResult& result);
        static void RenderArmorBar(const EntityProcessResult& result);
        static bool ShouldRenderEntity(const EntityProcessResult& result);
    };

    // Utility functions
    namespace Utils {
        void LimitFrameRate(int targetFPS);
        bool IsGameWindowActive();
        bool IsInServer();
        std::string GetCurrentMapName();
    }

    // Global state
    extern std::unique_ptr<CheatManager> g_cheatManager;
    extern std::unique_ptr<MapManager> g_mapManager;
    extern std::unique_ptr<globalvars> g_globalVars;
}

// Font management
struct FontManager {
    ImFont* normal15px = nullptr;

    void Initialize();
    void Shutdown();
};
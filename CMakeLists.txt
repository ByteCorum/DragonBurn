#______                            ______                  
#|  _  \                           | ___ \                 
#| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
#| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
#| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
#|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
#                  __/ |                                   
#                 |___/                                    
#
#https://discord.gg/5WcvdzFybD
#https://github.com/ByteCorum/DragonBurn

cmake_minimum_required(VERSION 3.20)
project(DragonBurn VERSION 3.3.8.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check
if(NOT WIN32)
    message(FATAL_ERROR "This project is designed for Windows only")
endif()

# Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/built_dbg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/built_dbg/cache")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built/cache")

# Include directories
include_directories(
    Libs/json
    OS-ImGui/imgui
    OS-ImGui/stb
    ${CMAKE_SOURCE_DIR}
)

# Source files
set(SOURCES
    # Main
    main.cpp
    
    # Config
    Config/ConfigMenu.cpp
    Config/ConfigSaver.cpp
    
    # Core
    Core/Cheats.cpp
    Core/GlobalVars.cpp
    Core/MemoryMgr.cpp
    
    # Features
    Features/Aimbot.cpp
    Features/Misc.cpp
    Features/Radar.cpp
    Features/RCS.cpp
    Features/TriggerBot.cpp
    
    # Game
    Game/Bone.cpp
    Game/Entity.cpp
    Game/Game.cpp
    
    # Helpers
    Helpers/Mouse.cpp
    Helpers/UIAccess.cpp
    
    # Offsets
    Offsets/Offsets.cpp
    
    # ImGui
    OS-ImGui/imgui/imgui.cpp
    OS-ImGui/imgui/imgui_demo.cpp
    OS-ImGui/imgui/imgui_draw.cpp
    OS-ImGui/imgui/imgui_impl_dx11.cpp
    OS-ImGui/imgui/imgui_impl_win32.cpp
    OS-ImGui/imgui/imgui_tables.cpp
    OS-ImGui/imgui/imgui_widgets.cpp
    
    # OS-ImGui
    OS-ImGui/OS-ImGui.cpp
    OS-ImGui/OS-ImGui_Base.cpp
    OS-ImGui/OS-ImGui_External.cpp
)

# Header files
set(HEADERS
    # Config
    Config/ConfigMenu.h
    Config/ConfigSaver.h
    
    # Core
    Core/Config.h
    Core/Cheats.h
    Core/Globals.h
    Core/GlobalVars.h
    Core/GUI.h
    Core/Init.h
    Core/MemoryMgr.h
    Core/Render.h
    
    # Features
    Features/Aimbot.h
    Features/BombTimer.h
    Features/ESP.h
    Features/Misc.h
    Features/Radar.h
    Features/RCS.h
    Features/SoundESP.h
    Features/SpectatorList.h
    Features/TriggerBot.h
    
    # Game
    Game/Bone.h
    Game/Entity.h
    Game/Game.h
    Game/View.h
    
    # Helpers
    Helpers/Format.h
    Helpers/GetWeaponIcon.h
    Helpers/KeyManager.h
    Helpers/Logger.h
    Helpers/Mouse.h
    Helpers/UIAccess.h
    Helpers/WebApi.h
    
    # Offsets
    Offsets/Offsets.h
    
    # ImGui
    OS-ImGui/imgui/imconfig.h
    OS-ImGui/imgui/imgui.h
    OS-ImGui/imgui/imgui_impl_dx11.h
    OS-ImGui/imgui/imgui_impl_win32.h
    OS-ImGui/imgui/imgui_internal.h
    OS-ImGui/imgui/imstb_rectpack.h
    OS-ImGui/imgui/imstb_textedit.h
    OS-ImGui/imgui/imstb_truetype.h
    
    # OS-ImGui
    OS-ImGui/OS-ImGui.h
    OS-ImGui/OS-ImGui_Base.h
    OS-ImGui/OS-ImGui_Exception.hpp
    OS-ImGui/OS-ImGui_External.h
    OS-ImGui/OS-ImGui_Struct.h
    
    # STB
    OS-ImGui/stb/stb_image.h
    
    # Resources
    Resources/resource.h
    Resources/Font.hpp
    Resources/Images.hpp
    Resources/Language.hpp
    Resources/Sounds.hpp
    Resources/WeaponIcon.hpp
)

# Resource files
set(RESOURCE_FILES
    Resources/Resource.rc
    Resources/icon.ico
)

# Create the executable
add_executable(DragonBurn WIN32 ${SOURCES} ${HEADERS} ${RESOURCE_FILES})

# Set target name
set_target_properties(DragonBurn PROPERTIES OUTPUT_NAME "DragonBurn")

# Compiler-specific options
if(MSVC)
    # Warning level
    target_compile_options(DragonBurn PRIVATE /W3)
    
    # Disable SDL checks
    target_compile_options(DragonBurn PRIVATE /sdl-)
    
    # Debug configuration
    target_compile_definitions(DragonBurn PRIVATE
        $<$<CONFIG:Debug>:DBDEBUG>
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        _CONSOLE
        UNICODE
        _UNICODE
    )
    
    # Runtime library
    set_property(TARGET DragonBurn PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    
    # Linker options
    set_target_properties(DragonBurn PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE /DEBUG"
        LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF"
    )
    
    # UAC elevation (requires administrator) - still needed for some operations
    set_target_properties(DragonBurn PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(DragonBurn PRIVATE
        d3d11
        dxgi
        user32
        kernel32
        gdi32
        winspool
        comdlg32
        advapi32
        shell32
        ole32
        oleaut32
        uuid
        odbc32
        odbccp32
    )
endif()

# Group source files for IDE
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Resource Files" FILES ${RESOURCE_FILES})

# Create filters for better organization in IDEs
source_group("Source Files\\Config" FILES 
    Config/ConfigMenu.cpp
    Config/ConfigSaver.cpp
)

source_group("Source Files\\Core" FILES
    Core/Cheats.cpp
    Core/GlobalVars.cpp
    Core/MemoryMgr.cpp
)

source_group("Source Files\\Features" FILES
    Features/Aimbot.cpp
    Features/Misc.cpp
    Features/Radar.cpp
    Features/RCS.cpp
    Features/TriggerBot.cpp
)

source_group("Source Files\\Game" FILES
    Game/Bone.cpp
    Game/Entity.cpp
    Game/Game.cpp
)

source_group("Source Files\\Helpers" FILES
    Helpers/Mouse.cpp
    Helpers/UIAccess.cpp
)

source_group("Source Files\\ImGui" FILES
    OS-ImGui/imgui/imgui.cpp
    OS-ImGui/imgui/imgui_demo.cpp
    OS-ImGui/imgui/imgui_draw.cpp
    OS-ImGui/imgui/imgui_impl_dx11.cpp
    OS-ImGui/imgui/imgui_impl_win32.cpp
    OS-ImGui/imgui/imgui_tables.cpp
    OS-ImGui/imgui/imgui_widgets.cpp
)

source_group("Source Files\\OS-ImGui" FILES
    OS-ImGui/OS-ImGui.cpp
    OS-ImGui/OS-ImGui_Base.cpp
    OS-ImGui/OS-ImGui_External.cpp
)

# Header file grouping
source_group("Header Files\\Config" FILES
    Config/ConfigMenu.h
    Config/ConfigSaver.h
)

source_group("Header Files\\Core" FILES
    Core/Config.h
    Core/Cheats.h
    Core/Globals.h
    Core/GlobalVars.h
    Core/GUI.h
    Core/Init.h
    Core/MemoryMgr.h
    Core/Render.h
)

source_group("Header Files\\Features" FILES
    Features/Aimbot.h
    Features/BombTimer.h
    Features/ESP.h
    Features/Misc.h
    Features/Radar.h
    Features/RCS.h
    Features/SoundESP.h
    Features/SpectatorList.h
    Features/TriggerBot.h
)

source_group("Header Files\\Game" FILES
    Game/Bone.h
    Game/Entity.h
    Game/Game.h
    Game/View.h
)

source_group("Header Files\\Helpers" FILES
    Helpers/Format.h
    Helpers/GetWeaponIcon.h
    Helpers/KeyManager.h
    Helpers/Logger.h
    Helpers/Mouse.h
    Helpers/UIAccess.h
    Helpers/WebApi.h
)

source_group("Header Files\\ImGui" FILES
    OS-ImGui/imgui/imconfig.h
    OS-ImGui/imgui/imgui.h
    OS-ImGui/imgui/imgui_impl_dx11.h
    OS-ImGui/imgui/imgui_impl_win32.h
    OS-ImGui/imgui/imgui_internal.h
    OS-ImGui/imgui/imstb_rectpack.h
    OS-ImGui/imgui/imstb_textedit.h
    OS-ImGui/imgui/imstb_truetype.h
)

source_group("Header Files\\OS-ImGui" FILES
    OS-ImGui/OS-ImGui.h
    OS-ImGui/OS-ImGui_Base.h
    OS-ImGui/OS-ImGui_Exception.hpp
    OS-ImGui/OS-ImGui_External.h
    OS-ImGui/OS-ImGui_Struct.h
)

source_group("Header Files\\Resources" FILES
    Resources/resource.h
    Resources/Font.hpp
    Resources/Images.hpp
    Resources/Language.hpp
    Resources/Sounds.hpp
    Resources/WeaponIcon.hpp
)

# Set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DragonBurn)

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_minimum_required(VERSION 3.20)
project(DragonBurnSolution VERSION 3.3.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "This project is designed for Windows only")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/built_dbg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/built_dbg/cache")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built/cache")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    add_compile_options(/W3 /sdl-)
    add_compile_definitions(UNICODE _UNICODE)
    
    add_compile_options($<$<CONFIG:Debug>:/W3 /sdl->)
    add_compile_definitions($<$<CONFIG:Debug>:DBDEBUG _DEBUG>)
    
    add_compile_options($<$<CONFIG:Release>:/W3 /sdl- /O2>)
    add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
    
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF")
    
    set(UAC_MANIFEST "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()

# DragonBurn Kernel Project

set(KERNEL_SOURCES
    src/dragonburn-kernel/intel-driver.cpp
    src/dragonburn-kernel/main.cpp
    src/dragonburn-kernel/kdmapper.cpp
    src/dragonburn-kernel/portable-executable.cpp
    src/dragonburn-kernel/service.cpp
    src/dragonburn-kernel/utils.cpp
)

set(KERNEL_HEADERS
    src/dragonburn-kernel/include/cfg.h
    src/dragonburn-kernel/include/intel-driver.h
    src/dragonburn-kernel/include/intel-driver-resource.h
    src/dragonburn-kernel/include/json.hpp
    src/dragonburn-kernel/include/kdmapper.h
    src/dragonburn-kernel/include/logger.h
    src/dragonburn-kernel/include/nt.h
    src/dragonburn-kernel/include/portable-executable.h
    src/dragonburn-kernel/include/service.h
    src/dragonburn-kernel/include/utils.h
    src/dragonburn-kernel/include/web-api.h
    src/dragonburn-kernel/resource.h
)

set(KERNEL_RESOURCES
    src/dragonburn-kernel/resource.rc
    src/dragonburn-kernel/icon.ico
)

add_executable(DragonBurn-kernel WIN32 ${KERNEL_SOURCES} ${KERNEL_HEADERS} ${KERNEL_RESOURCES})

target_include_directories(DragonBurn-kernel PRIVATE src/dragonburn-kernel/include)

target_compile_options(DragonBurn-kernel PRIVATE
    $<$<CONFIG:Debug>:/W4 /Od>
    $<$<CONFIG:Release>:/W3 /O2 /Oi /GL /MP>
    /sdl
)

target_compile_definitions(DragonBurn-kernel PRIVATE
    $<$<CONFIG:Debug>:DBDEBUG _DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    UNICODE
    _UNICODE
)

set_target_properties(DragonBurn-kernel PROPERTIES OUTPUT_NAME "DragonBurn-kernel")

set_target_properties(DragonBurn-kernel PROPERTIES
    LINK_FLAGS_DEBUG "/DEBUG"
    LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG"
    LINK_FLAGS "${UAC_MANIFEST}"
)

target_link_libraries(DragonBurn-kernel PRIVATE
    version
    kernel32
    user32
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

set_target_properties(DragonBurn-kernel PROPERTIES FOLDER "Kernel")

# DragonBurn User-Mode Project

set(USERMODE_SOURCES
    # Main
    src/dragonburn-usermode/main.cpp
    
    # Config
    src/dragonburn-usermode/config/config-menu.cpp
    src/dragonburn-usermode/config/config-saver.cpp
    
    # Core
    src/dragonburn-usermode/core/cheats.cpp
    src/dragonburn-usermode/core/global-vars.cpp
    src/dragonburn-usermode/core/memory.cpp
    
    # Features
    src/dragonburn-usermode/features/aimbot.cpp
    src/dragonburn-usermode/features/misc.cpp
    src/dragonburn-usermode/features/radar.cpp
    src/dragonburn-usermode/features/rcs.cpp
    src/dragonburn-usermode/features/triggerbot.cpp
    
    # Game
    src/dragonburn-usermode/game/bone.cpp
    src/dragonburn-usermode/game/entity.cpp
    src/dragonburn-usermode/game/game.cpp
    
    # Helpers
    src/dragonburn-usermode/helpers/ui-access.cpp
    src/dragonburn-usermode/helpers/storage.cpp
    
    # Offsets
    src/dragonburn-usermode/offsets/offsets.cpp
    
    # ImGui (OS-ImGui)
    src/dragonburn-usermode/imgui-extencion/imgui/imgui.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_demo.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_draw.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_dx11.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_win32.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_tables.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_widgets.cpp
    
    # OS-ImGui (Extended)
    src/dragonburn-usermode/imgui-extencion/ex-imgui.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-base.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-external.cpp
)

set(USERMODE_HEADERS
    # Config
    src/dragonburn-usermode/config/config-menu.h
    src/dragonburn-usermode/config/config-saver.h
    
    # Core
    src/dragonburn-usermode/core/config.h
    src/dragonburn-usermode/core/cheats.h
    src/dragonburn-usermode/core/globals.h
    src/dragonburn-usermode/core/global-vars.h
    src/dragonburn-usermode/core/gui.h
    src/dragonburn-usermode/core/init.h
    src/dragonburn-usermode/core/memory.h
    src/dragonburn-usermode/core/render.h
    
    # Features
    src/dragonburn-usermode/features/aimbot.h
    src/dragonburn-usermode/features/bomb-timer.h
    src/dragonburn-usermode/features/esp.h
    src/dragonburn-usermode/features/misc.h
    src/dragonburn-usermode/features/radar.h
    src/dragonburn-usermode/features/rcs.h
    src/dragonburn-usermode/features/sound-esp.h
    src/dragonburn-usermode/features/spectator-list.h
    src/dragonburn-usermode/features/triggerbot.h
    
    # Game
    src/dragonburn-usermode/game/bone.h
    src/dragonburn-usermode/game/entity.h
    src/dragonburn-usermode/game/game.h
    src/dragonburn-usermode/game/view.h
    
    # Helpers
    src/dragonburn-usermode/helpers/key-manager.h
    src/dragonburn-usermode/helpers/logger.h
    src/dragonburn-usermode/helpers/network.h
    src/dragonburn-usermode/helpers/storage.h
    src/dragonburn-usermode/helpers/ui-access.h
    src/dragonburn-usermode/helpers/weapon-icons.h
    
    # Offsets
    src/dragonburn-usermode/offsets/offsets.h
    
    # ImGui (OS-ImGui)
    src/dragonburn-usermode/imgui-extencion/imgui/imconfig.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_dx11.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_win32.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_internal.h
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_rectpack.h
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_textedit.h
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_truetype.h
    
    # OS-ImGui (Extended)
    src/dragonburn-usermode/imgui-extencion/ex-imgui.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-base.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-exception.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-external.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-struct.h
    src/dragonburn-usermode/imgui-extencion/stb/stb_image.h
    
    # Resources
    src/dragonburn-usermode/resources/resource.h
    src/dragonburn-usermode/resources/font.h
    src/dragonburn-usermode/resources/images.h
    src/dragonburn-usermode/resources/language.h
    src/dragonburn-usermode/resources/sounds.h
    src/dragonburn-usermode/resources/weaponicon.h
)

set(USERMODE_RESOURCES
    src/dragonburn-usermode/resources/resource.rc
    src/dragonburn-usermode/resources/icon.ico
)

add_executable(DragonBurn-usermode WIN32 ${USERMODE_SOURCES} ${USERMODE_HEADERS} ${USERMODE_RESOURCES})

target_include_directories(DragonBurn-usermode PRIVATE
    src/dragonburn-usermode/dependencies/json
    src/dragonburn-usermode/imgui-extencion
    src/dragonburn-usermode
)

target_compile_definitions(DragonBurn-usermode PRIVATE
    $<$<CONFIG:Debug>:DBDEBUG _DEBUG _CONSOLE>
    $<$<CONFIG:Release>:NDEBUG _CONSOLE>
    UNICODE
    _UNICODE
)

set_target_properties(DragonBurn-usermode PROPERTIES OUTPUT_NAME "DragonBurn-usermode")

set_target_properties(DragonBurn-usermode PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE /DEBUG"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF"
    LINK_FLAGS "${UAC_MANIFEST}"
)

target_link_libraries(DragonBurn-usermode PRIVATE
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

set_target_properties(DragonBurn-usermode PROPERTIES FOLDER "Main")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DragonBurn-usermode)

# Build Options

option(BUILD_USERMODE "Build DragonBurn user-mode application" ON)

option(BUILD_KERNEL "Build DragonBurn kernel mapper" ON)

if(NOT BUILD_USERMODE)
    set_target_properties(DragonBurn-usermode PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(DragonBurn-usermode PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif()

if(NOT BUILD_KERNEL)
    set_target_properties(DragonBurn-kernel PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(DragonBurn-kernel PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif()

# =============================================================================

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/built)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/built/cache)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/built_dbg)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/built_dbg/cache)

if(BUILD_KERNEL)
    add_custom_command(TARGET DragonBurn-usermode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/DragonBurn-kernel.exe
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/DragonBurn-kernel.exe
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/DragonBurn-kernel.exe
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/DragonBurn-kernel.exe
        COMMENT "Copying kernel executable for usermode to find"
    )
endif()

source_group("DragonBurn\\Config" FILES
    src/dragonburn-usermode/config/config-menu.cpp
    src/dragonburn-usermode/config/config-menu.h
    src/dragonburn-usermode/config/config-saver.cpp
    src/dragonburn-usermode/config/config-saver.h
)

source_group("DragonBurn\\Core" FILES
    src/dragonburn-usermode/core/cheats.cpp
    src/dragonburn-usermode/core/cheats.h
    src/dragonburn-usermode/core/config.h
    src/dragonburn-usermode/core/globals.h
    src/dragonburn-usermode/core/global-vars.cpp
    src/dragonburn-usermode/core/global-vars.h
    src/dragonburn-usermode/core/gui.h
    src/dragonburn-usermode/core/init.h
    src/dragonburn-usermode/core/memory.cpp
    src/dragonburn-usermode/core/memory.h
    src/dragonburn-usermode/core/render.h
)

source_group("DragonBurn\\Features" FILES
    src/dragonburn-usermode/features/aimbot.cpp
    src/dragonburn-usermode/features/aimbot.h
    src/dragonburn-usermode/features/bomb-timer.h
    src/dragonburn-usermode/features/esp.h
    src/dragonburn-usermode/features/misc.cpp
    src/dragonburn-usermode/features/misc.h
    src/dragonburn-usermode/features/radar.cpp
    src/dragonburn-usermode/features/radar.h
    src/dragonburn-usermode/features/rcs.cpp
    src/dragonburn-usermode/features/rcs.h
    src/dragonburn-usermode/features/sound-esp.h
    src/dragonburn-usermode/features/spectator-list.h
    src/dragonburn-usermode/features/triggerbot.cpp
    src/dragonburn-usermode/features/triggerbot.h
)

source_group("DragonBurn\\Game" FILES
    src/dragonburn-usermode/game/bone.cpp
    src/dragonburn-usermode/game/bone.h
    src/dragonburn-usermode/game/entity.cpp
    src/dragonburn-usermode/game/entity.h
    src/dragonburn-usermode/game/game.cpp
    src/dragonburn-usermode/game/game.h
    src/dragonburn-usermode/game/view.h
)

source_group("DragonBurn\\Helpers" FILES
    src/dragonburn-usermode/helpers/key-manager.h
    src/dragonburn-usermode/helpers/logger.h
    src/dragonburn-usermode/helpers/network.h
    src/dragonburn-usermode/helpers/storage.cpp
    src/dragonburn-usermode/helpers/storage.h
    src/dragonburn-usermode/helpers/ui-access.cpp
    src/dragonburn-usermode/helpers/ui-access.h
    src/dragonburn-usermode/helpers/weapon-icons.h
)

source_group("DragonBurn\\ImGui" FILES
    src/dragonburn-usermode/imgui-extencion/imgui/imconfig.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_demo.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_draw.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_dx11.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_dx11.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_win32.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_win32.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_internal.h
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_tables.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_widgets.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_rectpack.h
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_textedit.h
    src/dragonburn-usermode/imgui-extencion/imgui/imstb_truetype.h
)

source_group("DragonBurn\\OS-ImGui" FILES
    src/dragonburn-usermode/imgui-extencion/ex-imgui.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-base.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-base.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-exception.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-external.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-external.h
    src/dragonburn-usermode/imgui-extencion/ex-imgui-struct.h
    src/dragonburn-usermode/imgui-extencion/stb/stb_image.h
)

source_group("DragonBurn\\Offsets" FILES
    src/dragonburn-usermode/offsets/offsets.cpp
    src/dragonburn-usermode/offsets/offsets.h
)

source_group("DragonBurn\\Resources" FILES
    src/dragonburn-usermode/resources/resource.rc
    src/dragonburn-usermode/resources/icon.ico
    src/dragonburn-usermode/resources/resource.h
    src/dragonburn-usermode/resources/font.h
    src/dragonburn-usermode/resources/images.h
    src/dragonburn-usermode/resources/language.h
    src/dragonburn-usermode/resources/sounds.h
    src/dragonburn-usermode/resources/weaponicon.h
    src/dragonburn-usermode/main.cpp
)

source_group("Kernel\\Sources" FILES
    src/dragonburn-kernel/intel-driver.cpp
    src/dragonburn-kernel/kdmapper.cpp
    src/dragonburn-kernel/main.cpp
    src/dragonburn-kernel/portable-executable.cpp
    src/dragonburn-kernel/service.cpp
    src/dragonburn-kernel/utils.cpp
)

source_group("Kernel\\Headers" FILES
    src/dragonburn-kernel/include/cfg.h
    src/dragonburn-kernel/include/intel-driver.h
    src/dragonburn-kernel/include/intel-driver-resource.h
    src/dragonburn-kernel/include/json.hpp
    src/dragonburn-kernel/include/kdmapper.h
    src/dragonburn-kernel/include/logger.h
    src/dragonburn-kernel/include/nt.h
    src/dragonburn-kernel/include/portable-executable.h
    src/dragonburn-kernel/include/service.h
    src/dragonburn-kernel/include/utils.h
    src/dragonburn-kernel/include/web-api.h
    src/dragonburn-kernel/resource.h
)

source_group("Kernel\\Resources" FILES
    src/dragonburn-kernel/resource.rc
    src/dragonburn-kernel/icon.ico
)

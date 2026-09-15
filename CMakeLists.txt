cmake_minimum_required(VERSION 3.20)

project(DragonBurn LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "DragonBurn can only be built on Windows.")
endif()

if(NOT MSVC)
    message(FATAL_ERROR "The MSVC toolchain is required.")
endif()

# Mirror the original MSBuild preprocessor environment: CMake injects /DWIN32 and /D_WINDOWS into the base flags, but the .vcxproj build defines neither.
# The ex-imgui headers conditionally compile their declarations on the subsystem macros, so both must be removed to match the working .sln build.
string(REGEX REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# The mapper .vcxproj Release configuration defines no NDEBUG; strip CMake's default /DNDEBUG to match. The usermode target re-declares NDEBUG explicitly.
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

option(DRAGONBURN_BUILD_MAPPER   "Build the DragonBurn-mapper executable"   ON)
option(DRAGONBURN_BUILD_USERMODE "Build the DragonBurn-usermode executable" ON)
option(DRAGONBURN_BUILD_KERNEL "Build the DragonBurn-kernel image" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}/built")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/built_dbg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")

# UACExecutionLevel RequireAdministrator equivalent: embeds a manifest that requests elevation for the executables.
set(DRAGONBURN_UAC_ADMIN "/MANIFESTUAC:level='requireAdministrator'")

if(DRAGONBURN_BUILD_MAPPER)

add_executable(DragonBurn-mapper
    src/dragonburn-mapper/main.cpp
    src/dragonburn-mapper/intel-driver.cpp
    src/dragonburn-mapper/kdmapper.cpp
    src/dragonburn-mapper/portable-executable.cpp
    src/dragonburn-mapper/service.cpp
    src/dragonburn-mapper/utils.cpp
    src/dragonburn-mapper/resource.rc
)

target_include_directories(DragonBurn-mapper PRIVATE
    src/dragonburn-mapper/include
)

target_compile_features(DragonBurn-mapper PRIVATE cxx_std_17)

target_compile_definitions(DragonBurn-mapper PRIVATE
    UNICODE
    _UNICODE
    $<$<CONFIG:Debug>:DBDEBUG>
)

target_compile_options(DragonBurn-mapper PRIVATE
    /MP
    /permissive-
    /sdl
    /W4
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Release>:/Oi>
    $<$<CONFIG:Release>:/Gy>
)

# WholeProgramOptimization (LTCG) in Release.
set_property(TARGET DragonBurn-mapper PROPERTY
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

target_link_libraries(DragonBurn-mapper PRIVATE version)

target_link_options(DragonBurn-mapper PRIVATE
    ${DRAGONBURN_UAC_ADMIN}
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
)

endif()

if(DRAGONBURN_BUILD_USERMODE)

add_executable(DragonBurn-usermode
    src/dragonburn-usermode/main.cpp
    src/dragonburn-usermode/config/config-menu.cpp
    src/dragonburn-usermode/config/config-saver.cpp
    src/dragonburn-usermode/core/cheats.cpp
    src/dragonburn-usermode/core/global-vars.cpp
    src/dragonburn-usermode/core/memory.cpp
    src/dragonburn-usermode/features/aimbot.cpp
    src/dragonburn-usermode/features/misc.cpp
    src/dragonburn-usermode/features/radar.cpp
    src/dragonburn-usermode/features/rcs.cpp
    src/dragonburn-usermode/features/triggerbot.cpp
    src/dragonburn-usermode/game/bone.cpp
    src/dragonburn-usermode/game/entity.cpp
    src/dragonburn-usermode/game/game.cpp
    src/dragonburn-usermode/helpers/storage.cpp
    src/dragonburn-usermode/helpers/ui-access.cpp
    src/dragonburn-usermode/offsets/offsets.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-base.cpp
    src/dragonburn-usermode/imgui-extencion/ex-imgui-external.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_demo.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_draw.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_dx11.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_impl_win32.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_tables.cpp
    src/dragonburn-usermode/imgui-extencion/imgui/imgui_widgets.cpp
    src/dragonburn-usermode/resources/resource.rc
)

target_include_directories(DragonBurn-usermode PRIVATE
    src/dragonburn-usermode
    src/dragonburn-usermode/dependencies/json
)

target_compile_features(DragonBurn-usermode PRIVATE cxx_std_20)

target_compile_definitions(DragonBurn-usermode PRIVATE
    UNICODE
    _UNICODE
    _CONSOLE
    $<$<CONFIG:Debug>:DBDEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(DragonBurn-usermode PRIVATE
    /MP
    /permissive-
    /W3
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Release>:/Oi>
    $<$<CONFIG:Release>:/Gy>
)

# WholeProgramOptimization (LTCG) in Release.
set_property(TARGET DragonBurn-usermode PROPERTY
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Change MultiThreadedDLL to MultiThreaded for a self-contained release exe.
set_property(TARGET DragonBurn-usermode PROPERTY
    MSVC_RUNTIME_LIBRARY
    "$<$<CONFIG:Debug>:MultiThreadedDebug>$<$<NOT:$<CONFIG:Debug>>:MultiThreadedDLL>")

# d3d11/dxgi: ImGui DX11 backend and overlay renderer; dwmapi: overlay window composition.
target_link_libraries(DragonBurn-usermode PRIVATE d3d11 dxgi dwmapi)

target_link_options(DragonBurn-usermode PRIVATE
    ${DRAGONBURN_UAC_ADMIN}
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
)

endif()

# Project Instructions: DragonBurn

## Project Overview
DragonBurn is a CS2 (Counter-Strike 2) external read-only cheat with kernel-mode driver support. It uses a kernel driver to read game memory from kernel mode, bypassing user-mode anti-cheat detection. The project consists of two main components: `DragonBurn-kernel` (driver mapper) and `DragonBurn-usermode` (the cheat application).

## Tech Stack
- **Language**: C++20 (MSVC on Windows)
- **Build System**: CMake (3.20+) with Visual Studio project files
- **Target Platform**: Windows only
- **UI Framework**: ImGui with DirectX 11 backend (OS-ImGui wrapper)
- **Driver Mapping**: kdmapper library for kernel driver mapping
- **JSON**: nlohmann/json for configuration and API responses
- **Target Game**: Counter-Strike 2 (cs2.exe)

## Project Structure

### Root Level
- `CMakeLists.txt` - Root CMake configuration (multi-project: usermode + kernel)
- `DragonBurn.sln` - Visual Studio solution file
- `README.md` - Project documentation with setup instructions
- `LICENSE` - GNU GPL v3.0

### DragonBurn-kernel/
Kernel-mode driver mapper component:
- `main.cpp` - Entry point, driver mapping orchestration
- `kdmapper.cpp` - Driver mapping implementation (AllocationMode: AllocatePool or AllocateIndependentPages)
- `intel_driver.cpp` - Intel driver connection and unloading
- `service.cpp` - Windows service management
- `portable_executable.cpp` - PE image parsing
- `utils.cpp` - Utility functions
- `include/` - Header files with driver interfaces

### DragonBurn-usermode/
User-mode cheat application:
- `main.cpp` - Application entry, initialization flow
- `Config/` - Configuration UI and persistence
- `Core/` - Core infrastructure (Cheats orchestrator, Memory manager, GUI, Init)
- `Features/` - Cheat features (Aimbot, ESP, Misc, Radar, RCS, SoundESP, SpectatorList, TriggerBot)
- `Game/` - Game data structures (Bone, Entity, Game state, View matrix)
- `Helpers/` - Utilities (KeyManager, Logger, Mouse, StorageMgr, UIAccess, WebApi)
- `Offsets/` - Game offset management and auto-update
- `OS-ImGui/` - ImGui wrapper with DirectX 11 rendering and external window attachment
- `Resources/` - Embedded resources (Fonts, Images, Language strings, Sounds, Weapon icons)
- `Libs/json/` - nlohmann/json library

## Build & Run

### Prerequisites
- Windows 10/11
- Visual Studio 2022 with C++ support (MSVC)
- CMake 3.20+

### Build Commands
```bash
# CMake build (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Visual Studio build
msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64
```

### Run Commands
```bash
# First, map the kernel driver (requires admin)
./built/DragonBurn-kernel.exe [--securemode] [--legacyimg] [--forceprefs]

# Then run the cheat
./built/DragonBurn.exe [--securemode] [--legacyimg] [--forceprefs]

# Flags:
# --securemode: Use non-contiguous memory pages for better stealth
# --legacyimg: Use legacy kernel image
# --forceprefs: Force apply recommended Windows kernel preferences
```

### Output Directories
- Release: `built/`
- Debug: `built_dbg/`

## Conventions

### Code Style
- **Files**: PascalCase for headers and cpp files (`Aimbot.h`, `MemoryMgr.cpp`)
- **Classes**: PascalCase (`class OSImGui`, `namespace Cheats`)
- **Functions**: camelCase (`void UpdateOffsets()`, `bool ConnectDriver()`)
- **Variables**: camelCase for locals, `m_` prefix for members, `g_` prefix for globals
- **Constants**: UPPER_CASE or PascalCase depending on context
- **Namespaces**: `Cheats`, `Init`, `Misc`, `OSImGui`, `kdmapper`

### Logging
Use the custom `Log` class from `Helpers/Logger.h` (kernel) or `DragonBurn-usermode/Helpers/Logger.h`:
```cpp
Log::Info("Message");      // Information
Log::Error("Message");     // Error (red)
Log::Warning("Message");   // Warning (yellow)
Log::Fine("Message");      // Success (green)
Log::Custom("Message", 13); // Custom color
Log::PreviousLine();        // Move cursor up one line
```

### Error Handling
- Use try-catch for web operations with retry logic
- Use goto-based retry loops for version checking and offset updates (pattern used throughout)
- Return error codes from functions when appropriate

### Memory Reading
All game memory reading goes through the kernel driver:
```cpp
memoryManager.ConnectDriver(L"\\.\\DragonBurn-kmd");
memoryManager.Attach(pid);
// Then use memoryManager.Read<type>(address) for reading
```

### Offset Management
Offsets are automatically updated from `a2x/cs2-dumper` via web API:
```cpp
Offset.UpdateOffsets();  // Fetches and parses latest offsets
gGame.InitAddress();      // Resolves addresses from offsets
```

### Adding New Features
1. Create new `.h` and `.cpp` files in `DragonBurn-usermode/Features/`
2. Add feature declaration to `DragonBurn-usermode/Core/Cheats.h`
3. Implement feature logic in the new `.cpp` file
4. Add feature toggles to `Config/ConfigMenu.cpp`
5. Add feature execution to `Cheats::Run()` in `Core/Cheats.cpp`

### Configuration Options
Configuration options are defined in `DragonBurn-usermode/Core/Config.h` (MenuConfig namespace) and saved/loaded via `Config/ConfigSaver.cpp`.

## Git Workflow

### Commit Message Format
Follow Conventional Commits as documented in `docs/CommitMessagesGuidance.md`:
```
type(scope): description

Detailed description

Tags and references
```

**Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

**Examples:**
- `fix: skip rendering when window is not focused`
- `refactor: update offset provider to a2x/cs2-dumper`
- `feat: add securemode flag for driver mapping`
- `docs: add commands to fix vulnerable driver blocklist`

### Branches
- `stable` - Main development branch (protected)
- `WIP` - Work in progress branch
- `kernel` - Kernel-specific development

### Pull Requests
Target branches: `stable`, `WIP`, `kernel`
CI runs on all three branches (see `.github/workflows/build-check.yml`)

## Important Notes

1. **Kernel Driver**: The actual kernel driver binary is closed-source and must be embedded in `cfg.h` as a byte array (`cfg::image` and `cfg::imageLegacy`)

2. **Windows Preferences**: The cheat checks and may require specific Windows kernel preferences to be disabled:
   - VulnerableDriverBlocklistEnable
   - HypervisorEnforcedCodeIntegrity
   - EnableVirtualizationBasedSecurity
   - RunAsPPL

3. **Anti-Cheat Compatibility**: Undetected by most anti-cheats except FaceIt. The cheat automatically stops FaceIt, Vanguard services on startup.

4. **Version Checking**: Both kernel and usermode versions are checked against a remote JSON bin (https://api.jsonbin.io/v3/b/690e4759ae596e708f4b20b3)

5. **UI Access**: In release mode, the cheat elevates to UIAccess level via `Helpers/UIAccess.cpp`

## External Dependencies
- **kdmapper**: Kernel driver mapper library (included in source)
- **nlohmann/json**: JSON parsing library (included in `Libs/json/`)
- **ImGui**: Dear ImGui library with DirectX 11 backend (included in `OS-ImGui/imgui/`)
- **STB**: stb_image for image loading (included in `OS-ImGui/stb/`)
- **a2x/cs2-dumper**: Offset provider via web API

## File Modification Guidelines

### DO:
- Match existing naming conventions (PascalCase files, camelCase functions)
- Use the `Log` namespace for all console output
- Route all memory reads through the kernel driver (MemoryMgr)
- Use goto for retry logic (established pattern)
- Add new features to the appropriate directory
- Follow the existing configuration pattern

### DON'T:
- Modify kernel driver source (closed-source component)
- Use direct process memory reading (bypass kernel driver)
- Remove or modify anti-debug checks without understanding implications
- Change the web API endpoints without updating version checking
- Commit compiled binaries or driver images to the repository

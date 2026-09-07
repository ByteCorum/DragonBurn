# DragonBurn - Detailed Project Information

## Overview
DragonBurn is a CS2 (Counter-Strike 2) kernel-mode read-only external cheat with extensive features, full customization, and automatic offset updates. This is a **Windows-only C++ project** using kernel-mode driver for memory reading to bypass user-mode anti-cheat detection.

**Important**: This is a cheating software. Use for educational purposes only in controlled environments. May be detected by anti-cheat systems (Faceit confirmed, others unknown). May violate game terms of service.

## Architecture

### Dual-Component Design

```
┌─────────────────────────────────────────────────────────────┐
│                    DragonBurn Architecture                      │
├─────────────────────────────┬───────────────────────────────┤
│      User-Mode Component      │       Kernel-Mode Component     │
│  (dragonburn-usermode)        │      (dragonburn-kernel)       │
├─────────────────────────────┼───────────────────────────────┤
│  ┌─────────────────────────┐  │  ┌─────────────────────────┐  │
│  │         GUI              │  │  │     Kernel Driver         │  │
│  │    (ImGui + DirectX)     │  │  │  (Memory Reading)         │  │
│  └─────────────────────────┘  │  └─────────────────────────┘  │
│           │                     │            │                    │
│  ┌─────────────────────────┐  │  ┌─────────────────────────┐  │
│  │      Cheat Features      │◄─┼─►│   Intel Driver Interface   │  │
│  │  (Aimbot, ESP, Radar,     │  │  │  (kdmapper wrapper)        │  │
│  │   TriggerBot, Misc)      │  │  └─────────────────────────┘  │
│  └─────────────────────────┘  │            │                    │
│           │                     │            ▼                    │
│  ┌─────────────────────────┐  │  ┌─────────────────────────┐  │
│  │    Memory Management     │  │  │    Mapped Kernel Driver   │  │
│  │   (User-mode interface)  │◄─┼─►│   (DragonBurn-kmd)         │  │
│  └─────────────────────────┘  │  └─────────────────────────┘  │
└─────────────────────────────┴───────────────────────────────┘
```

### Component Responsibilities

**User-Mode Component (`dragonburn-usermode`)**
- ImGui-based user interface with menu system
- Cheat feature implementations (aimbot, ESP, radar, triggerbot, etc.)
- Configuration management and persistence (JSON-based)
- Game process detection and attachment to cs2.exe
- Offset management and automatic updates from online source
- Rendering overlay on game window using DirectX 11

**Kernel-Mode Component (`dragonburn-kernel`)**
- Kernel driver mapping and management using kdmapper
- Intel driver loading and unloading for memory access
- Secure memory mapping options (non-contiguous pages)
- Windows kernel preference verification
- Anti-cheat service mitigation (stops Faceit, Vanguard)

### Data Flow

1. **Initialization**
   User runs DragonBurn-usermode.exe → Checks Windows version → Creates config directory → Verifies cheat version online → Launches kernel mapper if driver not mapped

2. **Kernel Mapping**
   Kernel mapper runs → Checks kernel preferences → Stops anti-cheat services → Loads Intel driver → Maps DragonBurn driver → Returns status

3. **Game Attachment**
   User-mode waits for cs2.exe → Detects process ID → Attaches via kernel driver → Updates offsets → Initializes addresses

4. **Cheat Loop**
   Main loop: Reads game memory → Processes entities → Executes features → Renders UI → Handles input

## Features

### Visual
- Box ESP (multiple types, rounding, filled, gradient)
- Skeleton
- Snap Line
- Sound ESP
- Bomb ESP
- Health/Armor Bar
- Weapon/Ammo
- Distance
- Name
- Scoped/Blind indicators
- AWP Crosshair
- Eye Ray
- Visual Color customization

### Radar Hack
- Point Size
- Proportion
- Range
- Alpha

### Aimbot
- Start Bullet selection
- Aim Lock
- Draw FOV
- Visible Check
- Auto Only
- Flash Check
- Scope Check
- Humanization
- FOV/Smooth settings
- Multi Hitbox selection

### RCS (Recoi Compensation)
- Yaw
- Pitch
- Preview

### Trigger Bot
- Scope Check
- Flash Check
- Stop Check
- Shot Delay
- Shot Duration
- TTD (Time to Die)

### Misc
- Bomb Timer
- Bunny Hop
- Head Line
- Hit Sound
- Hit Markers
- Auto knife
- Auto zeus
- Auto accept
- Spectator list
- Watermark
- Anti Record

## Tech Stack Details

| Component | Technology | Version | Purpose |
|-----------|-----------|---------|---------|
| **Language** | C++ | C++20 (usermode), C++17 (kernel) | Core development |
| **Build System** | CMake | 3.20+ | Cross-platform build config |
| **IDE** | Visual Studio | 2022 (17.10+) | Primary development |
| **Target** | Windows | 10/11 x64 | Only platform |
| **Architecture** | x64 | - | 64-bit only |
| **UI Framework** | ImGui | Latest | User interface |
| **Graphics API** | DirectX 11 | - | Rendering backend |
| **JSON Library** | nlohmann/json | Single-header | Config parsing |
| **Kernel Mapper** | kdmapper | Custom | Driver mapping |
| **Intel Driver** | Custom | - | Kernel operations |

### External Dependencies (Windows SDK)
- DirectX 11 headers and libraries
- Windows API: user32, kernel32, gdi32, winspool, comdlg32, advapi32, shell32, ole32, oleaut32, uuid, odbc32, odbccp32
- COM interfaces

## Project Structure

### Root Level
```
.
├── CMakeLists.txt          # CMake build configuration
├── dragonburn.sln         # Visual Studio solution
├── README.md              # Project documentation
├── LICENSE                # MIT License
├── docs/                  # Additional documentation
│   └── commit-messages-guidance.md
├── resources/             # Static assets
│   ├── assets/            # Icons, logos
│   ├── fonts/             # Font files
│   └── readme/            # README images
└── src/                   # Source code
    ├── dragonburn-kernel/ # Kernel component
    │   ├── include/        # Kernel headers
    │   └── *.cpp           # Kernel source
    └── dragonburn-usermode/ # User-mode component
        ├── config/         # Configuration UI/management
        ├── core/           # Core functionality
        ├── dependencies/   # Third-party libraries
        ├── features/       # Cheat features
        ├── game/           # Game-specific code
        ├── helpers/        # Utilities
        ├── imgui-extencion/ # ImGui + STB
        ├── offsets/        # Memory offsets
        └── resources/      # Resource headers
```

### User-Mode Directory Structure
```
src/dragonburn-usermode/
├── main.cpp                          # Application entry point
├── config/
│   ├── config-menu.cpp/.h            # Config UI menu
│   └── config-saver.cpp/.h           # Config save/load
├── core/
│   ├── cheats.cpp/.h                 # Main cheat loop
│   ├── config.h                      # Configuration variables
│   ├── globals.h                     # Global constants
│   ├── global-vars.cpp/.h            # Global variables
│   ├── gui.h                         # GUI interface
│   ├── init.h                        # Initialization
│   ├── memory.cpp/.h                 # Memory management
│   └── render.h                      # Render functions
├── dependencies/
│   └── json/                         # nlohmann/json
├── features/
│   ├── aimbot.cpp/.h                 # Aimbot feature
│   ├── bomb-timer.h                  # Bomb timer
│   ├── esp.h                         # ESP feature
│   ├── misc.cpp/.h                   # Misc features
│   ├── radar.cpp/.h                  # Radar hack
│   ├── rcs.cpp/.h                     # Recoil control
│   ├── sound-esp.h                   # Sound ESP
│   ├── spectator-list.h              # Spectator list
│   └── triggerbot.cpp/.h             # Trigger bot
├── game/
│   ├── bone.cpp/.h                   # Bone management
│   ├── entity.cpp/.h                 # Entity classes
│   ├── game.cpp/.h                   # Game logic
│   └── view.h                        # View handling
├── helpers/
│   ├── format.h                      # Format utilities
│   ├── getweaponicon.h               # Weapon icons
│   ├── key-manager.h                # Key management
│   ├── logger.h                      # Logging system
│   ├── mouse.h                        # Mouse control
│   ├── network.h                     # Network utilities
│   ├── storage.cpp/.h                # Storage
│   ├── ui-access.cpp/.h              # UI access
│   ├── weapon-icons.h                # Weapon icons
│   └── webapi.h                      # Web API
├── imgui-extencion/
│   ├── imgui/                        # ImGui library
│   │   ├── imconfig.h                # ImGui config
│   │   ├── imgui.cpp/.h              # ImGui core
│   │   ├── imgui_demo.cpp            # Demo
│   │   ├── imgui_draw.cpp            # Drawing
│   │   ├── imgui_impl_dx11.cpp/.h    # DirectX 11 impl
│   │   ├── imgui_impl_win32.cpp/.h  # Win32 impl
│   │   ├── imgui_internal.h          # Internal
│   │   ├── imgui_tables.cpp         # Tables
│   │   ├── imgui_widgets.cpp         # Widgets
│   │   └── imstb_*.h                # STB headers
│   ├── ex-imgui.cpp/.h               # Extended ImGui
│   ├── ex-imgui-base.cpp/.h         # Base
│   ├── ex-imgui-exception.h         # Exceptions
│   ├── ex-imgui-external.cpp/.h     # External
│   ├── ex-imgui-struct.h             # Structures
│   └── stb/                         # STB image
│       └── stb_image.h               # Image loading
├── offsets/
│   └── offsets.cpp/.h               # Memory offsets
└── resources/
    ├── resource.rc/.h                # Resources
    ├── icon.ico                     # Icon
    ├── font.h                       # Fonts
    ├── images.h                     # Images
    ├── language.h                   # Language strings
    ├── sounds.h                     # Sounds
    └── weaponicon.h                 # Weapon icons
```

### Kernel Directory Structure
```
src/dragonburn-kernel/
├── main.cpp                          # Kernel mapper entry
├── intel-driver.cpp                 # Intel driver interface
├── kdmapper.cpp                     # Driver mapper
├── portable-executable.cpp         # PE handling
├── service.cpp                     # Service management
├── utils.cpp                       # Utilities
├── resource.rc                      # Resources
├── icon.ico                         # Icon
└── include/
    ├── cfg.h                         # Configuration
    ├── crypto.h                      # Crypto
    ├── intel-driver.h               # Intel driver header
    ├── intel-driver-resource.h     # Resource header
    ├── json.hpp                     # JSON
    ├── kdmapper.h                    # Mapper header
    ├── logger.h                     # Logger
    ├── nt.h                          # Windows NT headers
    ├── portable-executable.h         # PE header
    ├── service.h                     # Service header
    ├── utils.h                       # Utils header
    ├── web-api.h                     # Web API
    └── resource.h                    # Resource header
```

## Build System

### CMake Configuration
The project uses CMake 3.20+ with the following key settings:

```cmake
cmake_minimum_required(VERSION 3.20)
project(DragonBurnSolution VERSION 3.3.8.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-only
if(NOT WIN32)
    message(FATAL_ERROR "This project is designed for Windows only")
endif()

# Configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/built_dbg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/built")
```

### Build Options

**CMake Options:**
- `BUILD_USERMODE` (ON) - Build DragonBurn-usermode
- `BUILD_KERNEL` (ON) - Build DragonBurn-kernel

**Build Commands:**
```bash
# Build everything (both projects)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Build only user-mode
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_KERNEL=OFF
cmake --build build --config Release

# Build only kernel
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_USERMODE=OFF
cmake --build build --config Release

# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

**MSBuild Commands:**
```cmd
# Release build
msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64

# Debug build
msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Debug -p:Platform=x64
```

### Compiler Settings

**User-Mode (C++20):**
- Warning Level: Level 3
- SDL Check: Disabled
- Preprocessor Definitions: `DBDEBUG`, `_DEBUG`, `_CONSOLE` (Debug) / `NDEBUG`, `_CONSOLE` (Release)
- Conformance Mode: Enabled
- Language Standard: C++20
- Runtime Library: MultiThreadedDebug (Debug) / MultiThreaded (Release)

**Kernel (C++17):**
- Warning Level: Level 4
- Optimization: Disabled (Debug) / MaxSpeed (Release)
- SDL Check: Enabled
- Conformance Mode: Enabled
- Language Standard: C++17
- Spectre Mitigation: Disabled

### Linker Settings

**User-Mode Libraries:**
- d3d11, dxgi, user32, kernel32, gdi32, winspool, comdlg32, advapi32, shell32, ole32, oleaut32, uuid, odbc32, odbccp32
- SubSystem: Console
- UAC: RequireAdministrator

**Kernel Libraries:**
- version, kernel32, user32, gdi32, winspool, comdlg32, advapi32, shell32, ole32, oleaut32, uuid, odbc32, odbccp32
- SubSystem: Console
- UAC: RequireAdministrator

## Key Entry Points

### User-Mode Main
**File**: `src/dragonburn-usermode/main.cpp`

**Main Function**:
```cpp
int main(int argc, char* argv[])
```

**Key Operations:**
1. Parse command line arguments (`--securemode`, `--legacyimg`, `--forceprefs`)
2. Elevate to UIAccess for admin privileges
3. Initialize logging system
4. Verify cheat version online
5. Connect to kernel driver or auto-map
6. Wait for CS2 process
7. Update offsets
8. Initialize addresses
9. Attach to game and start ImGui overlay

**END Key**: Opens/closes menu

### Kernel Mapper Main
**File**: `src/dragonburn-kernel/main.cpp`

**Main Function**:
```cpp
int wmain(const int argc, wchar_t** argv)
```

**Key Operations:**
1. Check if driver already running
2. Verify mapper version online
3. Check Windows kernel preferences
4. Stop anti-cheat services (Faceit, Vanguard)
5. Load Intel driver
6. Map DragonBurn driver using kdmapper
7. Handle secure mode option

### Cheat Core Loop
**File**: `src/dragonburn-usermode/core/cheats.cpp`

**Main Function**:
```cpp
void Cheats::Run()
```

**Key Operations:**
1. Collect entity data
2. Process entities
3. Execute features (Aimbot, ESP, Radar, etc.)
4. Update radar
5. Play hit sounds

## Development Guidelines

### Adding a New Cheat Feature

1. **Create Files** in `src/dragonburn-usermode/features/`:
   ```bash
   touch new-feature.cpp new-feature.h
   ```

2. **Implement Feature** in `new-feature.cpp`:
   ```cpp
   #include "new-feature.h"
   
   namespace NewFeature {
       inline bool Enabled = false;
       inline int SomeSetting = 0;
       
       void Run(CEntity& local, std::vector<EntityResult>& entities) {
           if (!Enabled) return;
           // Feature implementation
       }
   }
   ```

3. **Add Header** in `new-feature.h`:
   ```cpp
   #pragma once
   #include "..\game\game.h"
   
   namespace NewFeature {
       extern void Run(CEntity& local, std::vector<EntityResult>& entities);
   }
   ```

4. **Integrate into Main Loop** in `cheats.cpp`:
   ```cpp
   #include "features/new-feature.h"
   
   void Cheats::Run() {
       // Existing code
       NewFeature::Run(localEntity, entities);
   }
   ```

5. **Add to Config Menu** in `config-menu.cpp`:
   ```cpp
   ImGui::Checkbox("Enable New Feature", &NewFeature::Enabled);
   ImGui::SliderInt("Setting", &NewFeature::SomeSetting, 0, 100);
   ```

6. **Add Persistence** in `config-saver.cpp`:
   ```cpp
   // Save
   j["NewFeature"]["Enabled"] = NewFeature::Enabled;
   j["NewFeature"]["SomeSetting"] = NewFeature::SomeSetting;
   
   // Load
   NewFeature::Enabled = j["NewFeature"]["Enabled"];
   NewFeature::SomeSetting = j["NewFeature"]["SomeSetting"];
   ```

7. **Add to CMakeLists.txt** (optional for IDE organization):
   ```cmake
   source_group("DragonBurn\Features" FILES
       src/dragonburn-usermode/features/new-feature.cpp
       src/dragonburn-usermode/features/new-feature.h
   )
   ```

### Updating Memory Offsets

1. **Edit** `src/dragonburn-usermode/offsets/offsets.cpp`
2. **Update offset values** from new CS2 updates
3. **Test** thoroughly with latest game version
4. **Commit** with message: `fix: update offsets for CS2 version X.X.X.X`

### Command Line Arguments

| Argument | Description |
|----------|-------------|
| `--securemode` | Use non-contiguous memory pages (more stealth) |
| `--legacyimg` | Use legacy image mapping method |
| `--forceprefs` | Force apply recommended Windows preferences |

### Configuration Location
- `Documents\DragonBurn\` - Config files, logs, data
- `default.cfg` - Default configuration
- `Data\` - Additional data files

### Build Outputs
- Release: `built/DragonBurn-usermode.exe` + `built/DragonBurn-kernel.exe`
- Debug: `built_dbg/DragonBurn-usermode.exe` + `built_dbg/DragonBurn-kernel.exe`

## Code Conventions

### File Naming
- **Source files**: `snake-case.cpp` (e.g., `config-menu.cpp`, `global-vars.cpp`)
- **Header files**: Corresponding `snake-case.h`
- **Directories**: kebab-case (e.g., `dragonburn-usermode`, `imgui-extencion`)

### Class/Namespace Naming
- **PascalCase** for namespaces (e.g., `AimControl`, `Cheats`, `MenuConfig`, `GUI`)
- **PascalCase** for classes/structs (e.g., `CEntity`, `globalvars`)
- **camelCase** or **snake_case** for variables (mixed usage in codebase)

### Code Patterns

**Namespaces for Feature Isolation:**
```cpp
namespace AimControl {
    inline int HotKey = VK_LBUTTON;
    inline bool ScopeOnly = true;
    inline bool IgnoreFlash = false;
    
    void AimBot(const CEntity& Local, Vec3 LocalPos, std::vector<Vec3>& AimPosList);
}
```

**Inline Configuration Variables:**
```cpp
// In header files
namespace MenuConfig {
    inline std::string name = "DragonBurn";
    inline std::string version = "v3.7.10.4";
    inline std::string author = "ByteCorum";
}
```

**Logging System:**
```cpp
// In helpers/logger.h
namespace Log {
    void Info(const std::string& message);           // Information (white)
    void Error(const std::string& message);          // Error (red)
    void Warning(const std::string& message);        // Warning (yellow)
    void Fine(const std::string& message);           // Success (green)
    void Custom(const std::string& message, int color); // Custom color
    void PreviousLine();                             // Move cursor up
}

// Usage
Log::Info("Checking cheat version...");
Log::PreviousLine();
Log::Fine("Your cheat version is up to date");
```

**Exception Handling:**
```cpp
try {
    bool result = SomeOperation();
    if (result)
        Log::Fine("Success");
    else
        Log::Error("Failed");
} catch (const std::exception& error) {
    Log::Error(error.what());
}
```

**Retry Logic (using goto):**
```cpp
int tryCount = 0;
RETRY_LABEL://RETRY_LABEL
try {
    if (!Operation()) {
        if (tryCount < 3) {
            tryCount++;
            goto RETRY_LABEL;//RETRY_LABEL
        }
    }
} catch (const std::exception& error) {
    Log::Error(error.what());
}
```

## Git Conventions

### Commit Message Format
```
<type>(optional scope): <description>

<optional body>

<optional tags/references>
```

**Supported Types:**
| Type | Description | Example |
|------|-------------|---------|
| `feat` | New feature | `feat: add bomb timer feature` |
| `fix` | Bug fix | `fix: broken files dependencies after last update` |
| `docs` | Documentation | `docs: add ai guidance` |
| `style` | Code style | `style: improve dev-experience with case-sensitive fs` |
| `refactor` | Refactoring | `refactor: update offset provider` |
| `test` | Testing | `test: add current branch to build test` |
| `chore` | Maintenance | `chore: update dependencies` |

### Branch Strategy
- `stable`: Production-ready, tested code
- `wip_maintenance`: Work in progress and maintenance
- Feature branches: Named after feature (e.g., `data`, `launcher`, `fixes`, `improve-debug`, `crypto`, `WriteMemory`)

### Pull Requests
- Use provided template in `.github/pull_request_template.md`
- Link to relevant issues
- Include screenshots/previews for UI changes
- Describe changes and testing performed

## Troubleshooting

### Common Issues and Solutions

| Issue | Solution |
|-------|----------|
| Failed to elevate to UIAccess | Run as Administrator |
| Vulnerable driver list enabled | Disable in registry or use `--forceprefs` |
| Driver already mapped | Reboot PC or use `--forceprefs` |
| Failed to connect to kernel driver | Ensure mapper ran, check AV/AC software |
| Offsets outdated | Wait for auto-update or manually update |
| CS2 not detected | Ensure CS2 is running before launching |
| AV/AC blocking | Temporarily disable real-time protection |

### Kernel Mapping Registry Fixes
```cmd
# Check if driver is running
sc query DragonBurn-kmd

# Disable vulnerable driver blocklist
reg add "HKLM\SYSTEM\CurrentControlSet\Control\CI\Config" /v VulnerableDriverBlocklistEnable /t REG_DWORD /d 0 /f

# Stop anti-cheat services
sc stop faceit
sc stop vgc
sc stop vgk

# Apply recommended preferences
reg add "HKLM\SYSTEM\CurrentControlSet\Control\DeviceGuard\Scenarios\HypervisorEnforcedCodeIntegrity" /v Enabled /t REG_DWORD /d 0 /f
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Lsa" /v RunAsPPL /t REG_DWORD /d 0 /f
reg add "HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\DeviceGuard" /v EnableVirtualizationBasedSecurity /t REG_DWORD /d 0 /f
bcdedit /set hypervisorlaunchtype off
```

## Version Management
- **Project version**: `CMakeLists.txt` line 2
- **Cheat version**: `src/dragonburn-usermode/core/config.h`
- **Kernel version**: `src/dragonburn-kernel/include/cfg.h`
- **Online version check**: JSONBin API endpoint

## Where to Look

| I want to... | Look at... |
|--------------|------------|
| Understand main flow | `src/dragonburn-usermode/main.cpp` |
| Add new cheat feature | `src/dragonburn-usermode/features/` |
| Modify existing feature | `src/dragonburn-usermode/features/<feature-name>.cpp` |
| Change UI menu | `src/dragonburn-usermode/config/config-menu.cpp` |
| Add configuration | `src/dragonburn-usermode/core/config.h` |
| Fix memory reading | `src/dragonburn-usermode/core/memory.cpp` |
| Update offsets | `src/dragonburn-usermode/offsets/offsets.cpp` |
| Change kernel mapping | `src/dragonburn-kernel/main.cpp` |
| Add logging | `src/dragonburn-usermode/helpers/logger.h` |
| Modify build settings | `CMakeLists.txt` or `.vcxproj` files |
| Update README | `README.md` |
| Change version | `CMakeLists.txt` + `core/config.h` + `cfg.h` |

## Important Warnings

### Legal & Ethical
- This is a cheating software for Counter-Strike 2
- May violate game terms of service
- Use only for educational purposes in controlled environments
- Can be detected by anti-cheat systems (Faceit confirmed)

### Technical
- Kernel-mode drivers can cause system instability
- Requires administrator privileges
- Anti-virus may flag the kernel driver as malicious
- Modifying kernel code can crash your system
- Offsets may become outdated with CS2 updates

### Development
- Windows-only - cannot be built or run on Linux/macOS
- Requires Windows 10/11 x64
- Requires Visual Studio 2022 for full build support
- CMake can be used but requires Windows toolchain

---

**Last Updated**: 2026-09-07
**Project Version**: v3.3.8.0 (CMake) / v3.7.10.4 (Cheat)
**Contact**: https://discord.gg/5WcvdzFybD
**Repository**: https://github.com/ByteCorum/DragonBurn

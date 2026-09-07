# Onboarding Guide: DragonBurn

## Overview
DragonBurn is a CS2 (Counter-Strike 2) kernel-mode read-only external cheat with extensive features including ESP, aimbot, radar hack, trigger bot, and miscellaneous utilities. The project uses a dual-component architecture with a user-mode application for UI and cheat logic, and a kernel-mode driver for safe memory reading to bypass anti-cheat detection.

The project is designed for Windows only (x64) and requires administrator privileges for kernel driver mapping. It's built with C++20, ImGui for UI, and custom kernel mapping technologies.

## Tech Stack

| Layer | Technology | Version | Purpose |
|-------|-----------|---------|---------|
| **Language** | C++ | C++20 | Core development language |
| **Build System** | CMake | 3.20+ | Cross-platform build configuration |
| **IDE** | Visual Studio | 2022 (17.10+) | Primary development environment |
| **Target Platform** | Windows | 10/11 x64 | Only supported platform |
| **Architecture** | x64 | - | 64-bit only |
| **UI Framework** | ImGui | Latest | User interface rendering |
| **Graphics API** | DirectX 11 | - | Rendering backend for ImGui |
| **JSON Library** | nlohmann/json | Single-header | Configuration and data parsing |
| **Kernel Mapper** | kdmapper | Custom | Driver mapping library |
| **Intel Driver** | Custom | - | Kernel-mode operations |

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
│                              │                              │
│  ┌─────────────────────────┐  │                              │
│  │      Game Process        │  │                              │
│  │   (cs2.exe - external)    │  │                              │
│  └─────────────────────────┘  │                              │
└─────────────────────────────┴───────────────────────────────┘
```

### Component Responsibilities

**User-Mode Component (`dragonburn-usermode`)**
- ImGui-based user interface with menu system
- Cheat feature implementations (aimbot, ESP, radar, etc.)
- Configuration management and persistence
- Game process detection and attachment
- Offset management and automatic updates
- Rendering overlay on game window

**Kernel-Mode Component (`dragonburn-kernel`)**
- Kernel driver mapping and management
- Intel driver loading and unloading
- Secure memory mapping options
- Windows kernel preference verification
- Anti-cheat service mitigation

### Data Flow

1. **Initialization**
   ```
   User runs DragonBurn-usermode.exe
   → Checks Windows version compatibility
   → Creates config directory in Documents\DragonBurn
   → Verifies cheat version online
   → Launches kernel mapper if driver not already mapped
   ```

2. **Kernel Mapping**
   ```
   Kernel mapper (DragonBurn-kernel.exe) runs
   → Checks kernel preferences (VulnerableDriverBlocklist, etc.)
   → Stops anti-cheat services (Faceit, Vanguard)
   → Loads Intel driver
   → Maps DragonBurn driver into kernel memory
   → Returns success/failure to user-mode
   ```

3. **Game Attachment**
   ```
   User-mode waits for cs2.exe process
   → Detects CS2 process ID
   → Attaches to process via kernel driver
   → Updates memory offsets from online source
   → Initializes game addresses
   ```

4. **Cheat Execution Loop**
   ```
   Main loop runs continuously:
   → Reads game memory via kernel driver
   → Processes entity data (players, weapons, etc.)
   → Executes cheat features based on configuration
   → Renders UI overlay
   → Handles user input
   ```

5. **Feature Processing**
   ```
   Entity collection → Filtering → Feature-specific processing → Rendering
   Example (ESP):
   Collect entities → Filter valid enemies → Calculate ESP data (box, health, etc.) → Draw on screen
   ```

## Key Entry Points

### Main Application
- **File**: `src/dragonburn-usermode/main.cpp`
- **Function**: `int main(int argc, char* argv[])`
- **Purpose**: Application entry point, initialization, and main cheat loop
- **Key operations**:
  - Command line argument parsing (`--securemode`, `--legacyimg`, `--forceprefs`)
  - UI Access elevation for admin privileges
  - Logging system initialization
  - Kernel driver connection/auto-mapping
  - CS2 process detection and attachment
  - Offset updates
  - ImGui overlay setup and main loop

### Kernel Mapper
- **File**: `src/dragonburn-kernel/main.cpp`
- **Function**: `int wmain(const int argc, wchar_t** argv)`
- **Purpose**: Kernel driver mapping and management
- **Key operations**:
  - Windows kernel preference checks
  - Anti-cheat service termination
  - Intel driver loading
  - Kernel driver mapping via kdmapper
  - Secure mode handling

### Cheat Core
- **File**: `src/dragonburn-usermode/core/cheats.cpp`
- **Function**: `void Cheats::Run()`
- **Purpose**: Main cheat execution loop
- **Key operations**:
  - Entity data collection and processing
  - Feature execution (Aimbot, ESP, Radar, etc.)
  - Radar hack updates
  - Hit sound effects

### Configuration System
- **File**: `src/dragonburn-usermode/config/config-menu.cpp`
- **Purpose**: UI configuration menu
- **File**: `src/dragonburn-usermode/config/config-saver.cpp`
- **Purpose**: Configuration saving and loading
- **Format**: JSON-based persistence

## Directory Map

### Root Level
| Directory | Purpose | Key Files |
|-----------|---------|------------|
| `/` | Root project directory | `CMakeLists.txt`, `dragonburn.sln`, `README.md` |
| `docs/` | Documentation | `commit-messages-guidance.md` |
| `resources/` | Static assets | Images, fonts, icons |
| `src/` | Source code | All C++ source files |
| `.github/` | GitHub configuration | Workflows, issue templates |

### Source Code Structure (`src/`)
| Directory | Purpose | Components |
|-----------|---------|------------|
| `dragonburn-kernel/` | Kernel-mode driver | Main mapper, Intel driver interface, service management |
| `dragonburn-usermode/` | User-mode application | All cheat functionality |

### User-Mode Subdirectories (`src/dragonburn-usermode/`)
| Directory | Purpose | Key Files |
|-----------|---------|------------|
| `config/` | Configuration management | Menu UI, config saving/loading |
| `core/` | Core functionality | Cheats, memory management, GUI, initialization |
| `dependencies/` | Third-party libraries | JSON parser (nlohmann/json) |
| `features/` | Cheat features | Aimbot, ESP, Radar, RCS, TriggerBot, Misc, etc. |
| `game/` | Game-specific code | Entity classes, bone management, view handling |
| `helpers/` | Utility classes | Logger, mouse control, UI access, web API |
| `imgui-extencion/` | ImGui library | ImGui source, STB image loader |
| `offsets/` | Memory offsets | Offset management and updates |
| `resources/` | Resource headers | Fonts, images, language strings |

## Request Lifecycle

### Example: Aimbot Feature Execution

```
1. User holds aim key (default: LMB)
   ↓
2. Cheats::Run() called in main loop
   ↓
3. Entity data collected (CollectEntityData)
   ↓
4. Entities processed (ProcessEntities)
   ↓
5. AimControl::AimBot() called with local entity and aim positions
   ↓
6. Target selection based on:
   - FOV check
   - Visible check
   - Scope check (if enabled)
   - Flash check (if enabled)
   - Auto-only mode
   ↓
7. Target offset calculation (CalculateTargetOffset)
   ↓
8. Humanization applied (if enabled)
   ↓
9. Mouse movement executed
```

### Example: ESP Rendering

```
1. Entity data collected and filtered
   ↓
2. For each valid entity:
   ├── Box ESP calculation
   ├── Health/Armor bar rendering
   ├── Weapon/Ammo display
   ├── Distance calculation
   ├── Name display
   ├── Skeleton rendering (if enabled)
   ├── Snap lines (if enabled)
   └── Other visual features
   ↓
3. All ESP elements drawn on ImGui overlay
```

### Example: Memory Read via Kernel

```
1. User-mode requests memory read
   ↓
2. Request sent to kernel driver via \\.\DragonBurn-kmd
   ↓
3. Kernel driver reads process memory safely
   ↓
4. Data returned to user-mode
   ↓
5. User-mode processes data for cheat features
```

## Conventions

### File Naming
- **Source files**: `snake-case.cpp` (e.g., `config-menu.cpp`, `global-vars.cpp`)
- **Header files**: Corresponding `snake-case.h` (e.g., `config-menu.h`)
- **Directories**: kebab-case (e.g., `dragonburn-usermode`, `imgui-extencion`)
- **Note**: Some historical inconsistencies exist (e.g., `imgui-extencion` vs `imgui-extention`)

### Class/Namespace Naming
- **PascalCase** for namespaces (e.g., `AimControl`, `Cheats`, `MenuConfig`)
- **PascalCase** for classes/structs (e.g., `CEntity`, `globalvars`)
- **snake_case** or **camelCase** for variables (mixed usage)

### Code Patterns

**Namespaces for Feature Isolation**
```cpp
namespace AimControl {
    inline int HotKey = VK_LBUTTON;
    inline bool ScopeOnly = true;
    void AimBot(const CEntity& Local, Vec3 LocalPos, std::vector<Vec3>& AimPosList);
}
```

**Inline Configuration Variables**
```cpp
// In header files
namespace MenuConfig {
    inline std::string name = "DragonBurn";
    inline std::string version = "v3.7.10.4";
    inline std::string author = "ByteCorum";
}
```

**Logging System**
```cpp
Log::Info("Message");           // Information message
Log::Error("Message");          // Error message (red)
Log::Warning("Message");        // Warning message (yellow)
Log::Fine("Message");           // Success/fine message (green)
Log::Custom("Message", color);  // Custom color message
Log::PreviousLine();             // Move cursor to previous line for overwrite
```

**Exception Handling**
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

**Goto-based Retry Logic**
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
} catch (...) {}
```

### Git Conventions

**Commit Message Format**
```
<type>(optional scope): <description>

<optional description body>

<optional tags/references>
```

**Supported Types**
| Type | Description | Example |
|------|-------------|---------|
| `feat` | New feature | `feat: add bomb timer feature` |
| `fix` | Bug fix | `fix: broken files dependencies after last update` |
| `docs` | Documentation | `docs: add ai guidance` |
| `style` | Code style changes | `style: improve dev-experience with case-sensitive fs` |
| `refactor` | Code refactoring | `refactor: update offset provider to a2x/cs2-dumper` |
| `test` | Testing changes | `test: add current branch to build test` |
| `chore` | Maintenance tasks | `chore: Update Python version to use newer libs` |

**Branch Strategy**
- `stable`: Production-ready, tested code
- `wip_maintenance`: Work in progress and maintenance branch
- Feature branches: Named after feature (e.g., `data`, `launcher`, `fixes`, `improve-debug`, `crypto`, `WriteMemory`)

**Pull Requests**
- Use provided template
- Link to relevant issues
- Include screenshots/previews for UI changes
- Describe changes and testing performed

## Common Tasks

### Adding a New Cheat Feature

1. **Create feature files**
   ```bash
   # In src/dragonburn-usermode/features/
   touch new-feature.cpp new-feature.h
   ```

2. **Implement feature logic** in `new-feature.cpp`
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

3. **Add to cheats.h**
   ```cpp
   #include "features/new-feature.h"
   
   // Add to Cheats namespace
   void RunNewFeature(...);
   ```

4. **Integrate into main loop** in `cheats.cpp`
   ```cpp
   void Cheats::Run() {
       // ... existing code
       NewFeature::Run(localEntity, entities);
   }
   ```

5. **Add to config menu** in `config-menu.cpp`
   ```cpp
   // Add UI controls for the new feature
   ImGui::Checkbox("Enable New Feature", &NewFeature::Enabled);
   ImGui::SliderInt("Setting", &NewFeature::SomeSetting, 0, 100);
   ```

### Updating Memory Offsets

1. **Edit** `src/dragonburn-usermode/offsets/offsets.cpp`
2. **Update offset values** from new CS2 updates
3. **Test** thoroughly with latest game version
4. **Commit** with descriptive message

### Adding Configuration Option

1. **Add to config.h**
   ```cpp
   namespace MenuConfig {
       inline bool NewOption = false;
   }
   ```

2. **Add to config menu** in `config-menu.cpp`
   ```cpp
   ImGui::Checkbox("New Option", &MenuConfig::NewOption);
   ```

3. **Handle persistence** in `config-saver.cpp`
   ```cpp
   // Add to save/load functions
   j["NewOption"] = MenuConfig::NewOption;
   MenuConfig::NewOption = j["NewOption"];
   ```

### Building the Project

**Using CMake**
```bash
# Create build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Output: built/DragonBurn-usermode.exe
```

**Using MSBuild (Windows)**
```cmd
# Release build
msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64

# Output: built/DragonBurn-usermode.exe

# Debug build
msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Debug -p:Platform=x64

# Output: built_dbg/DragonBurn-usermode.exe
```

### Running the Project

1. **Prerequisites**
   - Windows 10/11 x64
   - Admin privileges
   - Counter-Strike 2 installed and running

2. **Execute**
   ```cmd
   # Release
   .\built\DragonBurn-usermode.exe
   
   # Debug (console visible)
   .\built_dbg\DragonBurn-usermode.exe
   ```

3. **In-game**
   - Press `END` key to open/close menu
   - Navigate menu with mouse
   - Toggle features as needed

### Debugging

**Visual Studio**
1. Open `dragonburn.sln`
2. Set `dragonburn-usermode` as startup project
3. Set command line arguments if needed (`--securemode`, `--legacyimg`, etc.)
4. Start debugging (F5)

**Logging**
- All operations logged to console
- Use `Log::` methods for custom logging
- Debug builds show console by default

### Troubleshooting

**Common Issues and Solutions**

| Issue | Solution |
|-------|----------|
| Failed to elevate to UIAccess | Run as Administrator |
| Vulnerable driver list enabled | Run mapper with `--forceprefs` or disable in registry |
| Driver already mapped | Reboot PC or use `--forceprefs` |
| Failed to connect to kernel driver | Ensure mapper ran successfully, check AV/AC software |
| Offsets outdated | Wait for automatic update or manually update offsets |
| CS2 not detected | Ensure CS2 is running before launching DragonBurn |
| AV/AC blocking | Disable real-time protection temporarily |

**Kernel Mapping Issues**
```cmd
# Check if driver is running
sc query DragonBurn-kmd

# Stop anti-cheat services
sc stop faceit
sc stop vgc
sc stop vgk

# Disable vulnerable driver blocklist
reg add "HKLM\SYSTEM\CurrentControlSet\Control\CI\Config" /v VulnerableDriverBlocklistEnable /t REG_DWORD /d 0 /f
```

## Where to Look

| I want to... | Look at... |
|--------------|------------|
| **Understand main flow** | `src/dragonburn-usermode/main.cpp` |
| **Add a new cheat feature** | `src/dragonburn-usermode/features/` |
| **Modify existing feature** | `src/dragonburn-usermode/features/<feature-name>.cpp` |
| **Change UI menu** | `src/dragonburn-usermode/config/config-menu.cpp` |
| **Add configuration** | `src/dragonburn-usermode/core/config.h` |
| **Fix memory reading** | `src/dragonburn-usermode/core/memory.cpp` |
| **Update offsets** | `src/dragonburn-usermode/offsets/offsets.cpp` |
| **Change kernel mapping** | `src/dragonburn-kernel/main.cpp` |
| **Add logging** | `src/dragonburn-usermode/helpers/logger.h` |
| **Modify build settings** | `CMakeLists.txt` or `.vcxproj` files |
| **Update README** | `README.md` |
| **Change version** | `CMakeLists.txt` (line 2) and `core/config.h` |
| **Add resources** | `resources/` directory |

## Version Management

- **Project version**: Defined in `CMakeLists.txt` (line 2)
- **Cheat version**: Defined in `src/dragonburn-usermode/core/config.h`
- **Kernel version**: Defined in `src/dragonburn-kernel/include/cfg.h`
- **Online version check**: Against JSONBin API endpoint

## Dependencies

### Internal Dependencies
- `OS-ImGui` library (included in repo)
- `kdmapper` library (included in repo)
- `nlohmann/json` (single-header, included in repo)
- `STB` image library (included in repo)

### External Dependencies (Windows SDK)
- DirectX 11
- Windows API (user32, kernel32, gdi32, etc.)
- COM interfaces

### Build Dependencies
- Visual Studio 2022
- Windows 10/11 SDK
- CMake 3.20+

## Contribution Guidelines

1. **Fork the repository**
2. **Create feature branch** from `stable` or `wip_maintenance`
3. **Follow commit message conventions** (see `docs/commit-messages-guidance.md`)
4. **Test thoroughly** before submitting PR
5. **Update documentation** if adding new features
6. **Use existing code patterns** (namespaces, logging, etc.)
7. **Submit PR** to appropriate branch

## Important Warnings

- **Legal**: This is a cheating software for Counter-Strike 2. Use at your own risk.
- **Detection**: May be detected by anti-cheat systems (Faceit confirmed, others unknown)
- **Security**: Kernel-mode drivers can compromise system security
- **Stability**: May cause system instability or crashes
- **Ethical**: Use only for educational purposes in controlled environments

## Quick Reference

### Hotkeys
- `END`: Toggle menu
- `LMB`: Aimbot (configurable)
- Other keys configurable in Misc settings

### Command Line Arguments
| Argument | Description |
|----------|-------------|
| `--securemode` | Use non-contiguous memory pages (more stealth) |
| `--legacyimg` | Use legacy image mapping method |
| `--forceprefs` | Force apply recommended Windows preferences |

### Configuration Location
- `Documents\DragonBurn\` - Config files, logs, and data
- `default.cfg` - Default configuration
- `Data\` - Additional data files

### Build Outputs
- Release: `built/DragonBurn-usermode.exe`
- Debug: `built_dbg/DragonBurn-usermode.exe`
- Kernel: `DragonBurn-kernel.exe` (auto-generated)

---

**Last Updated**: 2026-09-07  
**Project Version**: v3.3.8.0 (CMake) / v3.7.10.4 (Cheat)  
**Contact**: https://discord.gg/5WcvdzFybD  
**Repository**: https://github.com/ByteCorum/DragonBurn

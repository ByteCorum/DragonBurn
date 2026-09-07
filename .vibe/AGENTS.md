# Project Instructions - DragonBurn

## Overview
DragonBurn is a CS2 kernel-mode read-only external cheat with wide range of features, full customization, and automatic offset updates. This is a Windows-only C++ project using kernel-mode driver for memory reading.

## Tech Stack
| Layer | Technology | Version/Details |
|-------|-----------|----------------|
| Language | C++ | C++20 standard |
| Build System | CMake + MSBuild | Visual Studio 2022 (17.10+) |
| Target Platform | Windows | x64 architecture only |
| UI Framework | ImGui | OS-ImGui wrapper with DirectX 11 |
| JSON Library | nlohmann/json | Single-header implementation |
| Kernel Mapper | kdmapper | Custom driver mapping |
| Intel Driver | Custom | For kernel-mode operations |

## Project Structure

### Root Directory
```
.
├── CMakeLists.txt          # CMake build configuration (primary)
├── dragonburn.sln         # Visual Studio solution file
├── README.md              # Project documentation and features list
├── LICENSE                # MIT License
├── docs/                  # Additional documentation
│   └── commit-messages-guidance.md
├── resources/             # Static assets (images, fonts, readme assets)
│   ├── assets/            # Icons, logos, other UI assets
│   ├── fonts/             # Font files
│   └── readme/            # README images
└── src/                   # Source code
    ├── dragonburn-kernel/ # Kernel-mode driver component
    │   ├── include/        # Kernel headers
    │   └── *.cpp           # Kernel source files
    └── dragonburn-usermode/ # User-mode application
        ├── config/         # Configuration UI and management
        ├── core/           # Core functionality (cheats, memory, GUI)
        ├── dependencies/   # Third-party libraries (json)
        ├── features/       # Cheat features (aimbot, radar, triggerbot, etc.)
        ├── game/           # Game-specific code (entities, bones, view)
        ├── helpers/        # Utility classes and helpers
        ├── imgui-extencion/ # ImGui library and STB image
        ├── offsets/        # Memory offset management
        └── resources/      # Resource headers
```

### Key Entry Points
- **User-mode**: `src/dragonburn-usermode/main.cpp` - Main application entry
- **Kernel-mode**: `src/dragonburn-kernel/main.cpp` - Kernel mapper entry
- **Cheat Logic**: `src/dragonburn-usermode/core/cheats.cpp` - Main cheat loop
- **GUI**: `src/dragonburn-usermode/core/gui.h` - ImGui-based UI
- **Configuration**: `src/dragonburn-usermode/config/config-menu.cpp` - Config UI

## Architecture Pattern
**Dual-component architecture**:
1. **User-mode component** (`dragonburn-usermode`): Handles UI (ImGui), cheat features, game memory reading via kernel driver
2. **Kernel-mode component** (`dragonburn-kernel`): Maps kernel driver for safe memory reading, handles anti-cheat bypass

**Data Flow**:
```
User Input → GUI (ImGui) → Cheat Features → Memory Requests → Kernel Driver → Game Memory → Processed Data → Render Overlay
```

## Build & Run

### Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2022 with C++ development tools
- CMake 3.20+
- Admin privileges for kernel mapping

### Build Commands
- **CMake**: `cmake -B build -DCMAKE_BUILD_TYPE=Release` then `cmake --build build --config Release`
- **MSBuild**: `msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64`
- **Debug**: `msbuild DragonBurn.sln -t:Rebuild -p:Configuration=Debug -p:Platform=x64`

### Run
- Execute `built/DragonBurn-usermode.exe` (release) or `built_dbg/DragonBurn-usermode.exe` (debug)
- Press END key to open/close menu
- Kernel mapper will auto-execute if needed

### Clean
- `cmake --build build --target clean` or manually delete `built/` and `built_dbg/` directories

## Conventions

### File Naming
- **C++ files**: `snake-case.cpp` with corresponding `snake-case.h`
- **Headers**: PascalCase for class/namespace names, snake_case for filenames
- **Directories**: kebab-case (e.g., `dragonburn-usermode`, `imgui-extencion`)

### Code Style
- **Namespaces**: Used extensively for feature organization (e.g., `AimControl`, `Cheats`, `MenuConfig`)
- **Inline variables**: `inline` used for configuration variables in headers
- **Logging**: Custom `Log::` namespace with `Info`, `Error`, `Warning`, `Fine`, `Custom` methods
- **Error handling**: Exceptions with try/catch, return codes for critical operations
- **Constants**: `#define` for compile-time constants, `inline` variables for runtime configs

### Git Conventions
- **Commit message format**: `<type>(optional scope): <description>`
- **Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`
- **Branches**: 
  - `stable` - Production-ready code
  - `wip_maintenance` - Work in progress/maintenance
  - Feature branches prefixed with feature name (e.g., `data`, `launcher`, `fixes`)
- **Pull requests**: Use provided template, link to issues

### Coding Patterns
- **Singletons**: Global configuration objects (e.g., `MenuConfig`, `gGame`)
- **Feature isolation**: Each cheat feature in separate namespace/file
- **Memory safety**: Kernel-mode for memory reading to avoid detection
- **Config persistence**: JSON-based config saving/loading
- **Version checking**: Online version verification before execution

## Testing
- **Build verification**: GitHub Actions workflow (`.github/workflows/build-check.yml`)
- **Test triggers**: Push to `stable` or `wip_maintenance`, Pull requests to these branches
- **Platform**: Windows-latest runner, x64 Release build
- **Artifacts**: Built executables archived as artifacts

## Security & Anti-Cheat
- **Kernel mode**: Driver runs in kernel space to bypass user-mode detection
- **Secure mapping**: `--securemode` flag for non-contiguous memory pages
- **Anti-cheat bypass**: Stops anti-cheat services (Faceit, Vanguard) before mapping
- **Version verification**: Online checks for cheat updates
- **Window version check**: OS compatibility verification

## Common Tasks

| Task | Command/Location |
|------|-----------------|
| Add new cheat feature | Create new file in `src/dragonburn-usermode/features/` |
| Add configuration | Update `src/dragonburn-usermode/core/config.h` |
| Update offsets | Modify `src/dragonburn-usermode/offsets/offsets.cpp` |
| Add UI element | Edit `src/dragonburn-usermode/config/config-menu.cpp` |
| Add kernel functionality | Edit `src/dragonburn-kernel/` files |
| Change build settings | Edit `CMakeLists.txt` or project `.vcxproj` files |
| Update documentation | Edit `README.md` or files in `docs/` |

## Important Notes
- **Admin required**: Application requires admin privileges for kernel mapping
- **AV/AC conflicts**: Antivirus and anti-cheat software may flag the kernel driver
- **CS2 dependency**: Requires Counter-Strike 2 running to attach
- **Online dependency**: Version checking requires internet connection
- **Platform limitation**: Windows only (explicit check in CMakeLists.txt)

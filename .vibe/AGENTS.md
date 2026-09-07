# DragonBurn Project - AI Instructions

## Critical Constraints
- Windows-only (x64). CMake fails with FATAL_ERROR on non-Windows
- Kernel-mode driver requires admin privileges
- Anti-cheat detection risk (Faceit confirmed)
- Educational use only in controlled environments

## Tech Stack
| Layer | Technology | Details |
|-------|-----------|---------|
| Language | C++ | C++20 (usermode), C++17 (kernel) |
| Build | CMake 3.20+ / MSBuild | Visual Studio 2022 |
| UI | ImGui | DirectX 11, OS-ImGui wrapper |
| Kernel | kdmapper | Windows kernel driver mapping |

## Project Structure
```
.
├── CMakeLists.txt              # Build config (separate or together)
├── src/dragonburn-usermode/    # User-mode app (cheat features, UI)
└── src/dragonburn-kernel/      # Kernel driver (memory reading)
```

## Key Files
- **Entry**: `src/dragonburn-usermode/main.cpp` (user), `src/dragonburn-kernel/main.cpp` (kernel)
- **Cheats**: `src/dragonburn-usermode/core/cheats.cpp`
- **Config**: `src/dragonburn-usermode/core/config.h`
- **Offsets**: `src/dragonburn-usermode/offsets/offsets.cpp`

## Build Options
```bash
# Build everything
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Build only user-mode
cmake -B build -DBUILD_KERNEL=OFF

# Build only kernel
cmake -B build -DBUILD_USERMODE=OFF
```

## Conventions
- snake-case.cpp with matching .h headers
- PascalCase namespaces (AimControl, Cheats)
- `Log::Info/Error/Warning/Fine()` for logging
- Git: `<type>: <description>` (feat, fix, docs, style, refactor, test, chore)

## AI Behavior Rules
- NEVER suggest Linux/macOS - explicitly Windows-only
- ALWAYS warn about admin requirements for kernel operations
- ALWAYS mention legal/ethical implications
- Reference INFO.md for detailed project information

## Quick Reference
| Task | Location |
|------|----------|
| Add feature | `src/dragonburn-usermode/features/` |
| Add config | `src/dragonburn-usermode/core/config.h` |
| Update offsets | `src/dragonburn-usermode/offsets/offsets.cpp` |

See INFO.md for complete project details, architecture, and development guides.

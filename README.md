# DragonBurn 2

<p align="center">
  <img src="Assets/banner.png" alt="DragonBurn banner">
</p>

DragonBurn is a Counter-Strike 2 external cheat that combines a user-mode overlay with a kernel-mode mapper. This README walks a new user through the entire workflow—from preparing Windows and building the binaries, to launching the tool and exploring every feature the menu exposes.

> ⚠️ **Disclaimer:** The project is provided for educational purposes. Running it in online games breaks the CS2 terms of service and may result in bans, data loss, or other penalties. You are fully responsible for your own usage.

---

## 1. Project overview

DragonBurn is split into two deliverables that must travel together:

- **`DragonBurn.exe`** – the user-mode DirectX 11 overlay built with C++20 and ImGui. It renders the menu, reads game data, applies aim/trigger/visual logic, and manages configuration files.
- **`DragonBurn-kernel.exe`** – a kdmapper-based helper that loads the unsigned kernel driver powering DragonBurn's read/write primitives. The mapper must run with administrator rights before the overlay starts.

The repository layout reflects that separation:

| Folder | What it contains |
| --- | --- |
| `DragonBurn-usermode/` | Main cheat source: rendering (ImGui), gameplay features (`Features/`), entity logic (`Game/`), memory manager, configuration UI, and saved-config logic (`Config/`). |
| `DragonBurn-kernel/` | Mapper sources, driver resources, and helper utilities used to map the vulnerable Intel driver and load DragonBurn's kernel payload. |
| `Crypto/` | Helper utilities for encrypting resources that ship with the overlay. |
| `Assets/`, `imgs/` | Images used in the UI and documentation. |
| `DragonBurn.sln`, `CMakeLists.txt` | Solution and cross-platform CMake project files that build both components. |

---

## 2. Quick start with the latest release

1. **Download** the newest release archive from the [GitHub releases page](https://github.com/ByteCorum/DragonBurn/releases/latest) and extract it to an easy-to-remember folder (for example, `C:/DragonBurn`).
2. **Place the files together:** the extracted folder must contain `DragonBurn.exe`, `DragonBurn-kernel.exe`, and the `Assets`/`imgs` directories. Do not separate the executables.
3. **Prepare Windows (administrator required):**
   - Close CS2 and any anti-cheat launchers (FaceIt, Vanguard, etc.).
   - Temporarily disable real-time antivirus protection—Defender routinely flags mapper behaviour as malicious.
4. **Run the kernel mapper:** right-click `DragonBurn-kernel.exe` → *Run as administrator*. A console window opens, maps the vulnerable `iqvw64e.sys` driver, and injects DragonBurn’s payload. Wait for a `"[+] success"` message before closing it. If it fails, consult the [Troubleshooting](#6-troubleshooting) section.
5. **Launch CS2** and wait until you reach the main menu.
6. **Start the overlay:** run `DragonBurn.exe`. On the first run DragonBurn creates a `Documents/DragonBurn/` folder for configs, checks the cheat/game version against the GitHub metadata, and downloads the latest offsets.
7. **Toggle the menu:** press `END` to show/hide the overlay. Use the mouse to configure features, and enjoy.

You must repeat steps 3–7 every time you reboot or whenever the vulnerable driver unloads.

---

## 3. Building DragonBurn from source

> 💡 Building is only supported on 64-bit Windows. The CMake script aborts on other operating systems.

### 3.1 Prerequisites

Install the following before cloning the repository:

- **Visual Studio 2022** with the *Desktop development with C++* workload and MSVC toolset v143.
- **Windows 10/11 SDK** (10.0.19041 or newer). The SDK headers/libs are required by both the overlay and the mapper.
- **Windows Driver Kit (WDK) 10** to compile the kernel mapper with elevated privileges.
- **CMake 3.20+** (optional, only if you prefer generating the build with CMake instead of opening the solution directly).
- **Git** to clone and manage the repository.

> ℹ️ DragonBurn targets modern builds of Windows 10 and Windows 11. The mapper has been validated up to Windows 11 24H2 26100.4351.

### 3.2 Clone the repository

```powershell
cd C:/Projects
git clone https://github.com/ByteCorum/DragonBurn.git DragonBurn
cd DragonBurn
```

No submodules are required—the ImGui, STB, and JSON sources are already checked in under `DragonBurn-usermode/`.

### 3.3 Build the user-mode overlay (Visual Studio GUI)

1. Open `DragonBurn.sln` in Visual Studio 2022.
2. When prompted, allow Visual Studio to generate CMake cache information.
3. In the toolbar, pick the **`Release`** configuration and **`x64`** platform.
4. Right-click the **DragonBurn** project in Solution Explorer and choose **Build**. Visual Studio compiles the overlay and drops `DragonBurn.exe` into `DragonBurn-usermode/Release/` (Debug builds land in `DragonBurn-usermode/Debug/`).
5. Copy the produced `DragonBurn.exe` next to the mapper after you compile the kernel component.

### 3.4 Build the user-mode overlay (CMake command line)

If you prefer generating a Visual Studio solution with CMake:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target DragonBurn
```

The resulting executable is written to `built/DragonBurn.exe` (or `built_dbg/` for Debug). This matches the output paths configured in `CMakeLists.txt`.

### 3.5 Build the kernel mapper

1. Ensure the **WDK** is installed—the mapper links against driver-specific libraries and requests administrator UAC elevation in the manifest.
2. Still inside Visual Studio, right-click the **DragonBurn-kernel** project (listed under the “Kernel” solution folder) and build it in the **Release** configuration. Alternatively, build from the command line:

   ```powershell
   cmake --build build --config Release --target DragonBurn-kernel
   ```

3. The compiled mapper is placed in `DragonBurn-kernel/Release/DragonBurn-kernel.exe` (or `built/DragonBurn-kernel.exe` when using the CMake output path).
4. Copy `DragonBurn-kernel.exe` to the same directory as the overlay executable before running the cheat.

### 3.6 Ship the binaries together

Collect these artefacts in a single folder:

- `DragonBurn.exe`
- `DragonBurn-kernel.exe`
- The `Assets/` and `imgs/` folders (used for UI resources)
- Any custom configuration files from `%USERPROFILE%/Documents/DragonBurn/`

Launching the overlay without the mapper or without the resource folders will result in missing functionality.

---

## 4. Preparing Windows before each session

DragonBurn performs several environment checks during startup (see `Core/Init.h`). To avoid common pitfalls:

1. **Run everything as administrator.** The mapper requires elevated privileges to load the vulnerable Intel driver and register a temporary service.
2. **Disable conflicting protections when troubleshooting:**
   - Windows Defender real-time protection
   - Third-party antivirus suites
   - Anti-cheat services (e.g., FaceIt `sc stop faceit`, Riot Vanguard `sc stop vgc` & `sc stop vgk`)
3. **Reboot if connection fails.** If DragonBurn cannot connect to the kernel driver after a successful map, restart Windows and relaunch the mapper with the optional `--legacymethod` flag.
4. **Keep virtualization features off** when using the legacy mapper method. The repository documents the registry and `bcdedit` commands that disable the Microsoft vulnerable-driver blocklist when necessary.
5. **Stay up-to-date.** DragonBurn verifies the cheat version and fetches the current CS2 offsets from GitHub (`a2x/cs2-dumper`) on launch, so an active internet connection is required the first time you run a new build.

---

## 5. Using the overlay in CS2

Once the mapper has been executed and CS2 is running:

1. **Start DragonBurn.exe.** The overlay attaches to `cs2.exe`, downloads offsets, and links to the mapped kernel driver to read game memory.
2. **Open the menu** with the `END` key. Every tab is organised by feature type:
   - `Visual` controls ESP, chams, colors, indicators, and overlay aesthetics.
   - `Radar` configures the built-in 2D radar widget.
   - `Aimbot`, `RCS`, and `Trigger` manage targeting assistance.
   - `Misc` toggles automation helpers, quality-of-life tools, watermark, spectator list, etc.
3. **Change options with your mouse.** Values update live—no restart needed.
4. **Save or load configs** from the `Config` tab. Config files are JSON documents stored inside `%USERPROFILE%/Documents/DragonBurn/`. You can rename, delete, or share them freely.
5. **Hide the UI** with `END` once you finish configuring. The cheat keeps running in the background.

---

## 6. Feature catalog

Each menu tab exposes a wide range of tweakable options. Below is a newcomer-friendly breakdown.

### 6.1 Visual intelligence

- **Box ESP / Filled / Gradient / Outline / Rounding** – draw 2D boxes around enemies with optional fill, gradient, and edge rounding controls.
- **Skeleton** – render the player bone structure for precise positioning.
- **Snap lines & Eye ray** – connect players to screen edges or draw their view direction to gauge awareness.
- **Sound ESP** – display audio cues (footsteps, shots) as radial pings with custom colours.
- **Bomb ESP & Carrier ESP** – highlight the C4 on the ground or the player carrying it.
- **Health & Armour bars / numerical overlays** – monitor enemy vitals at a glance.
- **Weapon, Ammo, Distance & Name tags** – append textual details beside ESP boxes.
- **Scoped/Blind indicators + Blind Hide** – show when targets are scoped or flashed, with the option to hide ESP on blinded enemies.
- **AWP crosshair & Preview widget** – enable an AWP-style crosshair for no-scope shots and preview changes inside the menu.
- **Multi-colour & Out-of-FOV arrows** – customise visible/hidden colours and mark enemies outside of the screen.

### 6.2 Radar awareness

- **2D radar overlay** with adjustable range, scale, point size, alpha, and crosshair lines.
- **Custom radar mode** that lets you reposition and resize the radar widget for multi-monitor setups.

### 6.3 Aim assistance & recoil control

- **Aimbot** – configure start-bullet thresholds, aim-lock, FOV, smoothing, visible checks, humanisation curves, multi-hitbox selection, and flash/scope gating.
- **RCS (Recoil Control System)** – fine-tune pitch/yaw compensation with real-time previews so you can match your spray pattern.
- **Trigger bot** – auto-fire when enemies cross your crosshair, including scope/flash/stop checks and granular delay/duration/TTD values.

### 6.4 Miscellaneous helpers

- **Bomb timer** – track plant/defuse progress with on-screen timers.
- **Bunny hop & auto-strafe** – maintain momentum by automating jump inputs.
- **Hit sound & hit markers** – audible and visual confirmation when your bullets connect.
- **Auto-knife / auto-zeus** – automatically use knife or Zeus in lethal range.
- **Auto-accept** – accept matchmaking queues instantly.
- **Spectator list** – monitor who is watching you.
- **Watermark & anti-record** – add overlay branding and block tools that capture the window.

### 6.5 Configuration & automation

- **Config saver/loader** – export all menu settings to JSON with author metadata (`Config/ConfigSaver.cpp`).
- **Auto-offset updates** – download the latest offsets/buttons/class structures from GitHub (`Offsets/Offsets.cpp`).
- **Version checks** – verify both DragonBurn and CS2 versions through remote metadata before enabling features (`Core/Init.h`).
- **UI access helpers** – simulate mouse input safely on the overlay while keeping CS2 in focus (`Helpers/UIAccess.*`, `Helpers/Mouse.*`).

---

## 7. Troubleshooting

| Symptom | Likely cause | Fix |
| --- | --- | --- |
| Antivirus flags the files as malware | Defender or another AV is blocking the mapper. | Temporarily disable real-time protection while launching DragonBurn. |
| `[-] \\Device\\Nal is already in use.` | A previous kdmapper instance or Intel driver is still loaded. | Reboot Windows or use [NalFix](https://github.com/VollRagm/NalFix) to release the handle before rerunning the mapper. |
| `[-] Vulnerable driver list has blocked the driver` | Microsoft’s vulnerable driver blocklist is enabled. | Follow Microsoft’s guide to disable the blocklist or apply the registry edits documented in the mapper README. |
| Mapper succeeds but DragonBurn cannot connect | Stale driver state or blocked IPC. | Reboot and rerun the mapper with `--legacymethod`. Ensure virtualization-based security is disabled. |
| STATUS_IMAGE_CERT_REVOKED / 0xC0000603 | Windows rejected the vulnerable driver certificate. | Disable the vulnerable driver list in the registry and restart. |
| Crash or no menu after CS2 update | Offsets are outdated. | Re-launch DragonBurn with an internet connection so it can download the newest offsets, or wait for the repository to publish compatible values. |

If problems persist, inspect the console output of `DragonBurn.exe` for detailed log messages and check the `%USERPROFILE%/Documents/DragonBurn/logs/` folder (created when logging is enabled).

---

## 8. Community & support

- **Discord:** [Join the community](https://discord.gg/5WcvdzFybD) for announcements and support.
- **Ko-fi:** [Support the author](https://ko-fi.com/bytecorum) if DragonBurn proved useful.
- **Issues & contributions:** Open GitHub issues or pull requests when you encounter reproducible bugs or have improvements to share.

---

## 9. License

DragonBurn is released under the [MIT License](LICENSE). You are free to fork, modify, and redistribute the project as long as you retain the original copyright notice.

---

Happy tinkering, and remember to stay safe while experimenting.

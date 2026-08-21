<p align="center"><img src="resources/readme/banner.png"></p><p align="center">  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white">
  <img src="https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white">
  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white">
  <img src="https://img.shields.io/badge/build-passing-76B900?style=for-the-badge&logo=&logoColor=whit">
  <img src="https://img.shields.io/badge/tests-100/100-76B900?style=for-the-badge&logo=&logoColor=whit">
  <img src="https://img.shields.io/badge/code quality-A+-76B900?style=for-the-badge&logo=&logoColor=whit">
  <img src="https://img.shields.io/badge/license-MIT-blue?style=for-the-badge&logo=&logoColor=whit">
  <img src="https://img.shields.io/badge/DragonBurn-v3.7.10.4-blue?style=for-the-badge&logo=&logoColor=whit">
  <img src="https://img.shields.io/badge/CS2-000000?style=for-the-badge&logo=counter-strike&logoColor=white">
  <img src="https://img.shields.io/badge/Kernel mode-28004D?style=for-the-badge">
  <img src="https://img.shields.io/badge/offsets auto update-D06B57?style=for-the-badge">
  <img src="https://img.shields.io/badge/undetected-03C75A?style=for-the-badge">
  <img src="https://img.shields.io/badge/fullscreen-supported-76B900?style=for-the-badge">
</p>

---

<h3>
<p align="center">
DragonBurn is one of the best CS2 kernel mode read only external cheats. It has a wide range of features, full customization, and automatic offset updates. Undetected by all anti-cheats except faceit.
</p></h3>

<p align="center">
<a href="https://github.com/ByteCorum/DragonBurn/releases/latest/download/DragonBurn.exe">Download latest release</a><br>
</p>

---

### 🌐Join our community

<a href="https://discord.gg/5WcvdzFybD"><img src="https://invidget.switchblade.xyz/5WcvdzFybD"></a>

---

### 📋Features

Press END key to open/close menu.

<details>
<summary>Visual</summary>

- Box ESP
- Box Type
- Box Rounding
- Filled Box ESP
- Gradient Filled Box ESP
- Skeleton
- Snap Line
- Sound esp
- Bomb esp
- Bomb carrier esp
- Visual Color
- Eye Ray
- Health Bar
- Armor Bar
- Weapon
- Ammo
- Distance
- Name
- Scoped
- Blind
- Blind Hide
- AWP Crosshair
- Visual Preview
- etc
    </details>

<details>
<summary>Radar Hack</summary>

- Point Size
- Proportion
- Range
- Alpha
    </details>

<details>
<summary>Aimbot</summary>

- Start Bullet
- Aim Lock
- Draw Fov
- Visible Check
- Auto Only
- Flash Check
- Scope Check
- Humanization
- FOV
- Smooth
- Multi Hitboxes Selection
    </details>

<details>
<summary>RCS</summary>

- Yaw
- Pitch
- Preview
    </details>

<details>
<summary>Trigger Bot</summary>

- Scope Check
- Flash Check
- Stop Check
- Shot Delay
- Shot Duration
- TTD
    </details>

<details>
<summary>Misc</summary>

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
    </details>

---

### 🏗️

TODO

---

### 🛠️How to use

TODO

---

### ❌Errors

<img src="resources/readme/error0.png" width="400" height="90">

> Error: `Windows Defender, other antivirus programs, or anti-cheats may flag cheat as virus`
>
> Solution: Turn off real-time protection

---

### ❌Mapper errors

cmd should be opened as admin

> Error: `[x] Kernel-mode driver image is empty`
>
> Solution: Fill `std::vector<uint8_t> image = {};` in `cfg.h` with kernel binaries

> Error: `[x] \Device\Nal is already in use.`
>
> Solution: Use [NalFix](https://github.com/VollRagm/NalFix)

> Error: `[x] Your vulnerable driver list is enabled and have blocked the driver loading`
>
> Solution: Disable vulnerable driver list, [official solution](https://support.microsoft.com/en-au/topic/kb5020779-the-vulnerable-driver-blocklist-after-the-october-2022-preview-release-3fcbe13a-6013-4118-b584-fcfbc6a09936)

> Still getting: `[x] Failed to register and start service for the vulnerable driver`
>
> Solution: Turn off all antiviruses and all anti-cheat clients, usually caused by faceit anti-cheat
>
> Faceit: `sc stop faceit`
> Vanguard: `sc stop vgc` `sc stop vgk`

<!--
> Error: `Driver is mapped successfully but failed to connect to kernel`
>
> Solution: Reboot pc and manually run mapper with `--legacymethod`
-->

> [!TIP]
> These cmds should fix any issues (after executing restart pc):
>
> ```
> reg add \"HKLM\SYSTEM\CurrentControlSet\Control\DeviceGuard\Scenarios\HypervisorEnforcedCodeIntegrity\" /v Enabled /t REG_DWORD /d 0 /f
>
> reg add \"HKLM\SYSTEM\CurrentControlSet\Control\Lsa\" /v RunAsPPL /t REG_DWORD /d 0 /f
>
> reg add \"HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\DeviceGuard\" /v EnableVirtualizationBasedSecurity /t REG_DWORD /d 00000000 /f
>
> bcdedit /set hypervisorlaunchtype off
>
> reg add \"HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\CI\Config\" /v VulnerableDriverBlocklistEnable /t REG_DWORD /d 00000000 /f
> ```

---

### 🖼️Preview

<p align="center">
<img src="resources/readme/img0.png">
</p>

<p align="center">
<img src="resources/readme/img1.png">
</p>

<p align="center">
<img src="resources/readme/img2.png">
</p>

---

### 🏅Crew

## TODO

### 📲Contacts

TODO

---

### 💸Support

TODO

---

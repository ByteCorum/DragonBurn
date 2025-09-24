#include "Misc.h"
#include "..\Resources\Language.h"
#include <iostream>
#include <Shellapi.h>
#include <filesystem>
#include <random>
#include "../Helpers/Logger.h"
#include "../Core/Cheats.h"
namespace fs = std::filesystem;

namespace System {
	static inline void Key_Click_HWND(HWND hwnd, int vk, BOOL sleep_ = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, WM_KEYDOWN, vk, 0);
		if (sleep_) Sleep(1);
		SendMessage(hwnd, WM_KEYUP, vk, 0);
		if (sleep_) Sleep(1);
	}
	static inline void Key_Con_HWND(HWND hwnd, int vk, BOOL way = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, way ? WM_KEYDOWN : WM_KEYUP, vk, 0);
	}
}

namespace Misc
{
	HitMarker hitMarker(0, std::chrono::steady_clock::now());
	const float HitMarker::SIZE = 10.f;
	const float HitMarker::GAP = 3.f;

	void Watermark(const CEntity& LocalPlayer) noexcept
	{
		if (!MiscCFG::WaterMark || (LocalPlayer.Pawn.TeamID == 0 && !MenuConfig::ShowMenu))
			return;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowPos(MenuConfig::MarkWinPos, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(0.8f);

		ImGui::Begin("Watermark", nullptr, windowFlags);

		if (MenuConfig::MarkWinChengePos)
		{
			ImGui::SetWindowPos("Watermark", MenuConfig::MarkWinPos);
			MenuConfig::MarkWinChengePos = false;
		}

		Vec3 Pos = LocalPlayer.Pawn.Pos;
		int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
		char fpsText[32];
		snprintf(fpsText, sizeof(fpsText), "  FPS: %d", currentFPS);

		ImGui::Text("  DragonBurn");
		ImGui::Text("  Kernel CS2 cheat");
		ImGui::Text("  Velocity: %.2f", LocalPlayer.Pawn.Speed);
		ImGui::Text("%s", fpsText);
		//ImGui::Text("  Pos: %.1f, %.1f, %.1f ", Pos.x, Pos.y, Pos.z);
		ImGui::Text("                                                      ");

		MenuConfig::MarkWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}

	void HitSound() noexcept
	{
		switch (MiscCFG::HitSound)
		{
		case 1:
			PlaySoundA(reinterpret_cast<char*>(neverlose_sound), NULL, SND_ASYNC | SND_MEMORY);
			break;
		case 2:
			PlaySoundA(reinterpret_cast<char*>(skeet_sound), NULL, SND_ASYNC | SND_MEMORY);
			break;
		default:
			break;
		}
	}

	void HitManager(CEntity& LocalPlayer, int& PreviousTotalHits) noexcept
	{
		if ((!MiscCFG::HitSound && !MiscCFG::HitMarker) || LocalPlayer.Controller.TeamID == 0 || MenuConfig::ShowMenu || !LocalPlayer.IsAlive())
			return;

		uintptr_t pBulletServices;
		int totalHits;
		memoryManager.ReadMemory(LocalPlayer.Pawn.Address + Offset.Pawn.BulletServices, pBulletServices);
		memoryManager.ReadMemory(pBulletServices + Offset.Pawn.TotalHit, totalHits);

		if (totalHits != PreviousTotalHits) {
			if (totalHits == 0 && PreviousTotalHits != 0)
			{
				// `totalHits` changed from non-zero to zero, do nothing
			}
			else
			{
				if (MiscCFG::HitSound)
				{
					HitSound();
				}
				if (MiscCFG::HitMarker)
				{
					hitMarker = HitMarker(255.f, std::chrono::steady_clock::now());
					hitMarker.Draw();
				}
			}
		}

		hitMarker.Update();
		PreviousTotalHits = totalHits;
	}

	void BunnyHop(const CEntity& Local) noexcept
	{
		if (!MiscCFG::BunnyHop ||  MenuConfig::ShowMenu || Local.Controller.TeamID == 0)
			return;
		
		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (hwnd_cs2 == NULL) {
			hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		}

		//int JumpBtn;
		//if (!memoryManager.ReadMemory(gGame.GetJumpBtnAddress(), JumpBtn))
		//	return;

		bool spacePressed = GetAsyncKeyState(VK_SPACE);
		bool isInAir = !Local.Pawn.HasFlag(PlayerPawn::Flags::ON_GROUND);

		static DWORD lastJumped = GetTickCount64();
		DWORD currentTick = GetTickCount64();

		if (spacePressed && !isInAir)
		{
			if (currentTick - lastJumped >= MenuConfig::BunnyHopDelay)
			{
				SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
				SendMessage(hwnd_cs2, WM_KEYDOWN, VK_SPACE, 0);
				lastJumped = currentTick;
			}
		}
	}

	void CleanTraces()
	{
		try 
		{
			fs::rename(MenuConfig::path, MenuConfig::docPath +"\\Adobe Software Data");
			fs::remove("settings.yml");

			//std::string current_path = fs::current_path().string();
			//std::string current_dir = fs::current_path().parent_path().string();
		}
		catch (...) {}
	}


	static void PerformKeyStop(int opposite_key, int delay_ms)
	{
		keybd_event(opposite_key, MapVirtualKey(opposite_key, 0), 0, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
		keybd_event(opposite_key, MapVirtualKey(opposite_key, 0), KEYEVENTF_KEYUP, 0);
	}

	static void CheckAndStopKey(int key, int opposite_key, std::map<int, bool>& key_released_map, std::chrono::steady_clock::time_point& last_stop_time)
	{
		short key_state = GetAsyncKeyState(key);
		bool previously_released = key_released_map[key];

		if (key_state & 0x8000)
		{
			key_released_map[key] = true;
		}
		else if (previously_released)
		{
			auto it = keyLayouts.find(Layout);
			if (it != keyLayouts.end()) {
				const auto& layout = it->second;
				if (!(GetAsyncKeyState(layout.left) & 0x8000) &&
					!(GetAsyncKeyState(layout.right) & 0x8000) &&
					!(GetAsyncKeyState(layout.forward) & 0x8000) &&
					!(GetAsyncKeyState(layout.backward) & 0x8000))
				{
					last_stop_time = std::chrono::steady_clock::now();
					std::thread(PerformKeyStop, opposite_key, MiscCFG::FastStopDelay + MiscCFG::FastStopOffset).detach();
				}
			}
			key_released_map[key] = false;
		}
	}

	void FastStop() noexcept
	{
		if (!MiscCFG::FastStop)
			return;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_LSHIFT) & 0x8000)
			return;

		static auto last_stop_time = std::chrono::steady_clock::now();
		static std::map<int, bool> key_released_map;

		MiscCFG::FastStopOffset = dis(gen);
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_stop_time).count() < MiscCFG::FastStopDelay + MiscCFG::FastStopOffset)
			return;

		auto it = keyLayouts.find(Layout);
		if (it != keyLayouts.end()) {
			const auto& layout = it->second;
			CheckAndStopKey(layout.left, layout.right, key_released_map, last_stop_time);
			CheckAndStopKey(layout.right, layout.left, key_released_map, last_stop_time);
			CheckAndStopKey(layout.forward, layout.backward, key_released_map, last_stop_time);
			CheckAndStopKey(layout.backward, layout.forward, key_released_map, last_stop_time);
		}
	}


	void ExecuteCommand(const std::string& Command_Str) noexcept
	{
		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (!hwnd_cs2) return;
	
		COPYDATASTRUCT cds;
		cds.cbData = static_cast<DWORD>(Command_Str.size() + 1);
		cds.dwData = 0;
		cds.lpData = (void*)Command_Str.c_str();
		SendMessage(hwnd_cs2, WM_COPYDATA, 0, (LPARAM)&cds);
	
		// Key/mouse fallbacks mirroring original behavior
		if (Command_Str == "+jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, TRUE);
		else if (Command_Str == "-jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, FALSE);
	
		if (Command_Str == "+duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, TRUE);
		else if (Command_Str == "-duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, FALSE);
	
		if (Command_Str == "+lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, TRUE); // 'F'
		else if (Command_Str == "-lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, FALSE);
	
		if (Command_Str == "drop") System::Key_Click_HWND(hwnd_cs2, 0x47, TRUE); // 'G'
	
		if (Command_Str == "+attack") mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack") mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+attack2") mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack2") mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+forward") System::Key_Con_HWND(hwnd_cs2, 0x57, TRUE); // 'W'
		else if (Command_Str == "-forward") System::Key_Con_HWND(hwnd_cs2, 0x57, FALSE);
	
		if (Command_Str == "+back") System::Key_Con_HWND(hwnd_cs2, 0x53, TRUE); // 'S'
		else if (Command_Str == "-back") System::Key_Con_HWND(hwnd_cs2, 0x53, FALSE);
	
		if (Command_Str == "+left") System::Key_Con_HWND(hwnd_cs2, 0x41, TRUE); // 'A'
		else if (Command_Str == "-left") System::Key_Con_HWND(hwnd_cs2, 0x41, FALSE);
	
		if (Command_Str == "+right") System::Key_Con_HWND(hwnd_cs2, 0x44, TRUE); // 'D'
		else if (Command_Str == "-right") System::Key_Con_HWND(hwnd_cs2, 0x44, FALSE);
	
	}


	void KnifeBot(const CEntity& Local, const std::vector<CEntity>& Entities, int autoKnifeKey) noexcept
	{
		if (!(Local.Pawn.WeaponName == "ct_knife" || Local.Pawn.WeaponName == "t_knife"))
			return;

		if (autoKnifeKey != 0 && !(GetAsyncKeyState(autoKnifeKey) & 0x8000))
			return;
	
		const Vec3 localPos = Local.Pawn.Pos;
		const Vec3 eyePos = Local.Pawn.CameraPos;
		const Vec2 viewAngles = Local.Pawn.ViewAngle;
	
		auto normalizeAngle = [](float a) -> float {
			while (a > 180.0f) a -= 360.0f;
			while (a < -180.0f) a += 360.0f;
			return a;
		};
	
		auto calcAngleDelta = [&](const Vec3& src, const Vec3& dst, const Vec2& current) -> Vec2 {
			const float dx = dst.x - src.x;
			const float dy = dst.y - src.y;
			const float dz = dst.z - src.z;
			const float hyp = sqrtf(dx * dx + dy * dy);
			Vec2 aim;
			aim.x = -atan2f(dz, hyp) * 57.2957795f;
			aim.y = atan2f(dy, dx) * 57.2957795f;
			Vec2 delta;
			delta.x = normalizeAngle(aim.x - current.x);
			delta.y = normalizeAngle(aim.y - current.y);
			return delta;
		};
	
		for (const auto& E : Entities)
		{
			if (!E.IsAlive())
				continue;
			if (E.Controller.TeamID == Local.Controller.TeamID)
				continue;
	
			const Vec3 enemyPos = E.Pawn.Pos;
			Vec3 targetPos = enemyPos;
			const auto bone = E.GetBone();
			if (!bone.BonePosList.empty()) {
				size_t headIndex = BONEINDEX::head;
				if (headIndex < bone.BonePosList.size()) {
					targetPos = bone.BonePosList[headIndex].Pos;
				}
			}

			const float dx = localPos.x - enemyPos.x;
			const float dy = localPos.y - enemyPos.y;
			const float dz = localPos.z - enemyPos.z;
			const float dist = sqrtf(dx * dx + dy * dy + dz * dz);
			if (dist > 70.0f)
				continue;

			const Vec2 delta = calcAngleDelta(eyePos, targetPos, viewAngles);
			if (hypotf(delta.x, delta.y) > 40.0f)
				continue;

			const int enemyHp = E.Pawn.Health;
			const bool facing = fabsf(normalizeAngle(viewAngles.y - E.Pawn.ViewAngle.y)) <= 50.0f;
			if ((enemyHp <= 55 && enemyHp > 30) || facing)
			{
				ExecuteCommand("+attack2");
				Sleep(1);
				ExecuteCommand("-attack2");
			}
			else
			{
				ExecuteCommand("+attack");
				Sleep(1);
				ExecuteCommand("-attack");
			}
		}
	}

	void ZeusBot(const CEntity& Local) noexcept
	{
		if (Local.Pawn.WeaponName != "zeus") return;

		DWORD uHandle = 0;
		if (!memoryManager.ReadMemory<DWORD>(Local.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle)) return;
		if (uHandle == -1) return;
		DWORD64 PawnAddress = CEntity::ResolveEntityHandle(uHandle);
		if (PawnAddress == 0) return;
		CEntity targetEntity;
		if (!targetEntity.UpdatePawn(PawnAddress)) return;
		if (targetEntity.Pawn.Address == 0 || MenuConfig::TeamCheck && targetEntity.Controller.TeamID == Local.Controller.TeamID) return;

		const Vec3 localPos = Local.Pawn.Pos;
		const Vec3 enemyPos = targetEntity.Pawn.Pos;
		const float dx = localPos.x - enemyPos.x;
		const float dy = localPos.y - enemyPos.y;
		const float dz = localPos.z - enemyPos.z;
		const float dist = sqrtf(dx * dx + dy * dy + dz * dz);

		if (dist > MiscCFG::AutoZeusDistance)
			return;

		ExecuteCommand("+attack");
		Sleep(1);
		ExecuteCommand("-attack");
	}

	void AntiAFKKickUpdate() noexcept
	{
		if (!MiscCFG::AntiAFKKick) return;
		static DWORD64 s_lastTick = 0;
		DWORD64 now = GetTickCount64();
		if (now - s_lastTick >= 5000) { // fixed 5s
			mouse_event(MOUSEEVENTF_MOVE, 1, 0, 0, 0);
			Sleep(1);
			mouse_event(MOUSEEVENTF_MOVE, -1, 0, 0, 0);
			s_lastTick = now;
		}
	}

	namespace AutoAccept
	{
		static bool EnsureForegroundCS2()
		{
			HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
			if (!hwnd_cs2)
				return false;
	
			// Attach thread input to allow us to set foreground window
			HWND hwndForeground = GetForegroundWindow();
			DWORD fgThread = hwndForeground ? GetWindowThreadProcessId(hwndForeground, nullptr) : 0;
			DWORD thisThread = GetCurrentThreadId();
			if (fgThread && fgThread != thisThread)
				AttachThreadInput(fgThread, thisThread, TRUE);
	
			ShowWindow(hwnd_cs2, SW_RESTORE);
			BringWindowToTop(hwnd_cs2);
			SetWindowPos(hwnd_cs2, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(hwnd_cs2, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetForegroundWindow(hwnd_cs2);
			SetActiveWindow(hwnd_cs2);
			SetFocus(hwnd_cs2);
	
			if (fgThread && fgThread != thisThread)
				AttachThreadInput(fgThread, thisThread, FALSE);
	
			// Small delay to let focus settle
			Sleep(80);
			return GetForegroundWindow() == hwnd_cs2;
		}
	
		static std::thread autoAcceptThread;
		static std::atomic<bool> autoAcceptRunning(false);
		static std::atomic<int> iterationCount(0);
		static const int kColorTolerance = 20; // tolerance for RGB matching
		static const int kDefaultIntervalMs = 1500; // baked-in interval
		static const int kDefaultMinPixels = 10000; // baked-in threshold
		
		void StartAutoAccept()
		{
			if (autoAcceptRunning.load()) return;
			
			autoAcceptRunning.store(true);
			autoAcceptThread = std::thread([]() {
				while (autoAcceptRunning.load())
				{
					if (MiscCFG::AutoAccept)
					{
						iterationCount++;
						
						if (DetectAcceptButton())
						{
							Log::Debug(std::string("[AutoAccept] [") + std::to_string(iterationCount.load()) + "] Found accept button! Clicking...");
						}
					}
					
					std::this_thread::sleep_for(std::chrono::milliseconds(kDefaultIntervalMs));
				}
			});
		}
		
		void StopAutoAccept()
		{
			if (!autoAcceptRunning.load()) return;
			
			autoAcceptRunning.store(false);
			
			if (autoAcceptThread.joinable())
			{
				autoAcceptThread.join();
			}
		}
		
		bool DetectAcceptButton()
		{
			// Use virtual screen to be robust on multi-monitor setups
			int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
			int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
			int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
			
			// Create device context for screen
			HDC hdcScreen = GetDC(NULL);
			HDC hdcMem = CreateCompatibleDC(hdcScreen);
			HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmScreen);
			
			// Copy virtual screen to bitmap including layered windows
			BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, vx, vy, SRCCOPY | CAPTUREBLT);
			
			// Define region of interest around CS2 window if available, otherwise center of virtual screen
			RECT wndRect{};
			HWND hwnd_cs2_roi = FindWindowA(NULL, "Counter-Strike 2");
			int roiLeft = 0, roiTop = 0, roiRight = screenWidth, roiBottom = screenHeight;
			if (hwnd_cs2_roi && GetWindowRect(hwnd_cs2_roi, &wndRect))
			{
				int wndW = wndRect.right - wndRect.left;
				int wndH = wndRect.bottom - wndRect.top;
				int roiW = static_cast<int>(wndW * 0.6f);
				int roiH = static_cast<int>(wndH * 0.6f);
				int wndCX = wndRect.left + wndW / 2;
				int wndCY = wndRect.top + wndH / 2;
				roiLeft = max(0, wndCX - roiW / 2 - vx);
				roiTop = max(0, wndCY - roiH / 2 - vy);
				roiRight = min(screenWidth, roiLeft + roiW);
				roiBottom = min(screenHeight, roiTop + roiH);
			}
			else
			{
				int roiWidth = static_cast<int>(screenWidth * 0.5f);
				int roiHeight = static_cast<int>(screenHeight * 0.5f);
				roiLeft = (screenWidth - roiWidth) / 2;
				roiTop = (screenHeight - roiHeight) / 2;
				roiRight = roiLeft + roiWidth;
				roiBottom = roiTop + roiHeight;
			}
	
			// Search for green pixels (CS2 accept button colors) within ROI
			long long sumX = 0, sumY = 0;
			int greenPixelCount = 0;
			int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
			
			for (int x = roiLeft; x < roiRight; x += 1)
			{
				for (int y = roiTop; y < roiBottom; y += 1)
				{
					COLORREF pixelColor = GetPixel(hdcMem, x, y);
					if (IsGreenPixel(pixelColor))
					{
						greenPixelCount++;
						int absX = x + vx;
						int absY = y + vy;
						sumX += absX;
						sumY += absY;
						if (absX < minX) minX = absX;
						if (absY < minY) minY = absY;
						if (absX > maxX) maxX = absX;
						if (absY > maxY) maxY = absY;
					}
				}
			}
			
			// Clean up GDI objects
			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmScreen);
			DeleteDC(hdcMem);
			ReleaseDC(NULL, hdcScreen);
			
			// Prefer bounding-box center to avoid centroid bias from gradients
			int bboxW = (maxX >= minX) ? (maxX - minX + 1) : 0;
			int bboxH = (maxY >= minY) ? (maxY - minY + 1) : 0;
			if (greenPixelCount >= kDefaultMinPixels && bboxW > 80 && bboxH > 30)
			{
				int centerX = (minX + maxX) / 2;
				int centerY = (minY + maxY) / 2;
				ClickAcceptButton(centerX, centerY);
				return true;
			}
	
			// Fallback: use centroid if bbox looks too small or fragmented
			if (greenPixelCount >= kDefaultMinPixels)
			{
				int centroidX = static_cast<int>(sumX / greenPixelCount);
				int centroidY = static_cast<int>(sumY / greenPixelCount);
				ClickAcceptButton(centroidX, centroidY);
				return true;
			}
			
			return false;
		}
		
		void ClickAcceptButton(int x, int y)
		{
			// Bring CS2 window to foreground to ensure click is received
			HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
			if (hwnd_cs2)
				EnsureForegroundCS2();
	
			// Helper lambda to perform one click (SendInput + client messages)
			auto performClick = [&](int cx, int cy)
			{
				INPUT inputs[2] = {};
				inputs[0].type = INPUT_MOUSE;
				inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				inputs[1].type = INPUT_MOUSE;
				inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(2, inputs, sizeof(INPUT));
	
				if (hwnd_cs2)
				{
					POINT ptClientTopLeft{ 0, 0 };
					ClientToScreen(hwnd_cs2, &ptClientTopLeft);
					int clientX = cx - ptClientTopLeft.x;
					int clientY = cy - ptClientTopLeft.y;
					LPARAM lParam = MAKELPARAM(clientX, clientY);
					PostMessage(hwnd_cs2, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
					PostMessage(hwnd_cs2, WM_LBUTTONUP, 0, lParam);
				}
			};
	
			// Retry strategy: 3 rapid attempts with slight jitter (faster timings)
			for (int attempt = 0; attempt < 3; ++attempt)
			{
				int jitterX = x + (attempt == 0 ? 0 : (attempt == 1 ? 2 : -2));
				int jitterY = y-10;
				SetCursorPos(jitterX, jitterY);
				Sleep(attempt == 0 ? 45 : 25);
				performClick(jitterX, jitterY);
				Sleep(35); // allow UI to respond
			}
		}
		
		bool IsGreenPixel(COLORREF color)
		{
			// Extract RGB components
			int r = GetRValue(color);
			int g = GetGValue(color);
			int b = GetBValue(color);
			
			// Target colors for the Accept button
			const int r1 = 54, g1 = 183, b1 = 82;
			const int r2 = 20, g2 = 61, b2 = 11;
			
			auto withinTol = [&](int cr, int cg, int cb, int tr, int tg, int tb) -> bool {
				return abs(cr - tr) <= kColorTolerance && abs(cg - tg) <= kColorTolerance && abs(cb - tb) <= kColorTolerance;
			};
			
			// Strong match with tolerance or general green dominance
			if (withinTol(r, g, b, r1, g1, b1) || withinTol(r, g, b, r2, g2, b2))
				return true;
			
			if (g > r + 40 && g > b + 40 && g > 90)
				return true;
			
			return false;
		}
		
		void UpdateAutoAccept()
		{
			// Run only if current map name indicates lobby (empty or "<empty>")
			const std::string mapName = Cheats::GetCurrentMapName();
			const bool lobby = (mapName.empty() || mapName == "<empty>");
			const bool shouldRun = MiscCFG::AutoAccept && lobby;
			if (shouldRun)
			{
				if (!autoAcceptRunning.load())
					StartAutoAccept();
			}
			else
			{
				if (autoAcceptRunning.load())
					StopAutoAccept();
			}
		}
	}
}
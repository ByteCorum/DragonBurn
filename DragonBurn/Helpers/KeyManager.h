#pragma once
#include <string>
#include <windows.h>

namespace KeyMgr
{
    inline std::string GetKeyName(int vk_code)
    {
        switch (vk_code)
        {
        case VK_LBUTTON: return "LMB";
        case VK_RBUTTON: return "RMB";
        case VK_CANCEL: return "Cancel";
        case VK_MBUTTON: return "MMB";
        case VK_XBUTTON1: return "X1MB";
        case VK_XBUTTON2: return "X2MB";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Control";
        case VK_MENU: return "Alt";
        case VK_PAUSE: return "Pause";
        case VK_CAPITAL: return "Caps";
        case VK_ESCAPE: return "Escape";
        case VK_SPACE: return "Space";
        case VK_LEFT: return "Left";
        case VK_UP: return "Up";
        case VK_RIGHT: return "Right";
        case VK_DOWN: return "Down";
        case VK_PRINT: return "Prt Scr";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "Pg Up";
        case VK_NEXT: return "Pg Down";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        default:
        {
            wchar_t name[16];
            BYTE keyboardState[256];
            GetKeyboardState(keyboardState);
            UINT scanCode = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);
            int result = ToUnicode(vk_code, scanCode, keyboardState, name, sizeof(name) / sizeof(name[0]), 0);
            if (result > 0)
            {
                name[result] = L'\0';
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, NULL, NULL);
                std::string str(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, name, -1, &str[0], size_needed, NULL, NULL);
                str.pop_back();
                return str;
            }
            return "N/A";
        }
        }
    }

    inline void GetPressedKey(int& vk_code, std::string* keyName)
    {
        int hotkey = 0;
        std::string buff = *keyName;
        *keyName = "...";

        while (true)
        {
            for (int key = 0; key < 256; key++)
            {
                if (GetAsyncKeyState(key) & 0x8000)
                {
                    hotkey = key;
                }
            }

            if (hotkey == MenuConfig::HotKey)
            {
                *keyName = buff;
                break;
            }
            if (hotkey > 0 && hotkey < 256)
            {
                vk_code = hotkey;
                *keyName = GetKeyName(hotkey);
                break;
            }
            //Sleep(1);
        }
    }
}
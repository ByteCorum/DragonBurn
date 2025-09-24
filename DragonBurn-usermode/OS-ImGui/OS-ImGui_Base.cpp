#include "OS-ImGui_Base.h"
#include "..\Resources\WeaponIcon.h"
#include "..\Resources\Font.h"
#include "..\Resources\Language.h"

namespace OSImGui
{
    bool OSImGui_Base::InitImGui(ID3D11Device* device, ID3D11DeviceContext* device_context)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        (void)io;

        //ImWchar ranges[] = {
        //    0x0020, 0x007F,   // Basic Latin characters
        //    0x00A0, 0x00FF,   // Latin characters with diacritics
        //    0x0400, 0x04FF,   // Cyrillic
        //    0x4E00, 0x9FFF,   // Simplified Chinese characters
        //    0x3040, 0x309F,   // Japanese Hiragana
        //    0x30A0, 0x30FF,   // Japanese Katakana
        //    0xFF00, 0xFFEF,   // Full-width characters
        //    0x2500, 0x257F,   // Lines and borders
        //    0x2600, 0x26FF,   // Weather symbols and other basic symbols
        //    0x2700, 0x27BF,   // Check symbols and other graphic symbols
        //    0x1F600, 0x1F64F, // Emoji
        //    0 // End of ranges
        //};


        ImFontAtlas* fontAtlas = new ImFontAtlas();
        ImFontConfig arialConfig;
        arialConfig.FontDataOwnedByAtlas = false;

        ImFont* arialFont = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 20.0f, &arialConfig, fontAtlas->GetGlyphRangesDefault());
        
        ImFontConfig iconConfig;
        iconConfig.MergeMode = true;
        iconConfig.PixelSnapH = true;
        iconConfig.OversampleH = 3;
        iconConfig.OversampleV = 3;
        iconConfig.FontDataOwnedByAtlas = false;

        ImFont* WeaponIconFont = fontAtlas->AddFontFromMemoryTTF((void*)cs_icon, sizeof(cs_icon), 20.0f);

        io.Fonts = fontAtlas;

        ImGui::DragonBurnDefaultStyle();
        io.LogFilename = nullptr;

        if (!ImGui_ImplWin32_Init(Window.hWnd))
            throw OSException("ImGui_ImplWin32_Init() call failed.");
        if (!ImGui_ImplDX11_Init(device, device_context))
            throw OSException("ImGui_ImplDX11_Init() call failed.");

        return true;
    }

    void OSImGui_Base::CleanImGui()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        g_Device.CleanupDeviceD3D();
        DestroyWindow(Window.hWnd);
        UnregisterClassA(Window.ClassName.c_str(), Window.hInstance);
    }

    std::wstring OSImGui_Base::StringToWstring(std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }
}
#pragma once
#include "../Game.h"
#include "Globals.hpp"

class globalvars
{
public:
    DWORD64 address = 0;

    float g_fRealTime;
    int g_iFrameCount;
    int g_iMaxClients;
    float g_fIntervalPerTick;
    float g_fCurrentTime;
    float g_fCurrentTime2;
    int g_iTickCount;
    float g_fIntervalPerTick2;
    void* g_vCurrentNetchan;
    char* g_cCurrentMap;
    char* g_cCurrentMapName;

    bool UpdateGlobalvars();
    bool GetRealTime();
    bool GetFrameCount();
    bool GetMaxClients();
    bool GetIntervalPerTick();
    bool GetcurrentTime();
    bool GetcurrentTime2();
    bool GetTickCount();
    bool GetIntervalPerTick2();
    bool GetCurrentNetchan();
    bool GetCurrentMap();
    bool GetCurrentMapName();
};
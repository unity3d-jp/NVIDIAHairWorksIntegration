#pragma once
#define hwImpl

using namespace DirectX; // for DirectX Math

#ifdef _WIN32
    #define hwWindows
#endif

#ifdef hwDebug
    void hwDebugLogImpl(const char* fmt, ...);
    #define hwDebugLog(...) hwDebugLogImpl(__VA_ARGS__)
#else
    #define hwDebugLog(...)
#endif
#include "HairWorksIntegration.h"

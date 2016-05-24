#pragma once
#define hwImpl

using namespace DirectX; // for DirectX Math

#ifdef _WIN32
    #define hwWindows
#endif

void hwLogImpl(const char* fmt, ...);
#define hwLog(...) hwLogImpl(__VA_ARGS__)

#include "HairWorksIntegration.h"

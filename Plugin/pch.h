#include <algorithm>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <cstdint>

#include <d3d11.h>
#include <directXMath.h>
#include <GFSDK_HairWorks.h>

using namespace DirectX;

#ifdef _WIN32
#define hwWindows
#endif // _WIN32

#define hwCLinkage extern "C"
#ifdef _MSC_VER
#define hwExport __declspec(dllexport)
#else
#define hwExport __attribute__((visibility("default")))
#endif

#ifdef hwDebug
void hwDebugLogImpl(const char* fmt, ...);
#define hwDebugLog(...) hwDebugLogImpl(__VA_ARGS__)
#ifdef hwVerboseDebug
#define hwDebugLogVerbose(...) hwDebugLogImpl(__VA_ARGS__)
#else
#define hwDebugLogVerbose(...)
#endif
#else
#define hwDebugLog(...)
#define hwDebugLogVerbose(...)
#endif


#ifdef hwWindows
#include <windows.h>

#ifndef hwNoAutoLink
#endif // hwNoAutoLink

#ifdef hwSupportD3D11
#include <d3d11.h>
#endif // hwSupportD3D11

#endif // hwWindows

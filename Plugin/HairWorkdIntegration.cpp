#include "pch.h"
#include "HairWorkdIntegration.h"
#include "hwContext.h"

ID3D11Device *g_d3d11_device = nullptr;
hwContext *g_hwContext = nullptr;


// Graphics device identifiers in Unity
enum GfxDeviceRenderer
{
    kGfxRendererOpenGL = 0, // desktop OpenGL
    kGfxRendererD3D9 = 1, // Direct3D 9
    kGfxRendererD3D11 = 2, // Direct3D 11
    kGfxRendererGCM = 3, // PlayStation 3
    kGfxRendererNull = 4, // "null" device (used in batch mode)
    kGfxRendererXenon = 6, // Xbox 360
    kGfxRendererOpenGLES20 = 8, // OpenGL ES 2.0
    kGfxRendererOpenGLES30 = 11, // OpenGL ES 3.0
    kGfxRendererGXM = 12, // PlayStation Vita
    kGfxRendererPS4 = 13, // PlayStation 4
    kGfxRendererXboxOne = 14, // Xbox One
    kGfxRendererMetal = 16, // iOS Metal
};

// Event types for UnitySetGraphicsDevice
enum GfxDeviceEventType {
    kGfxDeviceEventInitialize = 0,
    kGfxDeviceEventShutdown,
    kGfxDeviceEventBeforeReset,
    kGfxDeviceEventAfterReset,
};

hwCLinkage hwExport void UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
{
    if (eventType == kGfxDeviceEventInitialize) {
        if (deviceType == kGfxRendererD3D11) {
            g_d3d11_device = (ID3D11Device*)device;
        }
    }

    if (eventType == kGfxDeviceEventShutdown) {
    }
}

hwCLinkage hwExport void UnityRenderEvent(int eventID)
{
}



bool hwInitialize(const char *path_to_dll)
{
    if (g_hwContext == nullptr) {

    }
}

void hwFinalize(hwContext *ctx)
{

}

hwContext* hwGetContext();
bool hwLoadAssetFromFile(hwContext *ctx, const char *path);
bool hwLoadAssetFromMemory(hwContext *ctx, const void *data);


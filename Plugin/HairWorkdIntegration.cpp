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



#ifdef hwDebug
void hwDebugLogImpl(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);

#ifdef hwWindows
    char buf[2048];
    vsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
#else // hwWindows
    vprintf(fmt, vl);
#endif // hwWindows

    va_end(vl);
}
#endif // hwDebug


hwCLinkage hwExport bool hwInitialize(const char *path_to_dll)
{
    if (g_hwContext != nullptr) {
        return true;
    }

    g_hwContext = new hwContext();
    if (g_hwContext->initialize(path_to_dll, g_d3d11_device)) {
        return true;
    }
    else {
        hwFinalize();
        return false;
    }
}

hwCLinkage hwExport void hwFinalize()
{
    delete g_hwContext;
    g_hwContext = nullptr;
}

hwCLinkage hwExport hwContext* hwGetContext()
{
    return g_hwContext;
}



hwCLinkage hwExport hwShaderID hwShaderLoadFromFile(const char *path)
{
    if (path == nullptr || path[0] == '\0') { return hwNullID; }
    if (auto ctx = hwGetContext()) {
        return ctx->shaderLoadFromFile(path);
    }
    return hwNullID;
}
hwCLinkage hwExport void hwShaderRelease(hwShaderID sid)
{
    if (auto ctx = hwGetContext()) {
        ctx->shaderRelease(sid);
    }
}


hwCLinkage hwExport hwAssetID hwAssetLoadFromFile(const char *path)
{
    if (path == nullptr || path[0]=='\0') { return hwNullID; }
    if (auto ctx = hwGetContext()) {
        return ctx->assetLoadFromFile(path);
    }
    return hwNullID;
}
hwCLinkage hwExport void hwAssetRelease(hwAssetID aid)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetRelease(aid);
    }
}


hwCLinkage hwExport hwInstanceID hwInstanceCreate(hwAssetID aid)
{
    if (auto ctx = hwGetContext()) {
        return ctx->instanceCreate(aid);
    }
    return hwNullID;
}
hwCLinkage hwExport void hwInstanceRelease(hwInstanceID iid)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceRelease(iid);
    }
}
hwCLinkage hwExport void hwInstanceGetDescriptor(hwInstanceID iid, hwHairDescriptor *desc)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceGetDescriptor(iid, *desc);
    }
}
hwCLinkage hwExport void hwInstanceSetDescriptor(hwInstanceID iid, const hwHairDescriptor *desc)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceSetDescriptor(iid, *desc);
    }
}
hwCLinkage hwExport void hwInstanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceUpdateSkinningMatrices(iid, num_matrices, matrices);
    }
}


hwCLinkage hwExport void hwSetViewProjection(const hwMatrix *view, const hwMatrix *proj, float fov)
{
    if (auto ctx = hwGetContext()) {
        ctx->setViewProjection(*view, *proj, fov);
    }
}

hwCLinkage hwExport void hwSetRenderTarget(void *framebuffer, void *depthbuffer)
{
    if (auto ctx = hwGetContext()) {
        ctx->setRenderTarget(framebuffer, depthbuffer);
    }
}

hwCLinkage hwExport void hwSetShader(hwShaderID sid)
{
    if (auto ctx = hwGetContext()) {
        ctx->setShader(sid);
    }
}

hwCLinkage hwExport void hwRender(hwInstanceID iid)
{
    if (auto ctx = hwGetContext()) {
        ctx->render(iid);
    }
}

hwCLinkage hwExport void hwRenderShadow(hwInstanceID iid)
{
    if (auto ctx = hwGetContext()) {
        ctx->renderShadow(iid);
    }
}

hwCLinkage hwExport void hwStepSimulation(float dt)
{
    if (auto ctx = hwGetContext()) {
        ctx->stepSimulation(dt);
    }
}

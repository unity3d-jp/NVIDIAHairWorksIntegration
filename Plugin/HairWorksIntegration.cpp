#include "pch.h"
#include "HairWorksIntegration.h"
#include "hwContext.h"

#if defined(_M_IX86)
    #define hwSDKDLL "GFSDK_HairWorks.win32.dll"
#elif defined(_M_X64)
    #define hwSDKDLL "GFSDK_HairWorks.win64.dll"
#endif

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
    if (eventID == hwFlushEventID) {
        if (auto ctx = hwGetContext()) {
            ctx->flush();
        }
    }
}


hwCLinkage hwExport void hwMoveContext(hwContext *ctx)
{
    if (ctx && g_hwContext) {
        g_hwContext->move(*ctx);
    }
}
typedef void(*hwMoveContextT)(hwContext *ctx);

#if !defined(hwMaster)
// PatchLibrary で突っ込まれたモジュールは UnitySetGraphicsDevice() が呼ばれないので、
// DLL_PROCESS_ATTACH のタイミングで先にロードされているモジュールからデバイスをもらって同等の処理を行う。
BOOL WINAPI DllMain(HINSTANCE module_handle, DWORD reason_for_call, LPVOID reserved)
{
    if (reason_for_call == DLL_PROCESS_ATTACH)
    {
        if (HMODULE m = ::GetModuleHandleA("HairWorksIntegration.dll")) {
            auto proc = (hwMoveContextT)::GetProcAddress(m, "hwMoveContext");
            if (proc) {
                g_hwContext = new hwContext();
                proc(g_hwContext);
                if (!g_hwContext->valid()) {
                    delete g_hwContext;
                    g_hwContext = nullptr;
                }
            }
        }
    }
    else if (reason_for_call == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}

// "DllMain already defined in MSVCRT.lib" 対策
#if defined(_M_IX86)
extern "C" { int __afxForceUSRDLL; }
#elif defined(_M_X64)
extern "C" { int _afxForceUSRDLL; }
#endif

#endif


/*hwThreadLocal*/ hwLogCallback g_hwLogCallback;

#ifdef hwDebug
void hwDebugLogImpl(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);

    char buf[2048];
    vsprintf(buf, fmt, vl);
#ifdef hwWindows
    ::OutputDebugStringA(buf);
#else // hwWindows
    printf(buf);
#endif // hwWindows
    if (g_hwLogCallback) { g_hwLogCallback(buf); }

    va_end(vl);
}
#endif // hwDebug



hwCLinkage hwExport bool hwInitialize()
{
    if (g_hwContext != nullptr) {
        return true;
    }

    char path[MAX_PATH];
    {
        // get path to this module
        HMODULE mod = 0;
        ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&hwInitialize, &mod);
        DWORD size = ::GetModuleFileNameA(mod, path, sizeof(path));
        for (int i = size - 1; i >= 0; --i) {
            if (path[i] == '\\') {
                path[i+1] = '\0';
                std::strncat(path, hwSDKDLL, MAX_PATH);
                break;
            }
        }
    }

    g_hwContext = new hwContext();
    if (g_hwContext->initialize(path, g_d3d11_device)) {
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
    hwInitialize();
    return g_hwContext;
}

hwCLinkage hwExport int hwGetFlushEventID()
{
    return hwFlushEventID;
}



hwCLinkage hwExport void hwSetLogCallback(hwLogCallback cb)
{
    g_hwLogCallback = cb;
}

hwCLinkage hwExport hwHShader hwShaderLoadFromFile(const char *path)
{
    if (path == nullptr || path[0] == '\0') { return hwNullHandle; }
    if (auto ctx = hwGetContext()) {
        return ctx->shaderLoadFromFile(path);
    }
    return hwNullHandle;
}
hwCLinkage hwExport void hwShaderRelease(hwHShader sid)
{
    if (auto ctx = hwGetContext()) {
        ctx->shaderRelease(sid);
    }
}

hwCLinkage hwExport void hwShaderReload(hwHShader sid)
{
    if (auto ctx = hwGetContext()) {
        ctx->shaderReload(sid);
    }
}


hwCLinkage hwExport hwHAsset hwAssetLoadFromFile(const char *path, const hwConversionSettings *conv)
{
    if (path == nullptr || path[0]=='\0') { return hwNullHandle; }
    if (auto ctx = hwGetContext()) {
        return ctx->assetLoadFromFile(path, *conv);
    }
    return hwNullHandle;
}
hwCLinkage hwExport void hwAssetRelease(hwHAsset aid)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetRelease(aid);
    }
}

hwCLinkage hwExport void hwAssetReload(hwHAsset aid)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetReload(aid);
    }
}

hwCLinkage hwExport int hwAssetGetNumBones(hwHAsset aid)
{
    if (auto ctx = hwGetContext()) {
        return ctx->assetGetNumBones(aid);
    }
    return 0;
}

hwCLinkage hwExport const char* hwAssetGetBoneName(hwHAsset aid, int nth)
{
    if (auto ctx = hwGetContext()) {
        return ctx->assetGetBoneName(aid, nth);
    }
    return nullptr;
}

hwCLinkage hwExport void hwAssetGetBoneIndices(hwHAsset aid, hwFloat4 &o_indices)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetGetBoneIndices(aid, o_indices);
    }
}

hwCLinkage hwExport void hwAssetGetBoneWeights(hwHAsset aid, hwFloat4 &o_weight)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetGetBoneWeights(aid, o_weight);
    }
}

hwCLinkage hwExport void hwAssetGetBindPose(hwHAsset aid, int nth, hwMatrix &o_mat)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetGetBindPose(aid, nth, o_mat);
    }
}

hwCLinkage hwExport void hwAssetGetDefaultDescriptor(hwHAsset aid, hwHairDescriptor &o_desc)
{
    if (auto ctx = hwGetContext()) {
        ctx->assetGetDefaultDescriptor(aid, o_desc);
    }
}


hwCLinkage hwExport hwHInstance hwInstanceCreate(hwHAsset aid)
{
    if (auto ctx = hwGetContext()) {
        return ctx->instanceCreate(aid);
    }
    return hwNullHandle;
}
hwCLinkage hwExport void hwInstanceRelease(hwHInstance iid)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceRelease(iid);
    }
}
hwCLinkage hwExport void hwInstanceGetDescriptor(hwHInstance iid, hwHairDescriptor *desc)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceGetDescriptor(iid, *desc);
    }
}
hwCLinkage hwExport void hwInstanceSetDescriptor(hwHInstance iid, const hwHairDescriptor *desc)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceSetDescriptor(iid, *desc);
    }
}
hwCLinkage hwExport void hwInstanceSetTexture(hwHInstance iid, hwTextureType type, hwTexture *tex)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceSetTexture(iid, type, tex);
    }
}
hwCLinkage hwExport void hwInstanceUpdateSkinningMatrices(hwHInstance iid, int num_bones, hwMatrix *matrices)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceUpdateSkinningMatrices(iid, num_bones, matrices);
    }
}
hwCLinkage hwExport void hwInstanceUpdateSkinningDQs(hwHInstance iid, int num_bones, hwDQuaternion *dqs)
{
    if (auto ctx = hwGetContext()) {
        ctx->instanceUpdateSkinningDQs(iid, num_bones, dqs);
    }
}


hwCLinkage hwExport void hwSetViewProjection(const hwMatrix *view, const hwMatrix *proj, float fov)
{
    if (auto ctx = hwGetContext()) {
        ctx->setViewProjection(*view, *proj, fov);
    }
}

hwCLinkage hwExport void hwSetRenderTarget(hwTexture *framebuffer, hwTexture *depthbuffer)
{
    if (auto ctx = hwGetContext()) {
        ctx->setRenderTarget(framebuffer, depthbuffer);
    }
}

hwCLinkage hwExport void hwSetShader(hwHShader sid)
{
    if (auto ctx = hwGetContext()) {
        ctx->setShader(sid);
    }
}

hwCLinkage hwExport void hwSetLights(int num_lights, const hwLightData *lights)
{
    if (auto ctx = hwGetContext()) {
        ctx->setLights(num_lights, lights);
    }
}

hwCLinkage hwExport void hwRender(hwHInstance iid)
{
    if (auto ctx = hwGetContext()) {
        ctx->render(iid);
    }
}

hwCLinkage hwExport void hwRenderShadow(hwHInstance iid)
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

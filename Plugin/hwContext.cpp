#include "pch.h"
#include "HairWorkdIntegration.h"
#include "hwContext.h"


hwContext::hwContext()
    : m_sdk(nullptr)
{

}

hwContext::~hwContext()
{
    finalize();
}

hwContext::operator bool() const
{
    return m_sdk != nullptr;
}

hwSDK* hwContext::getSDK() const
{
    return m_sdk;
}

bool hwContext::initialize(const char *path_to_dll, void *d3d11_device)
{
    if (m_sdk != nullptr) {
        return true;
    }

    m_sdk = GFSDK_LoadHairSDK(path_to_dll, GFSDK_HAIRWORKS_VERSION);
    if (m_sdk != nullptr) {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") succeeded.\n", path_to_dll);
    }
    else {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") failed.\n", path_to_dll);
        return false;
    }

    if (m_sdk->InitRenderResources((ID3D11Device*)d3d11_device) == GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("InitRenderResources() succeeded.\n");
    }
    else {
        hwDebugLog("InitRenderResources() failed.\n");
        finalize();
        return false;
    }

    return true;
}

bool hwContext::finalize()
{
    if (m_sdk != nullptr) {
        m_sdk->Release();
        m_sdk = nullptr;
        return true;
    }
    return false;
}


hwAssetID hwContext::loadAssetFromFile(const std::string &path)
{
    {
        auto i = m_assets.find(path);
        if (i != m_assets.end()) {
            ++i->second.ref_count;
            return i->second.id;
        }
    }

    hwAssetID aid = hwNullID;
    if (m_sdk->LoadHairAssetFromFile(path.c_str(), (GFSDK_HairAssetID*)&aid) == GFSDK_HAIR_RETURN_OK) {
        auto &v = m_assets[path];
        v.id = aid;
        ++v.ref_count;
        hwDebugLog("LoadHairAssetFromFile(\"%s\") succeeded.\n", path.c_str());
    }
    else {
        hwDebugLog("LoadHairAssetFromFile(\"%s\") failed.\n", path.c_str());
    }
    return aid;
}

bool hwContext::releaseAsset(hwAssetID aid)
{
    bool ret = false;
    auto i = std::find_if(
        m_assets.begin(), m_assets.end(),
        [=](const AssetCont::value_type &p) { return p.second.id == aid; });
    if (i != m_assets.end() && --i->second.ref_count==0) {
        ret = m_sdk->FreeHairAsset((GFSDK_HairAssetID)i->second.id) == GFSDK_HAIR_RETURN_OK;
        m_assets.erase(i);
    }

    if (ret){ hwDebugLog("FreeHairAsset(%d) succeeded.\n", aid); }
    else    { hwDebugLog("FreeHairAsset(%d) failed.\n", aid); }
    return ret;
}

hwInstanceID hwContext::createInstance(hwAssetID aid)
{
    hwInstanceID iid = hwNullID;
    if (m_sdk->CreateHairInstance((GFSDK_HairAssetID)aid, (GFSDK_HairInstanceID*)&iid) == GFSDK_HAIR_RETURN_OK) {
        m_instances.resize(std::max<int>(m_instances.size(), aid));
        m_instances[iid] = iid;
        hwDebugLog("CreateHairInstance(%d) succeeded.\n", aid);
    }
    else
    {
        hwDebugLog("CreateHairInstance(%d) failed.\n", aid);
    }
    return iid;
}

bool hwContext::releaseInstance(hwInstanceID iid)
{
    bool ret = false;
    if (iid < m_instances.size() && m_instances[iid]!=hwNullID) {
        ret = m_sdk->FreeHairInstance((GFSDK_HairInstanceID)m_instances[iid]) == GFSDK_HAIR_RETURN_OK;
        m_instances[iid] = hwNullID;
    }
    if (ret){ hwDebugLog("FreeHairInstance(%d) succeeded.\n", iid); }
    else    { hwDebugLog("FreeHairInstance(%d) failed.\n", iid); }
    return ret;
}


void hwContext::getDescriptor(hwInstanceID iid, hwHairDescriptor &desc) const
{
    m_sdk->CopyCurrentInstanceDescriptor((GFSDK_HairInstanceID)iid, desc);
}

void hwContext::setDescriptor(hwInstanceID iid, const hwHairDescriptor &desc)
{
    m_sdk->UpdateInstanceDescriptor((GFSDK_HairInstanceID)iid, desc);
}

void hwContext::updateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices)
{
    m_sdk->UpdateSkinningMatrices((GFSDK_HairInstanceID)iid, num_matrices, (const gfsdk_float4x4*)matrices);
}


void hwContext::setRenderTarget(void *framebuffer, void *depthbuffer)
{
    // todo
}

void hwContext::setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov)
{
    m_sdk->SetViewProjection((const gfsdk_float4x4*)&view, (const gfsdk_float4x4*)&proj, GFSDK_HAIR_RIGHT_HANDED, fov);
}

void hwContext::render(hwInstanceID iid)
{
    m_sdk->RenderHairs((GFSDK_HairInstanceID)iid);
}

void hwContext::renderShadow(hwInstanceID iid)
{
    auto settings = GFSDK_HairShaderSettings(false, true);
    m_sdk->RenderHairs((GFSDK_HairInstanceID)iid, &settings);
}

void hwContext::stepSimulation(float dt)
{
    m_sdk->StepSimulation(dt);
}


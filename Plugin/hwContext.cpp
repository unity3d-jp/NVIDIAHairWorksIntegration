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
        hwDebugLog("hwContext::initialize(): GFSDK_LoadHairSDK() succeeded. (%s)\n", path_to_dll);
    }
    else {
        hwDebugLog("hwContext::initialize(): GFSDK_LoadHairSDK() failed. (%s)\n", path_to_dll);
        return false;
    }

    if (m_sdk->InitRenderResources((ID3D11Device*)d3d11_device) == GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("hwContext::initialize(): InitRenderResources() succeeded.\n");
    }
    else {
        hwDebugLog("hwContext::initialize(): InitRenderResources() failed.\n");
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
            return i->second;
        }
    }

    hwAssetID aid = hwNullID;
    if (m_sdk->LoadHairAssetFromFile(path.c_str(), (GFSDK_HairAssetID*)&aid) == GFSDK_HAIR_RETURN_OK) {
        m_assets[path] = aid;
        hwDebugLog("hwContext::loadAssetFromFile(): LoadHairAssetFromFile() succeeded. (%s)\n", path.c_str());
    }
    else {
        hwDebugLog("hwContext::loadAssetFromFile(): LoadHairAssetFromFile() failed. (%s)\n", path.c_str());
    }
    return aid;
}

bool hwContext::deleteAsset(hwAssetID aid)
{
    bool ret = false;
    auto i = std::find_if(
        m_assets.begin(), m_assets.end(),
        [=](const AssetCont::value_type &p) { return p.second == aid; });
    if (i != m_assets.end()) {
        ret = m_sdk->FreeHairAsset((GFSDK_HairAssetID)i->second) == GFSDK_HAIR_RETURN_OK;
        m_assets.erase(i);
    }
    return ret;
}

hwInstanceID hwContext::createInstance(hwAssetID asset_id)
{
    hwInstanceID iid = hwNullID;
    if (m_sdk->CreateHairInstance((GFSDK_HairAssetID)asset_id, (GFSDK_HairInstanceID*)&iid) == GFSDK_HAIR_RETURN_OK) {
        m_instances.resize(std::max<int>(m_instances.size(), iid));
        m_instances[iid] = iid;
    }
    return iid;
}

bool hwContext::deleteInstance(hwInstanceID instance_id)
{
    bool ret = false;
    if (instance_id < m_instances.size() && m_instances[instance_id] != hwNullID) {
        ret = m_sdk->FreeHairInstance((GFSDK_HairInstanceID)m_instances[instance_id]) == GFSDK_HAIR_RETURN_OK;
        m_instances[instance_id] = hwNullID;
    }
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



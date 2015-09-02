#include "pch.h"
#include "HairWorkdIntegration.h"
#include "hwContext.h"


bool hwFileToString(std::string &o_buf, const char *path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) { return false; }
    f.seekg(0, std::ios::end);
    o_buf.resize(f.tellg());
    f.seekg(0, std::ios::beg);
    f.read(&o_buf[0], o_buf.size());
    return true;
}



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
    if (path_to_dll == nullptr || d3d11_device == nullptr) { return false; }
    if (m_sdk != nullptr) { return true; }

    m_sdk = GFSDK_LoadHairSDK(path_to_dll, GFSDK_HAIRWORKS_VERSION);
    if (m_sdk != nullptr) {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") succeeded.\n", path_to_dll);
    }
    else {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") failed.\n", path_to_dll);
        return false;
    }

    if (m_sdk->InitRenderResources((ID3D11Device*)d3d11_device) == GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::InitRenderResources() succeeded.\n");
    }
    else {
        hwDebugLog("GFSDK_HairSDK::InitRenderResources() failed.\n");
        finalize();
        return false;
    }

    m_d3ddev = (ID3D11Device*)d3d11_device;
    m_d3ddev->GetImmediateContext(&m_d3dctx);

    return true;
}

void hwContext::finalize()
{
    for (auto &i : m_instances) { instanceRelease(i.id); }
    m_instances.clear();

    for (auto &i : m_assets) { assetRelease(i.id); }
    m_assets.clear();

    for (auto &i : m_shaders) { shaderRelease(i.id); }
    m_shaders.clear();

    if (m_sdk) {
        m_sdk->Release();
        m_sdk = nullptr;
    }
    if (m_d3dctx)
    {
        m_d3dctx->Release();
        m_d3dctx = nullptr;
    }

}



hwShaderID hwContext::shaderLoadFromFile(const std::string &path)
{
    {
        auto i = std::find_if(m_shaders.begin(), m_shaders.end(), [&](const ShaderHolder &v) { return v.path == path; });
        if (i != m_shaders.end() && i->ref_count > 0) {
            ++i->ref_count;
            return i->id;
        }
    }

    std::string bin;
    if (!hwFileToString(bin, path.c_str())) {
        hwDebugLog("failed to load shader (%s)\n", path.c_str());
        return hwNullID;
    }

    ShaderHolder tmp;
    tmp.path = path;
    tmp.id = m_shaders.size();
    ++tmp.ref_count;
    if (SUCCEEDED(m_d3ddev->CreatePixelShader(&bin[0], bin.size(), nullptr, &tmp.shader))) {
        m_shaders.push_back(tmp);
        hwDebugLog("CreatePixelShader(%s) : %d succeeded.\n", path.c_str(), tmp.id);
        return (hwShaderID)tmp.id;
    }
    else {
        hwDebugLog("CreatePixelShader(%s) failed.\n", path.c_str());
    }
    return hwNullID;
}

void hwContext::shaderRelease(hwShaderID sid)
{
    if (sid >= m_shaders.size()) { return; }

    auto &v = m_shaders[sid];
    if (v.ref_count > 0 && --v.ref_count == 0) {
        v.shader->Release();
        v = ShaderHolder();
        hwDebugLog("shaderRelease(%d)\n", sid);
    }
}


hwAssetID hwContext::assetLoadFromFile(const std::string &path)
{
    {
        auto i = std::find_if(m_assets.begin(), m_assets.end(), [&](const AssetHolder &v) { return v.path == path; });
        if (i != m_assets.end() && i->ref_count > 0) {
            ++i->ref_count;
            return i->id;
        }
    }

    hwAssetID aid = hwNullID;
    if (m_sdk->LoadHairAssetFromFile(path.c_str(), (GFSDK_HairAssetID*)&aid) == GFSDK_HAIR_RETURN_OK) {
        m_instances.resize(std::max<int>(m_instances.size(), aid));
        auto &v = m_assets[aid];
        v.path = path;
        v.id = aid;
        ++v.ref_count;
        hwDebugLog("GFSDK_HairSDK::LoadHairAssetFromFile(\"%s\") : %d succeeded.\n", path.c_str(), aid);
    }
    else {
        hwDebugLog("GFSDK_HairSDK::LoadHairAssetFromFile(\"%s\") failed.\n", path.c_str());
    }
    return aid;
}

void hwContext::assetRelease(hwAssetID aid)
{
    if (aid >= m_assets.size()) { return; }

    auto &v = m_assets[aid];
    if (v.ref_count > 0 && --v.ref_count==0) {
        if (m_sdk->FreeHairAsset((GFSDK_HairAssetID)aid) == GFSDK_HAIR_RETURN_OK) {
            hwDebugLog("GFSDK_HairSDK::FreeHairAsset(%d) succeeded.\n", aid);
        }
        else {
            hwDebugLog("GFSDK_HairSDK::FreeHairAsset(%d) failed.\n", aid);
        }
        v = AssetHolder();
    }
}

hwInstanceID hwContext::instanceCreate(hwAssetID aid)
{
    hwInstanceID iid = hwNullID;
    if (m_sdk->CreateHairInstance((GFSDK_HairAssetID)aid, (GFSDK_HairInstanceID*)&iid) == GFSDK_HAIR_RETURN_OK) {
        m_instances.resize(std::max<int>(m_instances.size(), iid));
        auto &v = m_instances[iid];
        v.id = iid;
        v.aid = aid;
        hwDebugLog("GFSDK_HairSDK::CreateHairInstance(%d) : %d succeeded.\n", aid, iid);
    }
    else
    {
        hwDebugLog("GFSDK_HairSDK::CreateHairInstance(%d) failed.\n", aid);
    }
    return iid;
}

void hwContext::instanceRelease(hwInstanceID iid)
{
    if (iid >= m_instances.size()) { return; }

    auto &v = m_instances[iid];
    if (v.id == iid) {
        if (m_sdk->FreeHairInstance((GFSDK_HairInstanceID)iid) == GFSDK_HAIR_RETURN_OK) {
            hwDebugLog("GFSDK_HairSDK::FreeHairInstance(%d) succeeded.\n", iid);
        }
        else {
            hwDebugLog("GFSDK_HairSDK::FreeHairInstance(%d) failed.\n", iid);
            v = InstanceHolder();
        }
    }
}


void hwContext::instanceGetDescriptor(hwInstanceID iid, hwHairDescriptor &desc) const
{
    if (m_sdk->CopyCurrentInstanceDescriptor((GFSDK_HairInstanceID)iid, desc) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::CopyCurrentInstanceDescriptor(%d) failed.\n", iid);
    }
}

void hwContext::instanceSetDescriptor(hwInstanceID iid, const hwHairDescriptor &desc)
{
    if (m_sdk->UpdateInstanceDescriptor((GFSDK_HairInstanceID)iid, desc) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::UpdateInstanceDescriptor(%d) failed.\n", iid);
    }
}

void hwContext::instanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices)
{
    if (m_sdk->UpdateSkinningMatrices((GFSDK_HairInstanceID)iid, num_matrices, (const gfsdk_float4x4*)matrices) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::UpdateSkinningMatrices(%d) failed.\n", iid);
    }
}


void hwContext::setRenderTarget(void *framebuffer, void *depthbuffer)
{
    // todo
}

void hwContext::setShader(hwShaderID sid)
{
    if (sid >= m_shaders.size()) { return; }

    auto &v = m_shaders[sid];
    if (v.shader) {
        m_d3dctx->PSSetShader(v.shader, nullptr, 0);
    }
}

void hwContext::setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov)
{
    if (m_sdk->SetViewProjection((const gfsdk_float4x4*)&view, (const gfsdk_float4x4*)&proj, GFSDK_HAIR_RIGHT_HANDED, fov) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::SetViewProjection() failed.\n");
    }
}

void hwContext::render(hwInstanceID iid)
{
    if (m_sdk->RenderHairs((GFSDK_HairInstanceID)iid) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::RenderHairs(%d) failed.\n", iid);
    }
}

void hwContext::renderShadow(hwInstanceID iid)
{
    auto settings = GFSDK_HairShaderSettings(false, true);
    if (m_sdk->RenderHairs((GFSDK_HairInstanceID)iid, &settings) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::RenderHairs(%d) failed.\n", iid);
    }
}

void hwContext::stepSimulation(float dt)
{
    if (m_sdk->StepSimulation(dt) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::StepSimulation(%f) failed.\n", dt);
    }
}


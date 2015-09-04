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

bool hwContext::initialize(const char *path_to_dll, hwDevice *d3d_device)
{
    if (path_to_dll == nullptr || d3d_device == nullptr) { return false; }
    if (m_sdk != nullptr) { return true; }

    m_sdk = GFSDK_LoadHairSDK(path_to_dll, GFSDK_HAIRWORKS_VERSION);
    if (m_sdk != nullptr) {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") succeeded.\n", path_to_dll);
    }
    else {
        hwDebugLog("GFSDK_LoadHairSDK(\"%s\") failed.\n", path_to_dll);
        return false;
    }

    if (m_sdk->InitRenderResources((ID3D11Device*)d3d_device) == GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::InitRenderResources() succeeded.\n");
    }
    else {
        hwDebugLog("GFSDK_HairSDK::InitRenderResources() failed.\n");
        finalize();
        return false;
    }

    m_d3ddev = (ID3D11Device*)d3d_device;
    m_d3ddev->GetImmediateContext(&m_d3dctx);
    if (m_sdk->SetCurrentContext(m_d3dctx) == GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::SetCurrentContext() succeeded.\n");
    }
    else {
        hwDebugLog("GFSDK_HairSDK::SetCurrentContext() failed.\n");
        finalize();
        return false;
    }

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

void hwContext::shaderReload(hwShaderID sid)
{
    if (sid >= m_shaders.size()) { return; }

    auto &v = m_shaders[sid];
    if (v.shader) {
        v.shader->Release();
        v.shader = nullptr;
    }

    std::string bin;
    if (!hwFileToString(bin, v.path.c_str())) {
        hwDebugLog("failed to reload shader (%s)\n", v.path.c_str());
        return;
    }
    if (SUCCEEDED(m_d3ddev->CreatePixelShader(&bin[0], bin.size(), nullptr, &v.shader))) {
        hwDebugLog("CreatePixelShader(%s) : %d reloaded.\n", v.path.c_str(), v.id);
    }
    else {
        hwDebugLog("CreatePixelShader(%s) failed to reload.\n", v.path.c_str());
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
    GFSDK_HairConversionSettings conversionSettings;
    {
        conversionSettings.m_targetHandednessHint = GFSDK_HAIR_RIGHT_HANDED;
        conversionSettings.m_targetUpAxisHint = GFSDK_HAIR_Y_UP;
        conversionSettings.m_targetSceneUnit = 0.0f;
    }
    if (m_sdk->LoadHairAssetFromFile(path.c_str(), (GFSDK_HairAssetID*)&aid, nullptr, &conversionSettings) == GFSDK_HAIR_RETURN_OK) {
        m_assets.resize(std::max<int>(m_assets.size(), aid + 1));
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

void hwContext::assetReload(hwAssetID aid)
{
    // todo
}

int hwContext::assetGetNumBones(hwAssetID aid) const
{
    uint32_t r = 0;
    if (m_sdk->GetNumBones((GFSDK_HairAssetID)aid, &r) != GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::GetNumBones(%d) failed.\n", aid);
    }
    return r;
}

const char* hwContext::assetGetBoneName(hwAssetID aid, int nth) const
{
    static char tmp[256];
    if (m_sdk->GetBoneName((GFSDK_HairAssetID)aid, nth, tmp) != GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::GetBoneName(%d) failed.\n", aid);
    }
    return tmp;
}

void hwContext::assetGetBoneIndices(hwAssetID aid, hwFloat4 &o_indices) const
{
    if (m_sdk->GetBoneIndices((GFSDK_HairAssetID)aid, &o_indices) != GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::GetBoneIndices(%d) failed.\n", aid);
    }
}

void hwContext::assetGetBoneWeights(hwAssetID aid, hwFloat4 &o_waits) const
{
    if (m_sdk->GetBoneWeights((GFSDK_HairAssetID)aid, &o_waits) != GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::GetBoneWeights(%d) failed.\n", aid);
    }
}

void hwContext::assetGetDefaultDescriptor(hwAssetID aid, hwHairDescriptor &o_desc) const
{
    if (m_sdk->CopyInstanceDescriptorFromAsset((GFSDK_HairAssetID)aid, o_desc) != GFSDK_HAIR_RETURN_OK) {
        hwDebugLog("GFSDK_HairSDK::CopyInstanceDescriptorFromAsset(%d) failed.\n", aid);
    }
}



hwInstanceID hwContext::instanceCreate(hwAssetID aid)
{
    hwInstanceID iid = hwNullID;
    if (m_sdk->CreateHairInstance((GFSDK_HairAssetID)aid, (GFSDK_HairInstanceID*)&iid) == GFSDK_HAIR_RETURN_OK) {
        m_instances.resize(std::max<int>(m_instances.size(), iid + 1));
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

void hwContext::instanceSetTexture(hwInstanceID iid, hwTextureType type, hwTexture *tex)
{
    auto *srv = getSRV(tex);
    if (!srv || m_sdk->SetTextureSRV((GFSDK_HairInstanceID)iid, type, srv) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::SetTextureSRV(%d, %d) failed.\n", iid, type);
    }
}

void hwContext::instanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices)
{
    if (m_sdk->UpdateSkinningMatrices((GFSDK_HairInstanceID)iid, num_matrices, (const gfsdk_float4x4*)matrices) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::UpdateSkinningMatrices(%d) failed.\n", iid);
    }
}



template<class T>
void hwContext::pushDrawCommand(const T &c)
{
    const char *begin = (const char*)&c;
    m_commands.insert(m_commands.end(), begin, begin+sizeof(T));
}

void hwContext::setRenderTarget(hwTexture *framebuffer, hwTexture *depthbuffer)
{
    DrawCommandRT c = { CID_SetRenderTarget, framebuffer, depthbuffer };
    pushDrawCommand(c);
}

void hwContext::setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov)
{
    DrawCommandVP c = { CID_SetViewProjection, fov, view, proj };
    pushDrawCommand(c);
}

void hwContext::setShader(hwShaderID sid)
{
    if (sid == hwNullID) { return; }
    DrawCommand c = { CID_SetShader, sid };
    pushDrawCommand(c);
}

void hwContext::render(hwInstanceID iid)
{
    if (iid == hwNullID) { return; }
    DrawCommand c = { CID_Render, iid };
    pushDrawCommand(c);
}

void hwContext::renderShadow(hwInstanceID iid)
{
    if (iid == hwNullID) { return; }
    DrawCommand c = { CID_RenderShadow, iid};
    pushDrawCommand(c);
}


hwSRV* hwContext::getSRV(hwTexture *tex)
{
    {
        auto i = m_srvtable.find(tex);
        if (i != m_srvtable.end()) {
            return i->second;
        }
    }

    hwSRV *ret = nullptr;
    if (SUCCEEDED(m_d3ddev->CreateShaderResourceView(tex, nullptr, &ret))) {
        m_srvtable[tex] = ret;
    }
    return ret;
}

hwRTV* hwContext::getRTV(hwTexture *tex)
{
    {
        auto i = m_rtvtable.find(tex);
        if (i != m_rtvtable.end()) {
            return i->second;
        }
    }

    hwRTV *ret = nullptr;
    if (SUCCEEDED(m_d3ddev->CreateRenderTargetView(tex, nullptr, &ret))) {
        m_rtvtable[tex] = ret;
    }
    return ret;
}


void hwContext::setRenderTargetImpl(hwTexture *framebuffer, hwTexture *depthbuffer)
{
    // todo
}

void hwContext::setViewProjectionImpl(const hwMatrix &view, const hwMatrix &proj, float fov)
{
    if (m_sdk->SetViewProjection((const gfsdk_float4x4*)&view, (const gfsdk_float4x4*)&proj, GFSDK_HAIR_RIGHT_HANDED, fov) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::SetViewProjection() failed.\n");
    }
}

void hwContext::setShaderImpl(hwShaderID sid)
{
    if (sid >= m_shaders.size()) { return; }

    auto &v = m_shaders[sid];
    if (v.shader) {
        m_d3dctx->PSSetShader(v.shader, nullptr, 0);
    }
}

void hwContext::renderImpl(hwInstanceID iid)
{
    ID3D11ShaderResourceView* ppSRV[GFSDK_HAIR_NUM_SHADER_RESOUCES];
    m_sdk->GetShaderResources((GFSDK_HairInstanceID)iid, ppSRV);
    m_d3dctx->PSSetShaderResources(0, GFSDK_HAIR_NUM_SHADER_RESOUCES, ppSRV);
    if (m_sdk->RenderHairs((GFSDK_HairInstanceID)iid) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::RenderHairs(%d) failed.\n", iid);
    }
}

void hwContext::renderShadowImpl(hwInstanceID iid)
{
    auto settings = GFSDK_HairShaderSettings(false, true);
    ID3D11ShaderResourceView* ppSRV[GFSDK_HAIR_NUM_SHADER_RESOUCES];
    m_sdk->GetShaderResources((GFSDK_HairInstanceID)iid, ppSRV);
    m_d3dctx->PSSetShaderResources(0, GFSDK_HAIR_NUM_SHADER_RESOUCES, ppSRV);
    if (m_sdk->RenderHairs((GFSDK_HairInstanceID)iid, &settings) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::RenderHairs(%d) failed.\n", iid);
    }
}

void hwContext::flush()
{
    for (int i=0; i<m_commands.size(); ) {
        CommandID cid = (CommandID&)m_commands[i];
        switch (cid) {
        case CID_SetViewProjection:
        {
            const auto c = (DrawCommandVP&)m_commands[i];
            setViewProjectionImpl(c.view, c.proj, c.fov);
            i += sizeof(c);
            break;
        }
        case CID_SetRenderTarget:
        {
            const auto c = (DrawCommandRT&)m_commands[i];
            setRenderTargetImpl(c.framebuffer, c.depthbuffer);
            i += sizeof(c);
            break;
        }
        case CID_SetShader:
        {
            const auto c = (DrawCommand&)m_commands[i];
            setShaderImpl(c.arg);
            i += sizeof(c);
            break;
        }
        case CID_Render:
        {
            const auto c = (DrawCommand&)m_commands[i];
            renderImpl(c.arg);
            i += sizeof(c);
            break;
        }
        case CID_RenderShadow:
        {
            const auto c = (DrawCommand&)m_commands[i];
            renderShadowImpl(c.arg);
            i += sizeof(c);
            break;
        }
        }
    }
    m_commands.clear();
}

void hwContext::stepSimulation(float dt)
{
    if (m_sdk->StepSimulation(dt) != GFSDK_HAIR_RETURN_OK)
    {
        hwDebugLog("GFSDK_HairSDK::StepSimulation(%f) failed.\n", dt);
    }
}


#ifndef hwContext_h
#define hwContext_h

struct hwShaderData
{
    hwShaderID id;
    int ref_count;
    ID3D11PixelShader *shader;
    std::string path;

    hwShaderData() : id(hwNullID), ref_count(0), shader(nullptr) {}
};

struct hwAssetData
{
    hwAssetID id;
    int ref_count;
    std::string path;

    hwAssetData() : id(hwNullID), ref_count(0) {}
};

struct hwInstanceData
{
    hwInstanceID id;
    hwAssetID aid;
    bool cast_shadow;
    bool receive_shadow;

    hwInstanceData() : id(hwNullID), aid(hwNullID), cast_shadow(), receive_shadow() {}
};

enum hwELightType
{
    hwELightType_Directional,
    hwELightType_Point,
};

struct hwLightData
{
    hwELightType type; int pad[3];
    hwFloat3 position; // direction if directional light, position if point light
    float range;
    hwFloat4 color;

    hwLightData()
        : type(hwELightType_Directional)
        , position({ 0.0f, 0.0f, 0.0f })
        , range(0.0f)
        , color({ 1.0f, 1.0f, 1.0f, 1.0 })
    {}
};

struct hwConstantBuffer
{
    int num_lights; int pad0[3];
    hwLightData lights[hwMaxLights];
    GFSDK_HairShaderConstantBuffer hw;

    hwConstantBuffer() : num_lights(0) {}
};



class hwContext
{
public:
    // メインスレッドと Unity のレンダリングスレッドは別である可能性があるため、
    // 外に見せる描画系関数はコマンドを積むだけにして、UnityRenderEvent() でそれを flush するという形式をとる。
    enum CommandID
    {
        CID_SetViewProjection,
        CID_SetRenderTarget,
        CID_SetShader,
        CID_SetLights,
        CID_Render,
        CID_RenderShadow,
    };
    struct DrawCommandVP
    {
        CommandID command;
        float fov;
        hwMatrix view;
        hwMatrix proj;
    };
    struct DrawCommandRT
    {
        CommandID command;
        hwTexture *framebuffer;
        hwTexture *depthbuffer;
    };
    struct DrawCommandL
    {
        CommandID command;
        int num_lights;
        hwLightData lights[hwMaxLights];
    };
    struct DrawCommand
    {
        CommandID command;
        int arg;
    };



public:
    hwContext();
    ~hwContext();
    bool valid() const;

    bool initialize(const char *path_to_dll, hwDevice *d3d_device);
    void finalize();
    void move(hwContext &from);

    hwShaderID      shaderLoadFromFile(const std::string &path);
    void            shaderRelease(hwShaderID sid);
    void            shaderReload(hwShaderID sid);

    hwAssetID       assetLoadFromFile(const std::string &path, const hwConversionSettings &conv);
    void            assetRelease(hwAssetID aid);
    void            assetReload(hwAssetID aid);
    int             assetGetNumBones(hwAssetID aid) const;
    const char*     assetGetBoneName(hwAssetID aid, int nth) const;
    void            assetGetBoneIndices(hwAssetID aid, hwFloat4 &o_indices) const;
    void            assetGetBoneWeights(hwAssetID aid, hwFloat4 &o_waits) const;
    void            assetGetDefaultDescriptor(hwAssetID aid, hwHairDescriptor &o_desc) const;

    hwInstanceID    instanceCreate(hwAssetID aid);
    void            instanceRelease(hwInstanceID iid);
    void            instanceGetDescriptor(hwInstanceID iid, hwHairDescriptor &desc) const;
    void            instanceSetDescriptor(hwInstanceID iid, const hwHairDescriptor &desc);
    void            instanceSetTexture(hwInstanceID iid, hwTextureType type, hwTexture *tex);
    void            instanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices);

    void setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov);
    void setRenderTarget(hwTexture *framebuffer, hwTexture *depthbuffer);
    void setShader(hwShaderID sid);
    void setLights(int num_lights, const hwLightData *lights);
    void render(hwInstanceID iid);
    void renderShadow(hwInstanceID iid);
    void flush();
    void stepSimulation(float dt);

private:
    template<class T> void pushDrawCommand(const T &c);
    void setViewProjectionImpl(const hwMatrix &view, const hwMatrix &proj, float fov);
    void setRenderTargetImpl(hwTexture *framebuffer, hwTexture *depthbuffer);
    void setShaderImpl(hwShaderID sid);
    void setLightsImpl(int num_lights, const hwLightData *lights);
    void renderImpl(hwInstanceID iid);
    void renderShadowImpl(hwInstanceID iid);
    hwSRV* getSRV(hwTexture *tex);
    hwRTV* getRTV(hwTexture *tex);

private:
    typedef std::vector<hwShaderData>       ShaderCont;
    typedef std::vector<hwAssetData>        AssetCont;
    typedef std::vector<hwInstanceData>     InstanceCont;
    typedef std::map<hwTexture*, hwSRV*>    SRVTable;
    typedef std::map<hwTexture*, hwRTV*>    RTVTable;
    typedef std::vector<char>               DrawCommands;

    ID3D11Device            *m_d3ddev;
    ID3D11DeviceContext     *m_d3dctx;
    hwSDK                   *m_sdk;
    ShaderCont              m_shaders;
    AssetCont               m_assets;
    InstanceCont            m_instances;
    SRVTable                m_srvtable;
    RTVTable                m_rtvtable;
    DrawCommands            m_commands;

    ID3D11DepthStencilState *m_rs_enable_depth;
    ID3D11Buffer            *m_rs_constant_buffer;

    hwConstantBuffer        m_cb;
};

#endif // hwContext_h

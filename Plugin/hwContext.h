#ifndef hwContext_h
#define hwContext_h


class hwContext
{
public:
    struct ShaderHolder
    {
        std::string path;
        hwShaderID id;
        int ref_count;
        ID3D11PixelShader *shader;
        ShaderHolder() : id(hwNullID), ref_count(0), shader(nullptr) {}
    };

    struct AssetHolder
    {
        std::string path;
        hwAssetID id;
        int ref_count;
        AssetHolder() : id(hwNullID), ref_count(0) {}
    };

    struct InstanceHolder
    {
        hwInstanceID id;
        hwAssetID aid;
        InstanceHolder() : id(hwNullID), aid(hwNullID) {}
    };


    // メインスレッドと Unity のレンダリングスレッドは別である可能性があるため、
    // 外に見せる描画系関数はコマンドを積むだけにして、UnityRenderEvent() でそれを flush するという形式をとる。
    enum CommandID
    {
        CID_SetViewProjection,
        CID_SetRenderTarget,
        CID_SetShader,
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
    struct DrawCommand
    {
        CommandID command;
        int arg;
    };



public:
    hwContext();
    ~hwContext();

    operator bool() const;
    hwSDK* getSDK() const;

    bool initialize(const char *path_to_dll, hwDevice *d3d_device);
    void finalize();

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
    void render(hwInstanceID iid);
    void renderShadow(hwInstanceID iid);
    void flush();
    void stepSimulation(float dt);

private:
    template<class T> void pushDrawCommand(const T &c);
    void setViewProjectionImpl(const hwMatrix &view, const hwMatrix &proj, float fov);
    void setRenderTargetImpl(hwTexture *framebuffer, hwTexture *depthbuffer);
    void setShaderImpl(hwShaderID sid);
    void renderImpl(hwInstanceID iid);
    void renderShadowImpl(hwInstanceID iid);
    hwSRV* getSRV(hwTexture *tex);
    hwRTV* getRTV(hwTexture *tex);

private:
    typedef std::vector<ShaderHolder>       ShaderCont;
    typedef std::vector<AssetHolder>        AssetCont;
    typedef std::vector<InstanceHolder>     InstanceCont;
    typedef std::vector<char>               DrawCommands;
    typedef std::map<hwTexture*, hwSRV*>    SRVTable;
    typedef std::map<hwTexture*, hwRTV*>    RTVTable;

    ID3D11Device        *m_d3ddev;
    ID3D11DeviceContext *m_d3dctx;
    hwSDK               *m_sdk;
    ShaderCont          m_shaders;
    AssetCont           m_assets;
    InstanceCont        m_instances;
    DrawCommands        m_commands;
    SRVTable            m_srvtable;
    RTVTable            m_rtvtable;
};

#endif // hwContext_h

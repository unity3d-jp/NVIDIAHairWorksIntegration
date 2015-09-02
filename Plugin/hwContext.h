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


public:
    hwContext();
    ~hwContext();

    operator bool() const;
    hwSDK* getSDK() const;

    bool initialize(const char *path_to_dll, void *d3d11_device);
    void finalize();

    hwShaderID      shaderLoadFromFile(const std::string &path);
    void            shaderRelease(hwShaderID aid);

    hwAssetID       assetLoadFromFile(const std::string &path);
    void            assetRelease(hwAssetID aid);

    hwInstanceID    instanceCreate(hwAssetID aid);
    void            instanceRelease(hwInstanceID iid);
    void            instanceGetDescriptor(hwInstanceID iid, hwHairDescriptor &desc) const;
    void            instanceSetDescriptor(hwInstanceID iid, const hwHairDescriptor &desc);
    void            instanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices);

    void setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov);
    void setRenderTarget(void *framebuffer, void *depthbuffer);
    void setShader(hwShaderID sid);
    void render(hwInstanceID iid);
    void renderShadow(hwInstanceID iid);
    void stepSimulation(float dt);


private:
    typedef std::vector<ShaderHolder>   ShaderCont;
    typedef std::vector<AssetHolder>    AssetCont;
    typedef std::vector<InstanceHolder> InstanceCont;

    ID3D11Device        *m_d3ddev;
    ID3D11DeviceContext *m_d3dctx;
    hwSDK               *m_sdk;
    ShaderCont          m_shaders;
    AssetCont           m_assets;
    InstanceCont        m_instances;
    hwMatrix            m_view;
    hwMatrix            m_projection;
};

#endif // hwContext_h

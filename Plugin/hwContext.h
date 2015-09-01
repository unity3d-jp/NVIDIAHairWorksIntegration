#ifndef hwContext_h
#define hwContext_h


class hwContext
{
public:
    hwContext();
    ~hwContext();

    operator bool() const;
    hwSDK* getSDK() const;

    bool initialize(const char *path_to_dll, void *d3d11_device);
    bool finalize();

    hwAssetID       loadAssetFromFile(const std::string &path);
    bool            deleteAsset(hwAssetID aid);

    hwInstanceID    createInstance(hwAssetID aid);
    bool            deleteInstance(hwInstanceID iid);

    void getDescriptor(hwInstanceID iid, hwHairDescriptor &desc) const;
    void setDescriptor(hwInstanceID iid, const hwHairDescriptor &desc);
    void updateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices);

    void setRenderTarget(void *framebuffer, void *depthbuffer);
    void setViewProjection(const hwMatrix &view, const hwMatrix &proj, float fov);
    void render(hwInstanceID iid);
    void renderShadow(hwInstanceID iid);
    void stepSimulation(float dt);

private:
    typedef std::map<std::string, hwAssetID>    AssetCont;
    typedef std::vector<hwInstanceID>           InstanceCont;
    hwSDK           *m_sdk;
    AssetCont       m_assets;
    InstanceCont    m_instances;
    hwMatrix        m_view;
    hwMatrix        m_projection;
};

#endif // hwContext_h

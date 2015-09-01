#ifndef hwContext_h
#define hwContext_h


class hwContext
{
public:
    hwContext();
    ~hwContext();

    bool initialize(const char *path_to_dll);
    bool finalize();

    hwAssetID       loadAssetFromFile(const char *path);
    bool            deleteAsset(hwAssetID asset_id);

    hwInstanceID    createInstance(hwAssetID asset_id);
    bool            deleteInstance(hwInstanceID instance_id);
    hwInstance*     getInstance(hwInstanceID instance_id);

    void setViewProjectionMatrix(const hwMatrix &view, const hwMatrix &proj);
    void stepSimulation();
    void render();

    hwSDK* getSDK() const;

private:
    hwSDK                               *m_sdk;
    std::map<std::string, hwAssetID>    m_assets_table;
    std::vector<hwInstance*>            m_instances;
    hwMatrix                            m_view;
    hwMatrix                            m_projection;
};


class hwInstance
{
public:
    hwInstance(hwContext *ctx, hwInstanceID id);
    operator bool() const;
    hwInstanceID getID() const;

    const hwHairDescriptor& getDescriptor() const;
    void setDescriptor(const hwHairDescriptor &desc);
    void updateSkinningMatrices(int num_matrices, const hwMatrix *matrices);

private:
    hwContext *m_ctx;
    hwInstanceID m_id;
    hwHairDescriptor m_desc;
};

#endif // hwContext_h

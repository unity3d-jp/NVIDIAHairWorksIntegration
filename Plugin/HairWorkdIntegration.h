#ifndef HairWorksIntegration_h
#define HairWorksIntegration_h

typedef GFSDK_HairSDK hwSDK;
typedef GFSDK_HairAssetID hwAssetID;
typedef GFSDK_HairInstanceID hwInstanceID;
typedef GFSDK_HairInstanceDescriptor hwHairDescriptor;
typedef XMMATRIX hwMatrix;
#define hwNullID 0xFFFF

class hwContext;
class hwInstance;


hwCLinkage hwExport bool            hwInitialize(const char *path_to_dll);
hwCLinkage hwExport void            hwFinalize();
hwCLinkage hwExport hwContext*      hwGetContext();


hwCLinkage hwExport hwAssetID       hwLoadAssetFromFile(const char *path);
hwCLinkage hwExport bool            hwDeleteAsset(hwAssetID aid);

hwCLinkage hwExport hwInstanceID    hwCreateInstance(hwAssetID aid);
hwCLinkage hwExport bool            hwDeleteInstance(hwInstanceID iid);
hwCLinkage hwExport void            hwSetDescriptor(hwInstanceID iid, const hwHairDescriptor &desc);
hwCLinkage hwExport void            hwUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices);

hwCLinkage hwExport void            hwSetViewProjectionMatrix(const hwMatrix &view, const hwMatrix &proj);
hwCLinkage hwExport void            hwStepSimulation();
hwCLinkage hwExport void            hwRender(hwInstanceID iid);

#endif // HairWorksIntegration_h

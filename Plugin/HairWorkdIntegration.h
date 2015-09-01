#ifndef HairWorksIntegration_h
#define HairWorksIntegration_h

typedef GFSDK_HairSDK hwSDK;
//typedef GFSDK_HairAssetID hwAssetID;
//typedef GFSDK_HairInstanceID hwInstanceID;
typedef int hwAssetID;
typedef int hwInstanceID;
typedef GFSDK_HairInstanceDescriptor hwHairDescriptor;
typedef XMMATRIX hwMatrix;
#define hwNullID 0xFFFF

class hwContext;


// Unity plugin callbacks
hwCLinkage hwExport void            UnitySetGraphicsDevice(void* device, int deviceType, int eventType);
hwCLinkage hwExport void            UnityRenderEvent(int eventID);


hwCLinkage hwExport bool            hwInitialize(const char *path_to_dll);
hwCLinkage hwExport void            hwFinalize();
hwCLinkage hwExport hwContext*      hwGetContext();


hwCLinkage hwExport hwAssetID       hwLoadAssetFromFile(const char *path);
hwCLinkage hwExport bool            hwDeleteAsset(hwAssetID aid);

hwCLinkage hwExport hwInstanceID    hwCreateInstance(hwAssetID aid);
hwCLinkage hwExport bool            hwDeleteInstance(hwInstanceID iid);
hwCLinkage hwExport void            hwGetDescriptor(hwInstanceID iid, hwHairDescriptor *desc);
hwCLinkage hwExport void            hwSetDescriptor(hwInstanceID iid, const hwHairDescriptor *desc);
hwCLinkage hwExport void            hwUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, const hwMatrix *matrices);

hwCLinkage hwExport void            hwSetViewProjection(const hwMatrix *view, const hwMatrix *proj, float fov);
hwCLinkage hwExport void            hwSetRenderTarget(void *framebuffer, void *depthbuffer);
hwCLinkage hwExport void            hwRender(hwInstanceID iid);
hwCLinkage hwExport void            hwRenderShadow(hwInstanceID iid);
hwCLinkage hwExport void            hwStepSimulation(float dt);

#endif // HairWorksIntegration_h

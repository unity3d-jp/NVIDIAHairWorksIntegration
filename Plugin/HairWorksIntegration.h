#ifndef HairWorksIntegration_h
#define HairWorksIntegration_h

typedef GFSDK_HairSDK                   hwSDK;
typedef GFSDK_HairAssetID               hwAssetID;
typedef GFSDK_HairInstanceID            hwInstanceID;
typedef GFSDK_HairInstanceDescriptor    hwHairDescriptor;
typedef GFSDK_HairConversionSettings    hwConversionSettings;
typedef GFSDK_HAIR_TEXTURE_TYPE         hwTextureType;

typedef ID3D11Device                    hwDevice;
typedef ID3D11Texture2D                 hwTexture;
typedef ID3D11ShaderResourceView        hwSRV;
typedef ID3D11RenderTargetView          hwRTV;

typedef gfsdk_float3            hwFloat3;
typedef gfsdk_float4            hwFloat4;
typedef gfsdk_dualquaternion    hwDQuaternion;
typedef gfsdk_float4x4          hwMatrix;
typedef uint32_t                hwHShader;      // H stands for Handle
typedef uint32_t                hwHAsset;       // 
typedef uint32_t                hwHInstance;    // 

typedef void(__stdcall *hwLogCallback)(const char *);
#define hwNullAssetID       GFSDK_HairAssetID_NULL
#define hwNullInstanceID    GFSDK_HairInstanceID_NULL
#define hwNullHandle        0xFFFFFFFF
#define hwMaxLights         8


struct  hwShaderData;
struct  hwAssetData;
struct  hwInstanceData;
struct  hwLightData;
class   hwContext;


// Unity plugin callbacks
hwCLinkage hwExport bool            hwInitialize();
hwCLinkage hwExport void            hwFinalize();
hwCLinkage hwExport hwContext*      hwGetContext();
hwCLinkage hwExport int             hwGetFlushEventID();
hwCLinkage hwExport void            hwSetLogCallback(hwLogCallback cb);

hwCLinkage hwExport hwHShader       hwShaderLoadFromFile(const char *path);
hwCLinkage hwExport void            hwShaderRelease(hwHShader sid);
hwCLinkage hwExport void            hwShaderReload(hwHShader sid);

hwCLinkage hwExport hwHAsset        hwAssetLoadFromFile(const char *path, const hwConversionSettings *conv);
hwCLinkage hwExport void            hwAssetRelease(hwHAsset aid);
hwCLinkage hwExport void            hwAssetReload(hwHAsset aid);
hwCLinkage hwExport int             hwAssetGetNumBones(hwHAsset aid);
hwCLinkage hwExport const char*     hwAssetGetBoneName(hwHAsset aid, int nth);
hwCLinkage hwExport void            hwAssetGetBoneIndices(hwHAsset aid, hwFloat4 &o_indices);
hwCLinkage hwExport void            hwAssetGetBoneWeights(hwHAsset aid, hwFloat4 &o_weight);
hwCLinkage hwExport void            hwAssetGetBindPose(hwHAsset aid, int nth, hwMatrix &o_mat);
hwCLinkage hwExport void            hwAssetGetDefaultDescriptor(hwHAsset aid, hwHairDescriptor &o_desc);

hwCLinkage hwExport hwHInstance     hwInstanceCreate(hwHAsset aid);
hwCLinkage hwExport void            hwInstanceRelease(hwHInstance iid);
hwCLinkage hwExport void            hwInstanceGetAssetID(hwHInstance iid);
hwCLinkage hwExport void            hwInstanceGetBounds(hwHInstance iid, hwFloat3 *o_min, hwFloat3 *o_max);
hwCLinkage hwExport void            hwInstanceGetDescriptor(hwHInstance iid, hwHairDescriptor *o_desc);
hwCLinkage hwExport void            hwInstanceSetDescriptor(hwHInstance iid, const hwHairDescriptor *desc);
hwCLinkage hwExport void            hwInstanceSetTexture(hwHInstance iid, hwTextureType type, hwTexture *tex);
hwCLinkage hwExport void            hwInstanceUpdateSkinningMatrices(hwHInstance iid, int num_bones, hwMatrix *matrices);
hwCLinkage hwExport void            hwInstanceUpdateSkinningDQs(hwHInstance iid, int num_bones, hwDQuaternion *dqs);

hwCLinkage hwExport void            hwBeginScene();
hwCLinkage hwExport void            hwEndScene();
hwCLinkage hwExport void            hwSetViewProjection(const hwMatrix *view, const hwMatrix *proj, float fov);
hwCLinkage hwExport void            hwSetRenderTarget(hwTexture *framebuffer, hwTexture *depthbuffer);
hwCLinkage hwExport void            hwSetShader(hwHShader sid);
hwCLinkage hwExport void            hwSetLights(int num_lights, const hwLightData *lights);
hwCLinkage hwExport void            hwRender(hwHInstance iid);
hwCLinkage hwExport void            hwRenderShadow(hwHInstance iid);
hwCLinkage hwExport void            hwStepSimulation(float dt);

#endif // HairWorksIntegration_h

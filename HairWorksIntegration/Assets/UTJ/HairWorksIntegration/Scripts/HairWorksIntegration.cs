using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

public struct hwAssetID
{
    public int id;
}

public struct hwInstanceID
{
    public int id;
}

public unsafe struct hwDescriptor
{
    // global controls
    bool m_enable;                    //!< [true/false] whether to enable this hair. When disabled, hair will not cause any computation/rendering

    // hair geometry (density/width/length/clump) controls
    float m_width;                  //!< [In Millimeters] hair width (thickness)
    float m_widthNoise;             //!< [0 - 1.0] noise factor for hair width noise 
    float m_widthRootScale;         //!< [0 - 1.0] scale factor for top side of the strand
    float m_widthTipScale;          //!< [0 - 1.0] scale factor for bottom side of the strand

    float m_clumpNoise;             //!< [0 - 1.0] probability of each hair gets clumped (0 = all hairs get clumped, 1 = clump scale is randomly distributed from 0 to 1)
    float m_clumpRoundness;         //!< [0 - 2.0] exponential factor to control roundness of clump shape (0 = linear cone, clump scale *= power(t, roundness), where t is normalized distance from the root)
    float m_clumpScale;             //!< [0 - 1.0] how clumped each hair face is

    float m_density;                    //!< [0 -    ] hair density per face (1.0 = 64 hairs per face)
    bool m_usePixelDensity;           //!< [true/false] whether to use per-pixel sampling or per-vertex sampling for density map

    float m_lengthNoise;                //!< [0 - 1.0] length variation noise
    float m_lengthScale;                //!< [0 - 1.0] length control for growing hair effect

    float m_waveScale;              //!< [In Centimeters] size of waves for hair waviness 
    float m_waveScaleNoise;         //!< [0 - 1.0] noise factor for the wave scale
    float m_waveScaleClump;         //!< [0 - 1.0] waviness at clump level
    float m_waveScaleStrand;            //!< [0 - 1.0] waviness at strand level
    float m_waveFreq;                   //!< [0 -    ] wave frequency (1.0 = one sine wave along hair length)
    float m_waveFreqNoise;          //!< [0 - 1.0] noise factor for the wave frequency 
    float m_waveRootStraighten;     //!< [0 - 1.0] For some distance from the root, we atteunate waviness so that root itself does not move [0-1]

    /// shading controls
    float m_rootAlphaFalloff;           //!< [0 - 1.0] falloff factor for alpha transition from root 
    Vector4 m_rootColor;               //!< [0 - 1.0] color of hair root (when hair textures are not used)
    Vector4 m_tipColor;                    //!< [0 - 1.0] color of hair tip (when hair textures are not used)
    float m_rootTipColorWeight;     //!< [0 - 1.0] blend factor between root and tip color in addition to hair length
    float m_rootTipColorFalloff;        //!< [0 - 1.0] falloff factor for root/tip color interpolation

    float m_diffuseBlend;               //!< [0 - 1.0] blend factor between Kajiya hair lighting vs normal skin lighting.
    float m_hairNormalWeight;           //!< [0 - 1.0] blend factor between mesh normal vs hair normal. Use higher value for longer (surface like) hair.
    int m_hairNormalBoneIndex;        //!< [0 - number of bones] index for the bone which we use as model center for diffuse shading purpose

    Vector4 m_specularColor;           //!< [0 - 1.0] specular color
    float m_specularNoiseScale;     //!< [0 - 1.0] amount of specular noise
    float m_specularEnvScale;           //!< [0 - 1.0] amount of specular scale from env probe
    float m_specularPrimary;            //!< [0 - 1.0] primary specular factor
    float m_specularPowerPrimary;       //!< [0 - ] primary specular power exponent
    float m_specularPrimaryBreakup; //!< [0 - 1.0] shift factor to make specular highlight move with noise
    float m_specularSecondary;      //!< [0 - 1.0] secondary specular factor
    float m_specularSecondaryOffset;    //!< [0 - 1.0] secondary highlight shift offset along tangents
    float m_specularPowerSecondary; //!< [0 - ] secondary specular power exponent		

    float m_glintStrength;          //!< [0 - 1.0] strength of the glint noise
    float m_glintCount;             //!< [0 - 1024] number of glint sparklets along each hair
    float m_glintExponent;          //!< [0 - ] glint power exponent

    bool m_castShadows;               //!< [true/false] this hair cast shadows onto the scene
    bool m_receiveShadows;            //!< [true/false] this hair receives shadows from the scene
    float m_shadowSigma;                //!< [In Centimeters] distance through hair volume beyond which hairs get completely shadowed.

    int m_strandBlendMode;            //!< [GFSDK_HAIR_STRAND_BLEND_MODE] blend mode when strand texture is used. Supported mode are defined in GFSDK_HAIR_STRAND_BLEND_MODE.
    float m_strandBlendScale;           //!< [0 - 1.0] scale strand texture before blend

    // simulation control
    float m_backStopRadius;         //!< [0 - 1.0] radius of backstop collision (normalized along hair length)
    float m_bendStiffness;          //!< [0 - 1.0] stiffness for bending, useful for long hair
    float m_damping;                    //!< [0 - ] damping to slow down hair motion
    Vector3 m_gravityDir;              //!< [0 - 1.0] gravity force direction (unit vector)
    float m_friction;                   //!< [0 - 1.0] friction when capsule collision is used
    float m_massScale;              //!< [In Meters] mass scale for this hair
    float m_inertiaScale;               //!< [0 - 1.0] inertia control. (0: no inertia, 1: full intertia)
    float m_inertiaLimit;               //!< [In Meters] speed limit where everything gets locked (for teleport etc.)
    float m_interactionStiffness;       //!< [0 - 1.0] how strong the hair interaction force is
    float m_rootStiffness;          //!< [0 - 1.0] attenuation of stiffness away from the root (stiffer at root, weaker toward tip)
    float m_pinStiffness;               //!< [0 - 1.0] stiffness for pin constraints
    bool m_simulate;                  //!< [true/false] whether to turn on/off simulation
    float m_stiffness;              //!< [0 - 1.0] how close hairs try to stay within skinned position
    float m_stiffnessStrength;      //!< [0 - 1.0] how strongly hairs move toward the stiffness target
    float m_stiffnessDamping;           //!< [0 - 1.0] how fast hair stiffness gerneated motion decays over time
    float m_tipStiffness;               //!< [0 - 1.0] attenuation of stiffness away from the tip (stiffer at tip, weaker toward root)
    bool m_useCollision;              //!< [true/false] whether to use the sphere/capsule collision or not for hair/body collision
    Vector3 m_wind;                        //!< [In Meters] vector force for main wind direction
    float m_windNoise;              //!< [0 - 1.0] strength of wind noise

    Vector4 m_stiffnessCurve;          //! [0 - 1.0] curve values for stiffness 
    Vector4 m_stiffnessStrengthCurve;  //! [0 - 1.0] curve values for stiffness strength
    Vector4 m_stiffnessDampingCurve;   //! [0 - 1.0] curve values for stiffness damping
    Vector4 m_bendStiffnessCurve;      //! [0 - 1.0] curve values for bend stiffness
    Vector4 m_interactionStiffnessCurve;//! [0 - 1.0] curve values for interaction stiffness

    // lod controls
    bool m_enableLOD;             //!< [true/false] whether to enable/disable entire lod scheme

    bool m_enableDistanceLOD;     //!< [true/false] whether to enable lod for far away object (distance LOD)
    float m_distanceLODStart;           //!< [In Meters] distance (in scene unit) to camera where fur will start fading out (by reducing density)
    float m_distanceLODEnd;         //!< [In Meters] distance (in scene unit) to camera where fur will completely disappear (and stop simulating)
    float m_distanceLODFadeStart;       //!< [In Meters] distance (in scene unit) to camera where fur will fade with alpha from 1 (this distance) to 0 (DistanceLODEnd)
    float m_distanceLODDensity;     //!< [0 - ] density when distance LOD is in action.  hairDensity gets scaled based on LOD factor.
    float m_distanceLODWidth;           //!< [In Millimeters] hair width that can change when close up density is triggered by closeup lod mechanism

    bool m_enableDetailLOD;           //!< [true/false] whether to enable lod for close object (detail LOD)
    float m_detailLODStart;         //!< [In Meters] distance (in scene unit) to camera where fur will start getting denser toward closeup density
    float m_detailLODEnd;               //!< [In Meters] distance (in scene unit) to camera where fur will get full closeup density value
    float m_detailLODDensity;           //!< [0 - ] density scale when closeup LOD is in action.  hairDensity gets scaled based on LOD factor.
    float m_detailLODWidth;         //!< [In Millimeters] hair width that can change when close up density is triggered by closeup lod mechanism

    float m_shadowDensityScale;     //!< [0 - 1] density scale factor to reduce hair density for shadow map rendering

    bool m_useViewfrustrumCulling;    //!< [true/false] when this is on, density for hairs outside view are set to 0. Use this option when fur is in a closeup.
    bool m_useBackfaceCulling;        //!< [true/false] when this is on, density for hairs growing from backfacing faces will be set to 0
    float m_backfaceCullingThreshold; //!< [-1 - 1.0] threshold to determine backface, note that this value should be slightly smaller 0 to avoid hairs at the silhouette from disappearing

    bool m_useCullSphere;         //!< [true/false] when this is on, hairs get culled when their root points are inside the sphere
    Matrix4x4 m_cullSphereInvTransform;    //!< inverse of general affine transform (scale, rotation, translation..) applied to a unit sphere centered at origin

    int m_splineMultiplier;           //!< how many vertices are generated per each control hair segments in spline curves

    // drawing option
    bool m_drawRenderHairs;           //!< [true/false] draw render hair
    bool m_visualizeBones;            //!< [true/false] visualize skinning bones
    bool m_visualizeBoundingBox;      //!< [true/false] draw bounding box of hairs
    bool m_visualizeCapsules;     //!< [true/false] visualize collision capsules
    bool m_visualizeControlVertices; //!< [true/false] draw control vertices of guide hairs
    bool m_visualizeCullSphere;       //!< [true/false] draw cull sphere
    bool m_visualizeFrames;           //!< [true/false] visualize coordinate frames
    bool m_visualizeGrowthMesh;       //!< [true/false] draw growth mesh
    bool m_visualizeGuideHairs;       //!< [true/false] draw guide hairs
    bool m_visualizeHairInteractions;//!< [true/false] draw hair interaction lines
    int m_visualizeHairSkips;     //!< [0 - ] for per hair visualization, how many hairs to skip?
    bool m_visualizeLocalPos;     //!< [true/false] visualize target pose for bending
    bool m_visualizePinConstraints;   //!< [true/false] whether to visualize pin constraint spheres
    bool m_visualizeShadingNormals;   //!< [true/false] visualize normals used for hair shading
    bool m_visualizeShadingNormalBone;    //!< [true/false] visualize bone used as shading normal center
    bool m_visualizeSkinnedGuideHairs; //!< [true/false] draw skinned positions for guide hairs

    int m_colorizeMode;               //!< [GFSDK_HAIR_COLORIZE_MODE] colorize hair based on various terms. See GFSDK_HAIR_COLORIZE_MODE.

    // texture control
    fixed int m_textureChannels[(int)GFSDK_HAIR_TEXTURE_TYPE.GFSDK_HAIR_NUM_TEXTURES]; //!< texture chanel for each control textures.  

    // model to world transform
    Matrix4x4 m_modelToWorld;              // render time transformation to offset hair from its simulated position
}

public enum GFSDK_HAIR_TEXTURE_TYPE
{
    GFSDK_HAIR_TEXTURE_DENSITY, //<! hair density map [ shape control ]
    GFSDK_HAIR_TEXTURE_ROOT_COLOR, //<! color at the hair root [ shading ]
    GFSDK_HAIR_TEXTURE_TIP_COLOR, //<! color at the hair tip [ shading ]
    GFSDK_HAIR_TEXTURE_WIDTH,  //<! width  [ shape control ]
    GFSDK_HAIR_TEXTURE_STIFFNESS,  //<! stiffness control [ simulation ]
    GFSDK_HAIR_TEXTURE_ROOT_STIFFNESS, //<! stiffness control for root stiffness [simulation]
    GFSDK_HAIR_TEXTURE_CLUMP_SCALE,  //<! clumpiness control [ shape control]
    GFSDK_HAIR_TEXTURE_CLUMP_ROUNDNESS, //<! clumping noise [ shape control]
    GFSDK_HAIR_TEXTURE_WAVE_SCALE, //<! waviness scale [ shape control ]
    GFSDK_HAIR_TEXTURE_WAVE_FREQ, //<! waviness frequency [ shape control ]
    GFSDK_HAIR_TEXTURE_STRAND, //<! texture along hair strand [ shading ]
    GFSDK_HAIR_TEXTURE_LENGTH, //<! length control [shape control] 
    GFSDK_HAIR_TEXTURE_SPECULAR, //<! specularity control [shading ] 
    GFSDK_HAIR_TEXTURE_WEIGHTS, //!< weight texture for multiple material blending [control for all other textures]

    GFSDK_HAIR_NUM_TEXTURES
};


public unsafe class HairWorksIntegration
{
    [DllImport ("HairWorksIntegration")] public static extern hwAssetID     hwLoadAssetFromFile(string path);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwDeleteAsset(hwAssetID aid);

    [DllImport ("HairWorksIntegration")] public static extern hwInstanceID hwCreateInstance(hwAssetID aid);
    [DllImport ("HairWorksIntegration")] public static extern bool hwDeleteInstance(hwInstanceID iid);
    [DllImport ("HairWorksIntegration")] public static extern void hwSetDescriptor(hwInstanceID iid);
    [DllImport ("HairWorksIntegration")] public static extern void hwUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, Matrix4x4 *matrices);

    [DllImport ("HairWorksIntegration")] public static extern float hwSetViewProjectionMatrix(ref Matrix4x4 view, ref Matrix4x4 proj);
    [DllImport ("HairWorksIntegration")] public static extern float hwStepSimulation();
    [DllImport ("HairWorksIntegration")] public static extern void hwRender();
}

using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

[System.Serializable]
public struct hwShaderID
{
    public static hwShaderID NullID = new hwShaderID(0xFFFF);

    public int id;

    public hwShaderID(int v) { this.id = v; }
    public static implicit operator hwShaderID(int v) { return new hwShaderID(v); }
    public static implicit operator int (hwShaderID v) { return v.id; }
    public static implicit operator bool (hwShaderID v) { return v.id != 0xFFFF; }
}

[System.Serializable]
public struct hwAssetID
{
    public static hwAssetID NullID = new hwAssetID(0xFFFF);

    public int id;

    public hwAssetID(int v) { this.id = v; }
    public static implicit operator hwAssetID(int v) { return new hwAssetID(v); }
    public static implicit operator int (hwAssetID v) { return v.id; }
    public static implicit operator bool (hwAssetID v) { return v.id != 0xFFFF; }
}

[System.Serializable]
public struct hwInstanceID
{
    public static hwInstanceID NullID = new hwInstanceID(0xFFFF);

    public int id;

    public hwInstanceID(int v) { this.id = v; }
    public static implicit operator hwInstanceID(int v) { return new hwInstanceID(v); }
    public static implicit operator int (hwInstanceID v) { return v.id; }
    public static implicit operator bool (hwInstanceID v) { return v.id != 0xFFFF; }
}

[System.Serializable]
public unsafe struct hwDescriptor
{
    // global controls
    public bool m_enable;                    //!< [true/false] whether to enable this hair. When disabled, hair will not cause any computation/rendering

    // hair geometry (density/width/length/clump) controls
    public float m_width;                  //!< [In Millimeters] hair width (thickness)
    public float m_widthNoise;             //!< [0 - 1.0] noise factor for hair width noise 
    public float m_widthRootScale;         //!< [0 - 1.0] scale factor for top side of the strand
    public float m_widthTipScale;          //!< [0 - 1.0] scale factor for bottom side of the strand

    public float m_clumpNoise;             //!< [0 - 1.0] probability of each hair gets clumped (0 = all hairs get clumped, 1 = clump scale is randomly distributed from 0 to 1)
    public float m_clumpRoundness;         //!< [0 - 2.0] exponential factor to control roundness of clump shape  = 0 = linear cone, clump scale *= power(t, roundness; where t is normalized distance from the root)
    public float m_clumpScale;             //!< [0 - 1.0] how clumped each hair face is

    public float m_density;                    //!< [0 -    ] hair density per face (1.0 = 64 hairs per face)
    public bool m_usePixelDensity;           //!< [true/false] whether to use per-pixel sampling or per-vertex sampling for density map

    public float m_lengthNoise;                //!< [0 - 1.0] length variation noise
    public float m_lengthScale;                //!< [0 - 1.0] length control for growing hair effect

    public float m_waveScale;              //!< [In Centimeters] size of waves for hair waviness 
    public float m_waveScaleNoise;         //!< [0 - 1.0] noise factor for the wave scale
    public float m_waveScaleClump;         //!< [0 - 1.0] waviness at clump level
    public float m_waveScaleStrand;            //!< [0 - 1.0] waviness at strand level
    public float m_waveFreq;                   //!< [0 -    ] wave frequency (1.0 = one sine wave along hair length)
    public float m_waveFreqNoise;          //!< [0 - 1.0] noise factor for the wave frequency 
    public float m_waveRootStraighten;     //!< [0 - 1.0] For some distance from the root, we atteunate waviness so that root itself does not move [0-1]

    /// shading controls
    public float m_rootAlphaFalloff;           //!< [0 - 1.0] falloff factor for alpha transition from root 
    public Color m_rootColor;               //!< [0 - 1.0] color of hair root (when hair textures are not used)
    public Color m_tipColor;                    //!< [0 - 1.0] color of hair tip (when hair textures are not used)
    public float m_rootTipColorWeight;     //!< [0 - 1.0] blend factor between root and tip color in addition to hair length
    public float m_rootTipColorFalloff;        //!< [0 - 1.0] falloff factor for root/tip color interpolation

    public float m_diffuseBlend;               //!< [0 - 1.0] blend factor between Kajiya hair lighting vs normal skin lighting.
    public float m_hairNormalWeight;           //!< [0 - 1.0] blend factor between mesh normal vs hair normal. Use higher value for longer (surface like) hair.
    public int m_hairNormalBoneIndex;        //!< [0 - number of bones] index for the bone which we use as model center for diffuse shading purpose

    public Color m_specularColor;           //!< [0 - 1.0] specular color
    public float m_specularNoiseScale;     //!< [0 - 1.0] amount of specular noise
    public float m_specularEnvScale;           //!< [0 - 1.0] amount of specular scale from env probe
    public float m_specularPrimary;            //!< [0 - 1.0] primary specular factor
    public float m_specularPowerPrimary;       //!< [0 - ] primary specular power exponent
    public float m_specularPrimaryBreakup; //!< [0 - 1.0] shift factor to make specular highlight move with noise
    public float m_specularSecondary;      //!< [0 - 1.0] secondary specular factor
    public float m_specularSecondaryOffset;    //!< [0 - 1.0] secondary highlight shift offset along tangents
    public float m_specularPowerSecondary; //!< [0 - ] secondary specular power exponent		

    public float m_glintStrength;          //!< [0 - 1.0] strength of the glint noise
    public float m_glintCount;             //!< [0 - 1024] number of glint sparklets along each hair
    public float m_glintExponent;          //!< [0 - ] glint power exponent

    public bool m_castShadows;               //!< [true/false] this hair cast shadows onto the scene
    public bool m_receiveShadows;            //!< [true/false] this hair receives shadows from the scene
    public float m_shadowSigma;                //!< [In Centimeters] distance through hair volume beyond which hairs get completely shadowed.

    public int m_strandBlendMode;            //!< [GFSDK_HAIR_STRAND_BLEND_MODE] blend mode when strand texture is used. Supported mode are defined in GFSDK_HAIR_STRAND_BLEND_MODE.
    public float m_strandBlendScale;           //!< [0 - 1.0] scale strand texture before blend

    // simulation control
    public float m_backStopRadius;         //!< [0 - 1.0] radius of backstop collision (normalized along hair length)
    public float m_bendStiffness;          //!< [0 - 1.0] stiffness for bending, useful for long hair
    public float m_damping;                    //!< [0 - ] damping to slow down hair motion
    public Vector3 m_gravityDir;              //!< [0 - 1.0] gravity force direction (unit vector)
    public float m_friction;                   //!< [0 - 1.0] friction when capsule collision is used
    public float m_massScale;              //!< [In Meters] mass scale for this hair
    public float m_inertiaScale;               //!< [0 - 1.0] inertia control. (0: no inertia, 1: full intertia)
    public float m_inertiaLimit;               //!< [In Meters] speed limit where everything gets locked (for teleport etc.)
    public float m_interactionStiffness;       //!< [0 - 1.0] how strong the hair interaction force is
    public float m_rootStiffness;          //!< [0 - 1.0] attenuation of stiffness away from the root (stiffer at root, weaker toward tip)
    public float m_pinStiffness;               //!< [0 - 1.0] stiffness for pin constraints
    public bool m_simulate;                  //!< [true/false] whether to turn on/off simulation
    public float m_stiffness;              //!< [0 - 1.0] how close hairs try to stay within skinned position
    public float m_stiffnessStrength;      //!< [0 - 1.0] how strongly hairs move toward the stiffness target
    public float m_stiffnessDamping;           //!< [0 - 1.0] how fast hair stiffness gerneated motion decays over time
    public float m_tipStiffness;               //!< [0 - 1.0] attenuation of stiffness away from the tip (stiffer at tip, weaker toward root)
    public bool m_useCollision;              //!< [true/false] whether to use the sphere/capsule collision or not for hair/body collision
    public Vector3 m_wind;                        //!< [In Meters] vector force for main wind direction
    public float m_windNoise;              //!< [0 - 1.0] strength of wind noise

    public Vector4 m_stiffnessCurve;          //! [0 - 1.0] curve values for stiffness 
    public Vector4 m_stiffnessStrengthCurve;  //! [0 - 1.0] curve values for stiffness strength
    public Vector4 m_stiffnessDampingCurve;   //! [0 - 1.0] curve values for stiffness damping
    public Vector4 m_bendStiffnessCurve;      //! [0 - 1.0] curve values for bend stiffness
    public Vector4 m_interactionStiffnessCurve;//! [0 - 1.0] curve values for interaction stiffness

    // lod controls
    public bool m_enableLOD;             //!< [true/false] whether to enable/disable entire lod scheme

    public bool m_enableDistanceLOD;     //!< [true/false] whether to enable lod for far away object (distance LOD)
    public float m_distanceLODStart;           //!< [In Meters] distance (in scene unit) to camera where fur will start fading out (by reducing density)
    public float m_distanceLODEnd;         //!< [In Meters] distance (in scene unit) to camera where fur will completely disappear (and stop simulating)
    public float m_distanceLODFadeStart;       //!< [In Meters] distance (in scene unit) to camera where fur will fade with alpha from 1 (this distance) to 0 (DistanceLODEnd)
    public float m_distanceLODDensity;     //!< [0 - ] density when distance LOD is in action.  hairDensity gets scaled based on LOD factor.
    public float m_distanceLODWidth;           //!< [In Millimeters] hair width that can change when close up density is triggered by closeup lod mechanism

    public bool m_enableDetailLOD;           //!< [true/false] whether to enable lod for close object (detail LOD)
    public float m_detailLODStart;         //!< [In Meters] distance (in scene unit) to camera where fur will start getting denser toward closeup density
    public float m_detailLODEnd;               //!< [In Meters] distance (in scene unit) to camera where fur will get full closeup density value
    public float m_detailLODDensity;           //!< [0 - ] density scale when closeup LOD is in action.  hairDensity gets scaled based on LOD factor.
    public float m_detailLODWidth;         //!< [In Millimeters] hair width that can change when close up density is triggered by closeup lod mechanism

    public float m_shadowDensityScale;     //!< [0 - 1] density scale factor to reduce hair density for shadow map rendering

    public bool m_useViewfrustrumCulling;    //!< [true/false] when this is on, density for hairs outside view are set to 0. Use this option when fur is in a closeup.
    public bool m_useBackfaceCulling;        //!< [true/false] when this is on, density for hairs growing from backfacing faces will be set to 0
    public float m_backfaceCullingThreshold; //!< [-1 - 1.0] threshold to determine backface, note that this value should be slightly smaller 0 to avoid hairs at the silhouette from disappearing

    public bool m_useCullSphere;         //!< [true/false] when this is on, hairs get culled when their root points are inside the sphere
    public Matrix4x4 m_cullSphereInvTransform;    //!< inverse of general affine transform (scale, rotation, translation..) applied to a unit sphere centered at origin

    public int m_splineMultiplier;           //!< how many vertices are generated per each control hair segments in spline curves

    // drawing option
    public bool m_drawRenderHairs;           //!< [true/false] draw render hair
    public bool m_visualizeBones;            //!< [true/false] visualize skinning bones
    public bool m_visualizeBoundingBox;      //!< [true/false] draw bounding box of hairs
    public bool m_visualizeCapsules;     //!< [true/false] visualize collision capsules
    public bool m_visualizeControlVertices; //!< [true/false] draw control vertices of guide hairs
    public bool m_visualizeCullSphere;       //!< [true/false] draw cull sphere
    public bool m_visualizeFrames;           //!< [true/false] visualize coordinate frames
    public bool m_visualizeGrowthMesh;       //!< [true/false] draw growth mesh
    public bool m_visualizeGuideHairs;       //!< [true/false] draw guide hairs
    public bool m_visualizeHairInteractions;//!< [true/false] draw hair interaction lines
    public int m_visualizeHairSkips;     //!< [0 - ] for per hair visualization, how many hairs to skip?
    public bool m_visualizeLocalPos;     //!< [true/false] visualize target pose for bending
    public bool m_visualizePinConstraints;   //!< [true/false] whether to visualize pin constraint spheres
    public bool m_visualizeShadingNormals;   //!< [true/false] visualize normals used for hair shading
    public bool m_visualizeShadingNormalBone;    //!< [true/false] visualize bone used as shading normal center
    public bool m_visualizeSkinnedGuideHairs; //!< [true/false] draw skinned positions for guide hairs

    public int m_colorizeMode;               //!< [GFSDK_HAIR_COLORIZE_MODE] colorize hair based on various terms. See GFSDK_HAIR_COLORIZE_MODE.

    // texture control
    public fixed int m_textureChannels[(int)hwTextureType.NUM_TEXTURES]; //!< texture chanel for each control textures.  

    // model to world transform
    public Matrix4x4 m_modelToWorld;              // render time transformation to offset hair from its simulated position


    public static hwDescriptor default_value
    {
        get
        {
            hwDescriptor ret = new hwDescriptor();
            ret.initialize();
            return ret;
        }
    }

    public void initialize()
    {
        m_enable = true;

        // default geometry parameters
        m_width = 1.0f;
        m_widthNoise = 0.0f;
        m_widthRootScale = 1.0f;
        m_widthTipScale = 0.1f;

        m_clumpNoise = 0.0f;
        m_clumpRoundness = 1.0f;
        m_clumpScale = 0.0f;

        m_density = 1.0f;
        m_lengthNoise = 1.0f;
        m_lengthScale = 1.0f;

        m_usePixelDensity = false;
        m_waveScale = 0.0f;
        m_waveScaleClump = 0.0f;
        m_waveScaleStrand = 1.0f;
        m_waveScaleNoise = 0.5f;
        m_waveFreq = 3.0f;
        m_waveFreqNoise = 0.5f;
        m_waveRootStraighten = 0.0f;

        // default shading parameters
        m_diffuseBlend = 0.5f;
        m_hairNormalWeight = 0.0f;
        m_hairNormalBoneIndex = -1;

        m_glintStrength = 0.0f;
        m_glintCount = 256.0f;
        m_glintExponent = 2.0f;
        m_rootTipColorWeight = 0.5f;
        m_rootTipColorFalloff = 1.0f;

        m_specularNoiseScale = 0.0f;
        m_specularEnvScale = 0.25f;
        m_specularPrimary = 0.1f;
        m_specularPrimaryBreakup = 0.0f;
        m_specularSecondary = 0.05f;
        m_specularSecondaryOffset = 0.1f;
        m_specularPowerPrimary = 100.0f;
        m_specularPowerSecondary = 20.0f;

        m_rootAlphaFalloff = 0.0f;

        m_shadowSigma = 0.2f;
        m_castShadows = true;
        m_receiveShadows = true;

        m_strandBlendMode = 0;
        m_strandBlendScale = 1.0f;

        // default simulation parameters
        m_simulate = true;
        m_backStopRadius = 0.0f;
        m_bendStiffness = 0.0f;
        m_damping = 0.0f;
        m_friction = 0.0f;
        m_inertiaScale = 1.0f;
        m_inertiaLimit = 1000.0f;
        m_interactionStiffness = 0.0f;
        m_massScale = 10.0f;
        m_windNoise = 0.0f;
        m_stiffness = 0.5f;
        m_stiffnessStrength = 1.0f;
        m_stiffnessDamping = 0.0f;
        m_rootStiffness = 0.5f;
        m_pinStiffness = 1.0f;
        m_tipStiffness = 0.0f;
        m_useCollision = false;

        // default LOD parameters
        m_enableLOD = false;

        m_enableDistanceLOD = true;
        m_distanceLODStart = 5.0f; 
        m_distanceLODEnd = 10.0f; 
        m_distanceLODFadeStart = 1000.0f;
        m_distanceLODDensity = 0.0f;
        m_distanceLODWidth = 1.0f;

        m_enableDetailLOD = true;
        m_detailLODStart = 2.0f; 
        m_detailLODEnd = 1.0f; 
        m_detailLODDensity = 1.0f;
        m_detailLODWidth = 1.0f;

        m_shadowDensityScale = 0.5f;

        m_useViewfrustrumCulling = true;
        m_useBackfaceCulling = false;
        m_backfaceCullingThreshold = -0.2f;
        m_useCullSphere = false;

        m_splineMultiplier = 4;

        // visualization options
        m_drawRenderHairs = true;		
        m_visualizeBones = false;
        m_visualizePinConstraints = false;
        m_visualizeCapsules = false;
        m_visualizeGrowthMesh = false;	
        m_visualizeGuideHairs = false;	
        m_visualizeControlVertices = false;
        m_visualizeBoundingBox = false;
        m_visualizeCullSphere = false;
        m_visualizeShadingNormalBone = false;
        m_visualizeHairInteractions = false;
        m_visualizeSkinnedGuideHairs = false;	
        m_visualizeFrames = false;
        m_visualizeLocalPos = false;
        m_visualizeShadingNormals = false;
        m_visualizeHairSkips = 0;

        m_colorizeMode = 0;

        {
            m_gravityDir.x = 0.0f;
            m_gravityDir.y = 0.0f;
            m_gravityDir.z = -1.0f;

            m_wind.x = 0.0f;
            m_wind.y = 0.0f;
            m_wind.z = 0.0f;

            m_rootColor.r = 1.0f; m_rootColor.g = 1.0f; m_rootColor.b = 1.0f; m_rootColor.a = 1.0f;
            m_tipColor.r = 1.0f; m_tipColor.g = 1.0f; m_tipColor.b = 1.0f; m_tipColor.a = 1.0f;
            m_specularColor.r = 1.0f; m_specularColor.g = 1.0f; m_specularColor.b = 1.0f; m_specularColor.a = 1.0f;

            m_modelToWorld = Matrix4x4.identity;
            m_cullSphereInvTransform = Matrix4x4.identity;

            m_stiffnessCurve = Vector4.one;
            m_stiffnessStrengthCurve = Vector4.one;
            m_stiffnessDampingCurve = Vector4.one;
            m_bendStiffnessCurve = Vector4.one;
            m_interactionStiffnessCurve = Vector4.one;

            fixed(int *p = m_textureChannels)
            {
                for (int i = 0; i < (int)hwTextureType.NUM_TEXTURES; i++)
                {
                    p[i] = 0;
                }
            }

        }
    }
}

public enum hwTextureType
{
    DENSITY, //<! hair density map [ shape control ]
    ROOT_COLOR, //<! color at the hair root [ shading ]
    TIP_COLOR, //<! color at the hair tip [ shading ]
    WIDTH,  //<! width  [ shape control ]
    STIFFNESS,  //<! stiffness control [ simulation ]
    ROOT_STIFFNESS, //<! stiffness control for root stiffness [simulation]
    CLUMP_SCALE,  //<! clumpiness control [ shape control]
    CLUMP_ROUNDNESS, //<! clumping noise [ shape control]
    WAVE_SCALE, //<! waviness scale [ shape control ]
    WAVE_FREQ, //<! waviness frequency [ shape control ]
    STRAND, //<! texture along hair strand [ shading ]
    LENGTH, //<! length control [shape control] 
    SPECULAR, //<! specularity control [shading ] 
    WEIGHTS, //!< weight texture for multiple material blending [control for all other textures]

    NUM_TEXTURES
};


public static class HairWorksIntegration
{
    public delegate void hwLogCallback(System.IntPtr cstr);
    [DllImport ("HairWorksIntegration")] public static extern int           hwGetFlushEventID();
    [DllImport ("HairWorksIntegration")] public static extern void          hwSetLogCallback(hwLogCallback cb);

    [DllImport ("HairWorksIntegration")] public static extern hwShaderID    hwShaderLoadFromFile(string path);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwShaderRelease(hwShaderID sid);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwShaderReload(hwShaderID sid);

    [DllImport ("HairWorksIntegration")] public static extern hwAssetID     hwAssetLoadFromFile(string path);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwAssetRelease(hwAssetID aid);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwAssetReload(hwAssetID aid);

    [DllImport ("HairWorksIntegration")] public static extern hwInstanceID  hwInstanceCreate(hwAssetID aid);
    [DllImport ("HairWorksIntegration")] public static extern bool          hwInstanceRelease(hwInstanceID iid);
    [DllImport ("HairWorksIntegration")] public static extern void          hwInstanceGetDescriptor(hwInstanceID iid, ref hwDescriptor desc);
    [DllImport ("HairWorksIntegration")] public static extern void          hwInstanceSetDescriptor(hwInstanceID iid, ref hwDescriptor desc);
    [DllImport ("HairWorksIntegration")] public static extern void          hwInstanceSetTexture(hwInstanceID iid, hwTextureType type, IntPtr tex);
    [DllImport ("HairWorksIntegration")] public static extern void          hwInstanceUpdateSkinningMatrices(hwInstanceID iid, int num_matrices, IntPtr matrices);

    [DllImport ("HairWorksIntegration")] public static extern void          hwSetViewProjection(ref Matrix4x4 view, ref Matrix4x4 proj, float fov);
    [DllImport ("HairWorksIntegration")] public static extern void          hwSetRenderTarget(System.IntPtr framebuffer, System.IntPtr depthbuffer);
    [DllImport ("HairWorksIntegration")] public static extern void          hwSetShader(hwShaderID sid);
    [DllImport ("HairWorksIntegration")] public static extern void          hwRender(hwInstanceID iid);
    [DllImport ("HairWorksIntegration")] public static extern void          hwRenderShadow(hwInstanceID iid);
    [DllImport ("HairWorksIntegration")] public static extern void          hwStepSimulation(float dt);

    static void LogCallback(System.IntPtr cstr)
    {
        Debug.Log(Marshal.PtrToStringAnsi(cstr));
    }

    public static void hwSetLogCallback()
    {
        hwSetLogCallback(LogCallback);
    }

}

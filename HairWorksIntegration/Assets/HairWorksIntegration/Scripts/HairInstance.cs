using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif


[AddComponentMenu("Hair Works Integration/Hair Instance")]
[RequireComponent(typeof(Renderer))]
public class HairInstance : MonoBehaviour
{
    #region static
    static HashSet<HairInstance> s_instances;
    static int s_nth_LateUpdate;
    static int s_nth_OnWillRenderObject;
    static public Vector2 s_resolution_scale = Vector2.one;
    static public RenderTexture s_framebuffer;
    static public RenderTexture s_depthbuffer;

    static public HashSet<HairInstance> GetInstances()
    {
        if (s_instances == null)
        {
            s_instances = new HashSet<HairInstance>();
        }
        return s_instances;
    }
    #endregion


    public string m_hair_shader = "HairWorksIntegration/DefaultHairShader.cso";
    public string m_hair_asset = "HairWorksIntegration/ExampleAsset.apx";
    public hwConversionSettings m_conversion = hwConversionSettings.default_value;
    public hwDescriptor m_params = hwDescriptor.default_value;
    public bool m_use_default_descriptor = true;
    hwShaderID m_sid = hwShaderID.NullID;
    hwAssetID m_aid = hwAssetID.NullID;
    hwInstanceID m_iid = hwInstanceID.NullID;
    Matrix4x4[] m_skinning_matrices;
    System.IntPtr m_skinning_matrices_ptr;


    public int shader_id { get { return m_sid; } }
    public int asset_id { get { return m_aid; } }
    public int instance_id { get { return m_iid; } }


    public void LoadHairShader(string path_to_cso)
    {
        // release existing shader
        if (m_sid)
        {
            HairWorksIntegration.hwShaderRelease(m_sid);
            m_sid = hwShaderID.NullID;
        }

        // load shader
        if (m_sid = HairWorksIntegration.hwShaderLoadFromFile(Application.streamingAssetsPath + "/" + path_to_cso))
        {
            m_hair_shader = path_to_cso;
        }
    }

    public void ReloadHairShader()
    {
        HairWorksIntegration.hwShaderReload(m_sid);
    }

    public void LoadHairAsset(string path_to_apx)
    {
        // release existing instance & asset
        if (m_iid)
        {
            HairWorksIntegration.hwInstanceRelease(m_iid);
            m_iid = hwInstanceID.NullID;
        }
        if (m_aid)
        {
            HairWorksIntegration.hwAssetRelease(m_aid);
            m_aid = hwAssetID.NullID;
        }

        // load & create instance
        if(m_aid = HairWorksIntegration.hwAssetLoadFromFile(Application.streamingAssetsPath + "/" + path_to_apx, ref m_conversion))
        {
            m_hair_asset = path_to_apx;
            m_iid = HairWorksIntegration.hwInstanceCreate(m_aid);
            if(m_use_default_descriptor)
            {
                HairWorksIntegration.hwAssetGetDefaultDescriptor(m_aid, ref m_params);
            }
        }
    }

    public void ReloadHairAsset()
    {
        HairWorksIntegration.hwAssetReload(m_aid);
    }

    public void AssignTexture(hwTextureType type, Texture2D tex)
    {
        HairWorksIntegration.hwInstanceSetTexture(m_iid, type, tex.GetNativeTexturePtr());
    }



    void Awake()
    {
        HairWorksIntegration.hwSetLogCallback();
        GetInstances().Add(this);
    }

    void OnDestroy()
    {
        HairWorksIntegration.hwInstanceRelease(m_iid);
        HairWorksIntegration.hwAssetRelease(m_aid);
        GetInstances().Remove(this);
    }

    void Start()
    {
        LoadHairShader(m_hair_shader);
        LoadHairAsset(m_hair_asset);
    }

    void Update()
    {
        if(m_skinning_matrices == null)
        {
            m_skinning_matrices = new Matrix4x4[HairWorksIntegration.hwAssetGetNumBones(m_aid)];
            m_skinning_matrices_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(m_skinning_matrices, 0);
        }

        // todo: gather skinning bones if needed
        var m = GetComponent<Transform>().localToWorldMatrix;
        for (int i=0; i< m_skinning_matrices.Length; ++i) {
            m_skinning_matrices[i] = m;
        }

        HairWorksIntegration.hwInstanceSetDescriptor(m_iid, ref m_params);
        HairWorksIntegration.hwInstanceUpdateSkinningMatrices(m_iid, m_skinning_matrices.Length, m_skinning_matrices_ptr);

        s_nth_LateUpdate = 0;
    }

    void LateUpdate()
    {
        if(s_nth_LateUpdate++ == 0)
        {
            HairWorksIntegration.hwStepSimulation(Time.deltaTime);
        }
    }

    void OnWillRenderObject()
    {
        if (s_nth_OnWillRenderObject++ == 0)
        {
            BeginRender();
            foreach (var a in GetInstances())
            {
                a.Render();
            }
        }
    }

    void OnRenderObject()
    {
        EndRender();
        s_nth_OnWillRenderObject = 0;
    }



    void BeginRender()
    {
        var cam = Camera.current;
        if(cam != null)
        {
            Matrix4x4 V = cam.worldToCameraMatrix;
            Matrix4x4 P = GL.GetGPUProjectionMatrix(cam.projectionMatrix, true);
            float fov = cam.fieldOfView;
            HairWorksIntegration.hwSetViewProjection(ref V, ref P, fov);
        }
    }

    void Render()
    {
        HairWorksIntegration.hwSetShader(m_sid);
        HairWorksIntegration.hwRender(m_iid);
    }

    void EndRender()
    {
        GL.IssuePluginEvent( HairWorksIntegration.hwGetFlushEventID() );
        GL.InvalidateState();
    }
}

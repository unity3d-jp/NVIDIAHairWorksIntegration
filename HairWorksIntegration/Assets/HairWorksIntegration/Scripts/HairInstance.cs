using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using UnityEngine;
using UnityEngine.Rendering;
#if UNITY_EDITOR
using UnityEditor;
#endif


[AddComponentMenu("Hair Works Integration/Hair Instance")]
public class HairInstance : MonoBehaviour
{
    #region static
    static HashSet<HairInstance> s_instances;
    static int s_nth_LateUpdate;
    static int s_nth_OnWillRenderObject;

    static CommandBuffer s_command_buffer;
    static HashSet<Camera> s_cameras = new HashSet<Camera>();

    static public HashSet<HairInstance> GetInstances()
    {
        if (s_instances == null)
        {
            s_instances = new HashSet<HairInstance>();
        }
        return s_instances;
    }
    #endregion

    static CameraEvent s_timing = CameraEvent.BeforeImageEffects;

    public string m_hair_shader = "HairWorksIntegration/DefaultHairShader.cso";
    public string m_hair_asset = "HairWorksIntegration/ExampleAsset.apx";
    public Transform m_root_bone;
    public hwConversionSettings m_load_settings = hwConversionSettings.default_value;
    public hwDescriptor m_params = hwDescriptor.default_value;
    public bool m_use_default_descriptor = true;
    public Mesh m_probe_mesh;
    hwHShader m_hshader = hwHShader.NullHandle;
    hwHAsset m_hasset = hwHAsset.NullHandle;
    hwHInstance m_hinstance = hwHInstance.NullHandle;

    public Transform[] m_bones;
    Matrix4x4[] m_inv_bindpose;
    Matrix4x4[] m_skinning_matrices;
    IntPtr m_skinning_matrices_ptr;


    public uint shader_id { get { return m_hshader; } }
    public uint asset_id { get { return m_hasset; } }
    public uint instance_id { get { return m_hinstance; } }


    public void LoadHairShader(string path_to_cso)
    {
        // release existing shader
        if (m_hshader)
        {
            HairWorksIntegration.hwShaderRelease(m_hshader);
            m_hshader = hwHShader.NullHandle;
        }

        // load shader
        if (m_hshader = HairWorksIntegration.hwShaderLoadFromFile(Application.streamingAssetsPath + "/" + path_to_cso))
        {
            m_hair_shader = path_to_cso;
        }
    }

    public void ReloadHairShader()
    {
        HairWorksIntegration.hwShaderReload(m_hshader);
    }

    public void LoadHairAsset(string path_to_apx)
    {
        // release existing instance & asset
        if (m_hinstance)
        {
            HairWorksIntegration.hwInstanceRelease(m_hinstance);
            m_hinstance = hwHInstance.NullHandle;
        }
        if (m_hasset)
        {
            HairWorksIntegration.hwAssetRelease(m_hasset);
            m_hasset = hwHAsset.NullHandle;
        }

        // load & create instance
        if (m_hasset = HairWorksIntegration.hwAssetLoadFromFile(Application.streamingAssetsPath + "/" + path_to_apx, ref m_load_settings))
        {
            m_hair_asset = path_to_apx;
            m_hinstance = HairWorksIntegration.hwInstanceCreate(m_hasset);
            if(m_use_default_descriptor)
            {
                HairWorksIntegration.hwAssetGetDefaultDescriptor(m_hasset, ref m_params);
            }
        }

        // update bone structure
        m_bones = null;
        m_skinning_matrices = null;
        m_skinning_matrices_ptr = IntPtr.Zero;
        UpdateBones();
    }

    public void ReloadHairAsset()
    {
        HairWorksIntegration.hwAssetReload(m_hasset);
    }

    public void AssignTexture(hwTextureType type, Texture2D tex)
    {
        HairWorksIntegration.hwInstanceSetTexture(m_hinstance, type, tex.GetNativeTexturePtr());
    }

    public void UpdateBones()
    {
        int num_bones = HairWorksIntegration.hwAssetGetNumBones(m_hasset);
        if (m_bones == null || m_bones.Length != num_bones)
        {
            m_bones = new Transform[num_bones];
            m_inv_bindpose = new Matrix4x4[num_bones];
            m_skinning_matrices = new Matrix4x4[num_bones];
            m_skinning_matrices_ptr = IntPtr.Zero;

            for (int i = 0; i < num_bones; ++i)
            {
                m_inv_bindpose[i] = Matrix4x4.identity;
                m_skinning_matrices[i] = Matrix4x4.identity;
            }

            if (m_root_bone == null)
            {
                m_root_bone = GetComponent<Transform>();
            }

            var children = m_root_bone.GetComponentsInChildren<Transform>();
            for (int i = 0; i < num_bones; ++i)
            {
                string name = HairWorksIntegration.hwAssetGetBoneNameString(m_hasset, i);
                m_bones[i] = Array.Find(children, (a) => { return a.name == name; });
                if (m_bones[i] == null) { m_bones[i] = m_root_bone; }
                HairWorksIntegration.hwAssetGetBindPose(m_hasset, i, ref m_inv_bindpose[i]);
                //m_inv_bindpose[i] = m_bones[i].localToWorldMatrix;
                m_inv_bindpose[i] = m_inv_bindpose[i].inverse;
            }

        }
        if(m_skinning_matrices_ptr == IntPtr.Zero)
        {
            m_skinning_matrices_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(m_skinning_matrices, 0);
        }

        for (int i = 0; i < m_bones.Length; ++i)
        {
            var t = m_bones[i];
            if (t != null)
            {
                //m_skinning_matrices[i] = t.localToWorldMatrix * m_inv_bindpose[i];
                m_skinning_matrices[i] = t.localToWorldMatrix;
                //m_skinning_matrices[i] = Matrix4x4.identity;
            }
        }
    }

    static public void Swap<T>(ref T a, ref T b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }



#if UNITY_EDITOR
    void Reset()
    {
        m_root_bone = GetComponent<Transform>();

        var renderer = GetComponent<Renderer>();
        if(renderer == null)
        {
            m_probe_mesh = new Mesh();
            m_probe_mesh.name = "Probe";
            m_probe_mesh.vertices = new Vector3[1] { Vector3.zero };
            m_probe_mesh.SetIndices(new int[1] { 0 }, MeshTopology.Points, 0);

            var mesh_filter = gameObject.AddComponent<MeshFilter>();
            mesh_filter.sharedMesh = m_probe_mesh;
            renderer = gameObject.AddComponent<MeshRenderer>();
            renderer.sharedMaterials = new Material[0] { };
        }
    }
#endif

    void Awake()
    {
        HairWorksIntegration.hwSetLogCallback();
    }

    void OnDestroy()
    {
        HairWorksIntegration.hwInstanceRelease(m_hinstance);
        HairWorksIntegration.hwAssetRelease(m_hasset);
    }

    void OnEnable()
    {
        GetInstances().Add(this);
        m_params.m_enable = true;
    }

    void OnDisable()
    {
        m_params.m_enable = false;
        GetInstances().Remove(this);
    }

    void Start()
    {
        LoadHairShader(m_hair_shader);
        LoadHairAsset(m_hair_asset);
    }

    void Update()
    {
        if(!m_hasset) { return; }

        UpdateBones();
        HairWorksIntegration.hwInstanceSetDescriptor(m_hinstance, ref m_params);
        HairWorksIntegration.hwInstanceUpdateSkinningMatrices(m_hinstance, m_skinning_matrices.Length, m_skinning_matrices_ptr);

        if (m_probe_mesh != null)
        {
            var bmin = Vector3.zero;
            var bmax = Vector3.zero;
            HairWorksIntegration.hwInstanceGetBounds(m_hinstance, ref bmin, ref bmax);

            var center = (bmin + bmax) * 0.5f;
            var size = bmax - center;
            m_probe_mesh.bounds = new Bounds(center, size);
        }


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
            EndRender();
        }
    }

    void OnRenderObject()
    {
        s_nth_OnWillRenderObject = 0;
    }




    static public bool IsDeferred(Camera cam)
    {
        if (cam.renderingPath == RenderingPath.DeferredShading
#if UNITY_EDITOR
            || (cam.renderingPath == RenderingPath.UsePlayerSettings && PlayerSettings.renderingPath == RenderingPath.DeferredShading)
#endif
            )
        {
            return true;
        }
        return false;
    }

    static public bool DoesRenderToTexture(Camera cam)
    {
        return IsDeferred(cam) || cam.targetTexture != null;
    }


    static public void ClearCommandBuffer()
    {
        foreach (var c in s_cameras)
        {
            if (c != null)
            {
                c.RemoveCommandBuffer(s_timing, s_command_buffer);
            }
        }
        s_cameras.Clear();
    }

    static void BeginRender()
    {
        HairWorksIntegration.hwBeginScene();

        var cam = Camera.current;
        if(cam != null)
        {
            Matrix4x4 V = cam.worldToCameraMatrix;
            Matrix4x4 P = GL.GetGPUProjectionMatrix(cam.projectionMatrix, DoesRenderToTexture(cam));
            float fov = cam.fieldOfView;
            HairWorksIntegration.hwSetViewProjection(ref V, ref P, fov);
            HairLight.AssignLightData();
        }
    }

    void Render()
    {
        if (!m_hasset) { return; }

        HairWorksIntegration.hwSetShader(m_hshader);
        HairWorksIntegration.hwRender(m_hinstance);
    }

    static void EndRender()
    {
        if (s_command_buffer == null)
        {
            s_command_buffer = new CommandBuffer();
            s_command_buffer.name = "Hair";
            s_command_buffer.IssuePluginEvent(HairWorksIntegration.hwGetRenderEventFunc(), 0);
        }

        var cam = Camera.current;
        if(cam != null && !s_cameras.Contains(cam))
        {
            cam.AddCommandBuffer(s_timing, s_command_buffer);
            s_cameras.Add(cam);
        }
        HairWorksIntegration.hwEndScene();
    }


    //void OnDrawGizmos()
    //{
    //    // just for debug
    //    if (m_inv_bindpose != null)
    //    {
    //        Vector3 size = new Vector3(0.1f, 0.1f, 0.1f);
    //        Vector4 pos = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    //        Gizmos.color = Color.cyan;
    //        foreach (var m in m_inv_bindpose)
    //        {
    //            Gizmos.DrawWireCube(m.inverse * pos, size);
    //        }
    //    }
    //}

}

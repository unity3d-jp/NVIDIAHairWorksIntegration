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

    HashSet<HairInstance> GetInstances()
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
    public hwDescriptor m_params = hwDescriptor.default_value;
    hwShaderID m_sid = hwShaderID.NullID;
    hwAssetID m_aid = hwAssetID.NullID;
    hwInstanceID m_iid = hwInstanceID.NullID;


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
        if(m_aid = HairWorksIntegration.hwAssetLoadFromFile(Application.streamingAssetsPath + "/" + path_to_apx))
        {
            m_hair_asset = path_to_apx;
            m_iid = HairWorksIntegration.hwInstanceCreate(m_aid);
        }
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
        m_params.m_modelToWorld = GetComponent<Transform>().localToWorldMatrix;

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
        if(s_nth_OnWillRenderObject++ == 0)
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
    }
}

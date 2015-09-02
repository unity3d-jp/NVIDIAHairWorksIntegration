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


    public string m_hair_asset;
    public string m_hair_shader;
    public hwDescriptor m_params;

    hwAssetID m_asset_id = hwAssetID.NullID;
    hwInstanceID m_instance_id = hwInstanceID.NullID;

    void Awake()
    {
        GetInstances().Add(this);
    }

    void OnDestroy()
    {
        HairWorksIntegration.hwInstanceRelease(m_instance_id);
        HairWorksIntegration.hwAssetRelease(m_asset_id);
        GetInstances().Remove(this);
    }

    void Update()
    {
        s_nth_LateUpdate = 0;
        if (!m_asset_id)
        {
            m_asset_id = HairWorksIntegration.hwAssetLoadFromFile(m_hair_asset);
        }
        if (m_asset_id && !m_instance_id)
        {
            m_instance_id = HairWorksIntegration.hwInstanceCreate(m_asset_id);
        }
    }

    void LateUpdate()
    {
        if(s_nth_LateUpdate++==0)
        {
            HairWorksIntegration.hwStepSimulation(Time.deltaTime);
        }
    }

    void OnWillRenderObject()
    {
        if(s_nth_OnWillRenderObject++==0)
        {
            BeginRender();
            foreach (var a in GetInstances())
            {
                a.Render();
            }
            PostRender();
        }
    }

    void OnRenderObject()
    {
        s_nth_OnWillRenderObject = 0;
    }




    void BeginRender()
    {
    }

    void Render()
    {
    }

    void PostRender()
    {
    }
}

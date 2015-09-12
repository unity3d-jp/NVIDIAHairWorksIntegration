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


[AddComponentMenu("Hair Works Integration/Hair Light")]
[RequireComponent(typeof(Light))]
[ExecuteInEditMode]
public class HairLight : MonoBehaviour
{
    #region static
    static HashSet<HairLight> s_instances;
    static hwLightData[] s_light_data;
    static IntPtr s_light_data_ptr;

    static public HashSet<HairLight> GetInstances()
    {
        if (s_instances == null)
        {
            s_instances = new HashSet<HairLight>();
        }
        return s_instances;
    }

    static public void AssignLightData()
    {
        if(s_light_data == null)
        {
            s_light_data = new hwLightData[hwLightData.MaxLights];
            s_light_data_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(s_light_data, 0);
        }

        var instances = GetInstances();
        int n = Mathf.Min(instances.Count, hwLightData.MaxLights);
        int i = 0;
        foreach (var l in instances)
        {
            s_light_data[i] = l.GetLightData();
            if(++i == n) { break; }
        }
        HairWorksIntegration.hwSetLights(n, s_light_data_ptr);
    }
    #endregion


    public enum Type
    {
        Directional,
        Point,
    }

    hwLightData m_data;

    public bool m_copy_light_params = false;
    public LightType m_type = LightType.Directional;
    public float m_range = 10.0f;
    public Color m_color = Color.white;
    public float m_intensity = 1.0f;
    CommandBuffer m_cb;

    public CommandBuffer GetCommandBuffer()
    {
        if(m_cb == null)
        {
            m_cb = new CommandBuffer();
            m_cb.name = "Hair Shadow";
            GetComponent<Light>().AddCommandBuffer(LightEvent.AfterShadowMap, m_cb);
        }
        return m_cb;
    }

    public hwLightData GetLightData()
    {
        var t = GetComponent<Transform>();
        m_data.type = (int)m_type;
        m_data.range = m_range;
        m_data.color = new Color(m_color.r * m_intensity, m_color.g * m_intensity, m_color.b * m_intensity, 0.0f);
        m_data.position = t.position;
        m_data.direction = t.forward;
        return m_data;

    }


    void OnEnable()
    {
        GetInstances().Add(this);
        if(GetInstances().Count > hwLightData.MaxLights)
        {
            Debug.LogWarning("Max HairLight is " + hwLightData.MaxLights + ". Current active HairLight is " + GetInstances().Count);
        }
    }

    void OnDisable()
    {
        GetInstances().Remove(this);
    }

    void Update()
    {
        if(m_copy_light_params)
        {
            var l = GetComponent<Light>();
            m_type = l.type;
            m_range = l.range;
            m_color = l.color;
            m_intensity = l.intensity;
        }
    }

}

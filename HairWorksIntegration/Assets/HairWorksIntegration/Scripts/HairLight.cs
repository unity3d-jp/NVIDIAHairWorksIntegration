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


[AddComponentMenu("Hair Works Integration/Hair Light")]
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

    public Type m_type;
    public float m_range = 5.0f;
    public Color m_color = Color.white;
    [Range(0.0f, 8.0f)] float m_intensity = 1.0f;

    hwLightData m_data;


    hwLightData GetLightData()
    {
        var t = GetComponent<Transform>();
        m_data.type = (int)m_type;
        m_data.range = m_range;
        m_data.color = new Color(m_color.r*m_intensity, m_color.g*m_intensity, m_color.b*m_intensity, 0.0f);
        switch (m_type)
        {
            case Type.Directional:
                m_data.position = t.forward;
                break;
            case Type.Point:
                m_data.position = t.position;
                break;
        }
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
    }


    void OnDrawGizmos()
    {
        var t = GetComponent<Transform>();
        Gizmos.DrawIcon(t.position, "DirectionalLight Gizmo", true);
    }

    void OnDrawGizmosSelected()
    {
        var t = GetComponent<Transform>();
        Gizmos.color = Color.red;
        Gizmos.DrawLine(t.position, t.position + t.forward);
    }
}

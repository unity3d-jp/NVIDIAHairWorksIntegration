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

    static public HashSet<HairLight> GetInstances()
    {
        if (s_instances == null)
        {
            s_instances = new HashSet<HairLight>();
        }
        return s_instances;
    }
    #endregion


    public enum Type
    {
        Directional,
        Point,
    }

    // public Type m_type; // todo
    public Color m_color = Color.white;
    [Range(0.0f, 8.0f)] float m_intensity = 1.0f;



    void Awake()
    {
        GetInstances().Add(this);
    }

    void OnDestroy()
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

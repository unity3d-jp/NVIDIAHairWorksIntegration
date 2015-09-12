using System;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(HairLight))]
public class HairLightEditor : Editor
{
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
    }
}

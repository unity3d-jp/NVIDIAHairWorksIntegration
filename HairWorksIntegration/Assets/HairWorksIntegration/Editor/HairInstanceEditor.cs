using System;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(HairInstance))]
public class HairInstanceEditor : Editor
{
    public override void OnInspectorGUI()
    {
        var t = target as HairInstance;

        GUILayout.BeginHorizontal();
        if (GUILayout.Button("Load Hair Asset"))
        {
            var path = EditorUtility.OpenFilePanel("Select apx file in StreamingAssets directory", Application.streamingAssetsPath, "apx");
            t.LoadHairAsset(MakeRelativePath(path));
        }
        if (GUILayout.Button("Reload Hair Asset"))
        {
            t.ReloadHairAsset();
        }
        GUILayout.EndHorizontal();

        GUILayout.Space(10);

        GUILayout.BeginHorizontal();
        if (GUILayout.Button("Load Hair Shader"))
        {
            var path = EditorUtility.OpenFilePanel("Select compiled shader (.cso) file in StreamingAssets directory", Application.streamingAssetsPath, "cso");
            t.LoadHairShader(MakeRelativePath(path));
        }
        if (GUILayout.Button("Reload Hair Shader"))
        {
            t.ReloadHairShader();
        }
        GUILayout.EndHorizontal();

        GUILayout.Space(10);

        DrawDefaultInspector();

        GUILayout.Space(10);

        GUILayout.Label(
            "hair shader: " + t.m_hair_shader + "\n" +
            "hair asset: " + t.m_hair_asset + "\n" +
            "shader id: " + HandleToString(t.shader_id) + "\n" +
            "asset id: " + HandleToString(t.asset_id) + "\n" +
            "instance id: " + HandleToString(t.instance_id));
    }

    static string HandleToString(uint h)
    {
        return h == 0xFFFFFFFF ? "(null)" : h.ToString();
    }

    static string MakeRelativePath(string path)
    {
        Uri path_to_assets = new Uri(Application.streamingAssetsPath + "/");
        return path_to_assets.MakeRelativeUri(new Uri(path)).ToString();
    }
}

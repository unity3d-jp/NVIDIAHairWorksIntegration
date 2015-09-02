using System;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(HairInstance))]
public class HairInstanceEditor : Editor
{
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();

        var t = target as HairInstance;
        if (GUILayout.Button("Select Hair Shader"))
        {
            var path = EditorUtility.OpenFilePanel("Select compiled shader (.cso) file in StreamingAssets directory", Application.streamingAssetsPath, "cso");
            t.LoadHairShader(MakeRelativePath(path));
        }
        if (GUILayout.Button("Select Hair Asset"))
        {
            var path = EditorUtility.OpenFilePanel("Select apx file in StreamingAssets directory", Application.streamingAssetsPath, "apx");
            t.LoadHairAsset(MakeRelativePath(path));
        }
        GUILayout.Label(
            "hair shader: " + t.m_hair_shader + "\n" +
            "hair asset: " + t.m_hair_asset + "\n" +
            "shader id: " + t.shader_id + "\n" +
            "asset id: " + t.asset_id + "\n" +
            "instance id: " + t.instance_id);
    }


    static string MakeRelativePath(string path)
    {
        Uri path_to_assets = new Uri(Application.streamingAssetsPath + "/");
        return path_to_assets.MakeRelativeUri(new Uri(path)).ToString();
    }
}

#if UNITY_EDITOR
using System;
using System.Collections;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;


public class Packaging
{
    [MenuItem("Assets/Hair Works Integration/Make Package")]
    public static void MakePackage()
    {
        string[] files = new string[]
        {
"Assets/UTJ/HairWorksIntegration",
"Assets/StreamingAssets/UTJ/HairWorksIntegration",
"Assets/smcs.rsp",
        };
        AssetDatabase.ExportPackage(files, "MassParticle.unitypackage", ExportPackageOptions.Recurse);
    }

}
#endif

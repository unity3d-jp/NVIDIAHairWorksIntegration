using System;
using UnityEngine;
using UnityEditor;

namespace UTJ
{
    static class MiscDebugMenu
    {

        [MenuItem("GameObject/Dump Transform")]
        static void DumpTransform()
        {
            var go = Selection.activeGameObject;
            if(go == null) { return; }

            var t = go.GetComponent<Transform>();
            string message = "";
            message += string.Format("position: {0}\n", t.position.ToString("0.00000"));
            message += string.Format("localPosition: {0}\n", t.localPosition.ToString("0.00000"));
            message += string.Format("eulerAngles: {0}\n", t.eulerAngles.ToString("0.00000"));
            message += string.Format("localEulerAngles: {0}\n", t.localEulerAngles.ToString("0.00000"));
            message += string.Format("rotation: {0}\n", t.rotation.ToString("0.00000"));
            message += string.Format("localRotation: {0}\n", t.localRotation.ToString("0.00000"));
            message += string.Format("lossyScale: {0}\n", t.lossyScale.ToString("0.00000"));
            message += string.Format("localScale: {0}\n", t.localScale.ToString("0.00000"));
            message += string.Format("localToWorld:\n{0}", t.localToWorldMatrix);
            Debug.Log(message);
        }
    }

} // namespace UTJ

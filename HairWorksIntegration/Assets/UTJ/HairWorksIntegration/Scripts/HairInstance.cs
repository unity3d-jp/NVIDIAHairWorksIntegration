using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Rendering;
#if UNITY_EDITOR
using UnityEditor;
#endif


namespace UTJ
{
    [AddComponentMenu("UTJ/Hair Works Integration/Hair Instance")]
    [ExecuteInEditMode]
    public class HairInstance : MonoBehaviour
    {
        #region static
        static HashSet<HairInstance> s_instances;
        static int s_nth_LateUpdate;
        static int s_nth_OnWillRenderObject;

        static CommandBuffer s_command_buffer;
        static HashSet<Camera> s_cameras = new HashSet<Camera>();

        static public HashSet<HairInstance> GetInstances()
        {
            if (s_instances == null)
            {
                s_instances = new HashSet<HairInstance>();
            }
            return s_instances;
        }
        #endregion

        static CameraEvent s_timing = CameraEvent.BeforeImageEffects;

        public string m_hair_asset;
        public string m_hair_shader = "UTJ/HairWorksIntegration/DefaultHairShader.cso";
        public Transform m_root_bone;
        public bool m_invert_bone_x = true;
        public hwi.Descriptor m_params = hwi.Descriptor.default_value;
        hwi.HShader m_hshader = hwi.HShader.NullHandle;
        hwi.HAsset m_hasset = hwi.HAsset.NullHandle;
        hwi.HInstance m_hinstance = hwi.HInstance.NullHandle;

        public Transform[] m_bones;
        Matrix4x4[] m_inv_bindpose;
        Matrix4x4[] m_skinning_matrices;
        IntPtr m_skinning_matrices_ptr;
        Matrix4x4 m_conversion_matrix;

        public Mesh m_probe_mesh;


        public uint shader_id { get { return m_hshader; } }
        public uint asset_id { get { return m_hasset; } }
        public uint instance_id { get { return m_hinstance; } }



        void RepaintWindow()
        {
#if UNITY_EDITOR
            var assembly = typeof(UnityEditor.EditorWindow).Assembly;
            var type = assembly.GetType("UnityEditor.GameView");
            var gameview = EditorWindow.GetWindow(type);
            gameview.Repaint();
#endif
        }

        public void LoadHairShader(string path_to_cso)
        {
            // release existing shader
            if (m_hshader)
            {
                hwi.hwShaderRelease(m_hshader);
                m_hshader = hwi.HShader.NullHandle;
            }

            // load shader
            if (m_hshader = hwi.hwShaderLoadFromFile(Application.streamingAssetsPath + "/" + path_to_cso))
            {
                m_hair_shader = path_to_cso;
            }
#if UNITY_EDITOR
            RepaintWindow();
#endif
        }

        public void ReloadHairShader()
        {
            hwi.hwShaderReload(m_hshader);
            RepaintWindow();
        }

        public void LoadHairAsset(string path_to_apx, bool reset_params = true)
        {
            // release existing instance & asset
            if (m_hinstance)
            {
                hwi.hwInstanceRelease(m_hinstance);
                m_hinstance = hwi.HInstance.NullHandle;
            }
            if (m_hasset)
            {
                hwi.hwAssetRelease(m_hasset);
                m_hasset = hwi.HAsset.NullHandle;
            }

            // load & create instance
            if (m_hasset = hwi.hwAssetLoadFromFile(Application.streamingAssetsPath + "/" + path_to_apx))
            {
                m_hair_asset = path_to_apx;
                m_hinstance = hwi.hwInstanceCreate(m_hasset);
                if (reset_params)
                {
                    hwi.hwAssetGetDefaultDescriptor(m_hasset, ref m_params);
                }
            }

            // update bone structure
            if (reset_params)
            {
                m_bones = null;
                m_skinning_matrices = null;
                m_skinning_matrices_ptr = IntPtr.Zero;
            }
            UpdateBones();

#if UNITY_EDITOR
            Update();
            RepaintWindow();
#endif
        }


        public void ReloadHairAsset()
        {
            hwi.hwAssetReload(m_hasset);
            hwi.hwAssetGetDefaultDescriptor(m_hasset, ref m_params);
            hwi.hwInstanceSetDescriptor(m_hinstance, ref m_params);
            RepaintWindow();
        }

        public void AssignTexture(hwi.TextureType type, Texture2D tex)
        {
            hwi.hwInstanceSetTexture(m_hinstance, type, tex.GetNativeTexturePtr());
        }

        public void UpdateBones()
        {
            int num_bones = hwi.hwAssetGetNumBones(m_hasset);
            if (m_bones == null || m_bones.Length != num_bones)
            {
                m_bones = new Transform[num_bones];

                if (m_root_bone == null)
                {
                    m_root_bone = GetComponent<Transform>();
                }

                var children = m_root_bone.GetComponentsInChildren<Transform>();
                for (int i = 0; i < num_bones; ++i)
                {
                    string name = hwi.hwAssetGetBoneNameString(m_hasset, i);
                    m_bones[i] = Array.Find(children, (a) => { return a.name == name; });
                    if (m_bones[i] == null) { m_bones[i] = m_root_bone; }
                }

            }

            if (m_skinning_matrices == null)
            {
                m_inv_bindpose = new Matrix4x4[num_bones];
                m_skinning_matrices = new Matrix4x4[num_bones];
                m_skinning_matrices_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(m_skinning_matrices, 0);
                for (int i = 0; i < num_bones; ++i)
                {
                    m_inv_bindpose[i] = Matrix4x4.identity;
                    m_skinning_matrices[i] = Matrix4x4.identity;
                }

                for (int i = 0; i < num_bones; ++i)
                {
                    hwi.hwAssetGetBindPose(m_hasset, i, ref m_inv_bindpose[i]);
                    m_inv_bindpose[i] = m_inv_bindpose[i].inverse;
                }

                m_conversion_matrix = Matrix4x4.identity;
                if (m_invert_bone_x)
                {
                    m_conversion_matrix *= Matrix4x4.Scale(new Vector3(-1.0f, 1.0f, 1.0f));
                }
            }


            for (int i = 0; i < m_bones.Length; ++i)
            {
                var t = m_bones[i];
                if (t != null)
                {
                    m_skinning_matrices[i] = t.localToWorldMatrix * m_conversion_matrix * m_inv_bindpose[i];
                }
            }
        }

        static public void Swap<T>(ref T a, ref T b)
        {
            T tmp = a;
            a = b;
            b = tmp;
        }



#if UNITY_EDITOR
        void Reset()
        {
            var skinned_mesh_renderer = GetComponent<SkinnedMeshRenderer>();
            m_root_bone = skinned_mesh_renderer != null ? skinned_mesh_renderer.rootBone : GetComponent<Transform>();

            var renderer = GetComponent<Renderer>();
            if (renderer == null)
            {
                m_probe_mesh = new Mesh();
                m_probe_mesh.name = "Probe";
                m_probe_mesh.vertices = new Vector3[1] { Vector3.zero };
                m_probe_mesh.SetIndices(new int[1] { 0 }, MeshTopology.Points, 0);

                var mesh_filter = gameObject.AddComponent<MeshFilter>();
                mesh_filter.sharedMesh = m_probe_mesh;
                renderer = gameObject.AddComponent<MeshRenderer>();
                renderer.sharedMaterials = new Material[0] { };
            }
        }
#endif

        void OnApplicationQuit()
        {
            ClearCommandBuffer();
        }

        void Awake()
        {
#if UNITY_EDITOR
            if(!hwi.hwLoadHairWorks())
            {
                EditorUtility.DisplayDialog(
                    "Hair Works Integration",
                    "Failed to load HairWorks (version " + hwi.hwGetSDKVersion() + ") dll. You need to get HairWorks SDK from NVIDIA website. Read document for more detail.",
                    "OK");
            }
#endif
            hwi.hwSetLogCallback();
        }

        void OnDestroy()
        {
            hwi.hwInstanceRelease(m_hinstance);
            hwi.hwAssetRelease(m_hasset);
        }

        void OnEnable()
        {
            GetInstances().Add(this);
            m_params.m_enable = true;
        }

        void OnDisable()
        {
            m_params.m_enable = false;
            GetInstances().Remove(this);
        }

        void Start()
        {
            LoadHairShader(m_hair_shader);
            LoadHairAsset(m_hair_asset, false);
        }

        void Update()
        {
            if (!m_hasset) { return; }

            UpdateBones();
            hwi.hwInstanceSetDescriptor(m_hinstance, ref m_params);
            hwi.hwInstanceUpdateSkinningMatrices(m_hinstance, m_skinning_matrices.Length, m_skinning_matrices_ptr);

            if (m_probe_mesh != null)
            {
                var bmin = Vector3.zero;
                var bmax = Vector3.zero;
                hwi.hwInstanceGetBounds(m_hinstance, ref bmin, ref bmax);

                var center = (bmin + bmax) * 0.5f;
                var size = bmax - center;
                m_probe_mesh.bounds = new Bounds(center, size);
            }


            s_nth_LateUpdate = 0;
        }

        void LateUpdate()
        {
            if (s_nth_LateUpdate++ == 0)
            {
                hwi.hwStepSimulation(Time.deltaTime);
            }
        }

        void OnWillRenderObject()
        {
            if (s_nth_OnWillRenderObject++ == 0)
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




        static public bool IsDeferred(Camera cam)
        {
            if (cam.renderingPath == RenderingPath.DeferredShading
#if UNITY_EDITOR
            || (cam.renderingPath == RenderingPath.UsePlayerSettings && PlayerSettings.renderingPath == RenderingPath.DeferredShading)
#endif
            )
            {
                return true;
            }
            return false;
        }

        static public bool DoesRenderToTexture(Camera cam)
        {
            return IsDeferred(cam) || cam.targetTexture != null;
        }


        static public void ClearCommandBuffer()
        {
            foreach (var c in s_cameras)
            {
                if (c != null)
                {
                    c.RemoveCommandBuffer(s_timing, s_command_buffer);
                }
            }
            s_cameras.Clear();
        }

        static void BeginRender()
        {
            if (s_command_buffer == null)
            {
                s_command_buffer = new CommandBuffer();
                s_command_buffer.name = "Hair";
                s_command_buffer.IssuePluginEvent(hwi.hwGetRenderEventFunc(), 0);
            }

            var cam = Camera.current;
            if (cam != null)
            {
                Matrix4x4 V = cam.worldToCameraMatrix;
                Matrix4x4 P = GL.GetGPUProjectionMatrix(cam.projectionMatrix, DoesRenderToTexture(cam));
                float fov = cam.fieldOfView;
                hwi.hwSetViewProjection(ref V, ref P, fov);
                HairLight.AssignLightData();

                if (!s_cameras.Contains(cam))
                {
                    cam.AddCommandBuffer(s_timing, s_command_buffer);
                    s_cameras.Add(cam);
                }
            }

            hwi.hwBeginScene();
        }

        void Render()
        {
            if (!m_hasset) { return; }

            hwi.hwSetShader(m_hshader);
            hwi.hwRender(m_hinstance);
        }

        static void EndRender()
        {
            hwi.hwEndScene();
        }

    }

}

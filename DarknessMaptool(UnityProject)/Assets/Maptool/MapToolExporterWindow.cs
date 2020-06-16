using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;
using UnityEngine.SceneManagement;
using UnityEditorInternal;

namespace YH_Cutstom
{

    public class MapToolExporterWindow : EditorWindow
    {
        const string WND_TITLE = "MapInfoExporter";
        static List<GameObject> allGameObjects = new List<GameObject>();
        static List<GameObject> targetStaticObjects = new List<GameObject>();
        static List<GameObject> targetSkinnedObjects = new List<GameObject>();
        static List<GameObject> targetInstancedbObjects = new List<GameObject>();
        static string strExportPath = "";
        Vector2 scrollPos1;
        Vector2 scrollPos2;
        Vector2 scrollPos3;

        bool staticGroupEnable = false;
        bool skinnedGroupEnable = false;
        bool instancedGroupEnable = false;
        int selected = 0;
        static public void ShowWindow()
        {
            MapToolExporterWindow window = EditorWindow.GetWindow(typeof(MapToolExporterWindow)) as MapToolExporterWindow;
            window.title = WND_TITLE;
            window.Show();
            //allGameObjects = FindObjectsOfType<GameObject>().ToList();
            allGameObjects = GameObject.FindObjectsOfType<GameObject>().ToList();

            targetStaticObjects = allGameObjects
                .Where(n => n.tag == "StaticObject")
                .OrderBy(n => n.name).ToList();

            targetSkinnedObjects = allGameObjects
               .Where(n => n.tag == "SkinnedObject")
               .OrderBy(n => n.name).ToList();

            targetInstancedbObjects = allGameObjects
               .Where(n => n.tag == "InstancedObject")
               .OrderBy(n => n.name).ToList();

        }
        private void DrawObjName(string toggleGroupName,string objName,List<GameObject> list,ref Vector2 scrolPos,ref bool groupEnable)
        {

            //groupEnable = EditorGUILayout.BeginToggleGroup(toggleGroupName, groupEnable);
            scrolPos = GUILayout.BeginScrollView(scrolPos, false, true);
            for (int i = 0; i < list.Count; ++i)
            {
                CreateSelectLabel(objName, list[i].name);

            }
            GUILayout.EndScrollView();
            //EditorGUILayout.EndToggleGroup();
        }
        private void OnGUI()
        {

            GUILayout.BeginHorizontal();
            CreateSelectLabel("출력 경로", strExportPath);
            if (GUILayout.Button("경로 설정"))
            {
                strExportPath = EditorUtility.SaveFilePanel("","","MapInfo","yhMap");
            }
            GUILayout.EndHorizontal();


            selected = GUILayout.Toolbar(selected, new string[] { "static", "skinned", "instanced" });
            
            if(selected == 0)
            DrawObjName("static", "", targetStaticObjects,ref scrollPos1,ref staticGroupEnable);
            if(selected == 1)
            DrawObjName("skinned", "", targetSkinnedObjects,ref scrollPos2, ref skinnedGroupEnable);
            if(selected == 2)
            DrawObjName("instanced", "", targetInstancedbObjects,ref scrollPos3, ref instancedGroupEnable);


            if (GUILayout.Button("export Map data"))
            {
                if (strExportPath == "")
                {
                    PopupWindow.Init("경로를 설정해주세요", "확인");
                    return;
                }
                var wrriter = new MapFileWrriter();
                wrriter.WriteData(strExportPath, targetStaticObjects, targetSkinnedObjects, targetInstancedbObjects);
                PopupWindow.Init("맵파일 생성 완료\n아래 경로를 확인하세요\n" + strExportPath, "확인");
            }
        }

        private void CreateSelectLabel(string title, string content, bool btnCreate = false, string btnName = "", System.Action<string> a = null)
        { //
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField(title, EditorStyles.boldLabel);
            // 높이 재수정 
            Rect position = EditorGUILayout.GetControlRect(false, 15f);
            EditorGUI.SelectableLabel(position, string.Format("{0}", content));
            if (btnCreate)
            {
                // 클립보드에 복사 
                if (GUILayout.Button(btnName))
                {
                    a(content);
                }
            }
            //EditorGUILayout.Space();
            EditorGUILayout.EndHorizontal();
        }
        private static string GetNewPath(GameObject gameObject, string oldPath = null)
        {
            // NOTE: This must return a path with the starting "Assets/" or else textures won't copy right

            string name = gameObject.name;

            string newPath = null;
            if (oldPath == null)
                newPath = EditorUtility.SaveFilePanel("Export File", name + ".yhMap", "yhMap", "Export " + name + " GameObject to a yhMap file");
            else
            {
                if (oldPath.StartsWith("/Assets"))
                {
                    oldPath = Application.dataPath.Remove(Application.dataPath.LastIndexOf("/Assets"), 7) + oldPath;
                    oldPath = oldPath.Remove(oldPath.LastIndexOf('/'), oldPath.Length - oldPath.LastIndexOf('/'));
                }
                newPath = EditorUtility.SaveFilePanel("Export yhfmt File", oldPath, name + ".yhfmt", "yhfmt");
            }

            int assetsIndex = newPath.IndexOf("Assets");

            if (assetsIndex < 0)
                return null;

            if (assetsIndex > 0)
                newPath = newPath.Remove(0, assetsIndex);

            return newPath;
        }
    }

}

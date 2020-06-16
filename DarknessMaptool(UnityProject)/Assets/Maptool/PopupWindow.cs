using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace YH_Cutstom
{
    public class PopupWindow : EditorWindow
    {
        static string m_strDesc = "";
        static string m_strButton = "";

        static public void Init(string desc, string button)
        {
            PopupWindow window = ScriptableObject.CreateInstance<PopupWindow>();
            window.position = new Rect(Screen.width / 2, Screen.height / 2, 250, 150);
            window.ShowPopup();
            m_strDesc = desc;
            m_strButton = button;

        }

        void OnGUI()
        {
            EditorGUILayout.LabelField(m_strDesc, EditorStyles.wordWrappedLabel);
            GUILayout.Space(70);
            if (GUILayout.Button(m_strButton)) this.Close();
        }

    }

}

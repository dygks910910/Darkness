using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
namespace YH_Cutstom
{
    public class MaptoolEditor : Editor
    {
        [MenuItem("Window/MaptoolExporterWindow")]
        static public void CreateWindow()
        {
            YH_Cutstom.MapToolExporterWindow.ShowWindow();
        }
    }

}

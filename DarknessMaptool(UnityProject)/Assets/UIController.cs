using UnityEngine;
using System.Collections;
using System.IO;
using System.Diagnostics;


public class UIController : MonoBehaviour
{
    string m_strPath = "D:\\Project\\Darkness\\Client";
    //     string m_strPath = "D:/Project/DarknessMaptool";

    // Update is called once per frame
    public void WriteWallAABB()
    {
        GameObject[] staticObjects = GameObject.FindGameObjectsWithTag("AABB");
        string str = "";
        FileStream f = new FileStream(m_strPath + "\\NewAABBData.txt", FileMode.Create, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f);

        ///헤더정보 출력:
        WriteHeader(writer, staticObjects.Length, 0, 0);
        //camera object +1해줌.
        WriteStaticObjectsAABB(writer, staticObjects);
        writer.WriteLine("//////////////////////StaticObjects//////////////////////");
        writer.WriteLine("//////////////////////InstancedObjects//////////////////////");
        writer.WriteLine("//////////////////////SkinnedObjects//////////////////////");

        writer.Close();
        print("Write Done!!");
    }
    public void WriteData()
    {
        GameObject[] staticObjects = GameObject.FindGameObjectsWithTag("StaticObject");
        GameObject[] skinnedObjects = GameObject.FindGameObjectsWithTag("SkinnedObject");
        GameObject[] instancedObjects = GameObject.FindGameObjectsWithTag("InstancedObject");
        FileStream f = new FileStream(m_strPath + "\\NewMapData2.txt", FileMode.Create, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f);

        ///헤더정보 출력:
        WriteHeader(writer, staticObjects.Length, skinnedObjects.Length, instancedObjects.Length);
        //camera object +1해줌.
        WriteStaticObjects(writer, staticObjects);
        WriteinstancedObjects(writer, instancedObjects);
        WriteSkinnedObjects(writer, skinnedObjects);
        writer.Close();
        print("Write Done!!");
    }
    public void WriteDataPlusAABB()
    {
        GameObject[] staticObjects = GameObject.FindGameObjectsWithTag("StaticObject");
        GameObject[] skinnedObjects = GameObject.FindGameObjectsWithTag("SkinnedObject");
        GameObject[] instancedObjects = GameObject.FindGameObjectsWithTag("InstancedObject");
        FileStream f = new FileStream(m_strPath + "\\NewMapDataAndAABB3.txt", FileMode.Create, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f);

        ///헤더정보 출력:
        WriteHeader(writer, staticObjects.Length, skinnedObjects.Length, instancedObjects.Length);
        //camera object +1해줌.
        WriteStaticObjectsAABB(writer, staticObjects);
        WriteinstancedObjectsAABB(writer, instancedObjects);
        WriteSkinnedObjectsAABB(writer, skinnedObjects);
        writer.Close();
        print("Write Done!!");
    }
    private void WriteHeader(StreamWriter writer, int staticObjectCount, int skinnedObjectCount, int instancedObjectCount)
    {
        string writeStr;
        writer.WriteLine("//////////////////////Header//////////////////////");
        writeStr = "StaticObjectCount: " + (staticObjectCount) + "\r";
        writer.WriteLine(writeStr);
        writeStr = "SkinnedObjectCount: " + skinnedObjectCount + "\r";
        writer.WriteLine(writeStr);
        writeStr = "InstancedObjectCount: " + instancedObjectCount + "\r";
        writer.WriteLine(writeStr);
    }

    private void WriteStaticObjects(StreamWriter writer, GameObject[] staticObjects)
    {
        GameObject Camera = GameObject.FindGameObjectWithTag("MainCamera");

        Transform transf;
        string writeStr = "";
        //camera object +1해줌.
        writer.WriteLine("//////////////////////StaticObjects//////////////////////");
        writer.WriteLine(writeStr);
        //         transf = Camera.GetComponent<Transform>();
        //         writeStr = Camera.name + "\r\n" +"T: "+ transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
        //             "R: "+transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() +" " +transf.rotation.w.ToString() + "\r\n" +
        //             "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";
        //         writer.WriteLine(writeStr);
        for (int i = 0; i < staticObjects.Length; ++i)
        {
            transf = staticObjects[i].GetComponent<Transform>();
            writeStr = staticObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";

            writer.WriteLine(writeStr);
        }

    }
    private void WriteStaticObjectsAABB(StreamWriter writer, GameObject[] staticObjects)
    {
        GameObject Camera = GameObject.FindGameObjectWithTag("MainCamera");

        Transform transf;
        string writeStr = "";
        //camera object +1해줌.
        writer.WriteLine("//////////////////////StaticObjects//////////////////////");
        writer.WriteLine(writeStr);
        //         transf = Camera.GetComponent<Transform>();
        //         writeStr = Camera.name + "\r\n" +"T: "+ transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
        //             "R: "+transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() +" " +transf.rotation.w.ToString() + "\r\n" +
        //             "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";
        //         writer.WriteLine(writeStr);
        for (int i = 0; i < staticObjects.Length; ++i)
        {
            transf = staticObjects[i].GetComponent<Transform>();
            writeStr = staticObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() ;
            writer.WriteLine(writeStr);
            WriteAABB(writer, staticObjects[i]);
        }

    }
    private void WriteSkinnedObjects(StreamWriter writer, GameObject[] skinnedObjects)
    {
        writer.WriteLine("//////////////////////SkinnedObjects//////////////////////");

        Transform transf;
        string writeStr = "";
        for (int i = 0; i < skinnedObjects.Length; ++i)
        {
            transf = skinnedObjects[i].GetComponent<Transform>();
            writeStr = skinnedObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";
            writer.WriteLine(writeStr);

            Transform[] tempTransforms = skinnedObjects[i].GetComponentsInChildren<Transform>();

            foreach (Transform child in tempTransforms)
            {
                if (child.name.Contains("CameraOffset"))
                {
                    writeStr = skinnedObjects[i].name + "cameraOffset" + "\r\n" + "T: " + child.transform.position.x.ToString() + " " + child.transform.position.y.ToString() + " " + child.transform.position.z.ToString() + "\r\n";
                    writer.WriteLine(writeStr);
                    ;

                    break;
                }
            }



        }
    }
    private void WriteSkinnedObjectsAABB(StreamWriter writer, GameObject[] skinnedObjects)
    {
        writer.WriteLine("//////////////////////SkinnedObjects//////////////////////");

        Transform transf;
        string writeStr = "";
        for (int i = 0; i < skinnedObjects.Length; ++i)
        {
            transf = skinnedObjects[i].GetComponent<Transform>();
            writeStr = skinnedObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString();
            writer.WriteLine(writeStr);
            WriteAABB(writer, skinnedObjects[i]);
            Transform[] tempTransforms = skinnedObjects[i].GetComponentsInChildren<Transform>();

            foreach (Transform child in tempTransforms)
            {
                if (child.name.Contains("CameraOffset"))
                {
                    writeStr = skinnedObjects[i].name + "cameraOffset" + "\r\n" + "T: " + child.transform.position.x.ToString() + " " + child.transform.position.y.ToString() + " " + child.transform.position.z.ToString() + "\r\n";
                    writer.WriteLine(writeStr);
                   
                    

                    break;
                }
            }



        }
    }
    private void WriteinstancedObjects(StreamWriter writer, GameObject[] instancedObjects)
    {
        writer.WriteLine("//////////////////////InstancedObjects//////////////////////");

        Transform transf;
        string writeStr = "";
        for (int i = 0; i < instancedObjects.Length; ++i)
        {
            transf = instancedObjects[i].GetComponent<Transform>();
            writeStr = instancedObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";

            writer.WriteLine(writeStr);
        }
    }
    private void WriteinstancedObjectsAABB(StreamWriter writer, GameObject[] instancedObjects)
    {
        writer.WriteLine("//////////////////////InstancedObjects//////////////////////");

        Transform transf;
        string writeStr = "";
        for (int i = 0; i < instancedObjects.Length; ++i)
        {
            transf = instancedObjects[i].GetComponent<Transform>();
            writeStr = instancedObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
            "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
            "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString();

            writer.WriteLine(writeStr);
            WriteAABB(writer, instancedObjects[i]);
        }
    }
    //private void WriteMultiMaterialObjects(StreamWriter writer, GameObject[] multiMaterial)
    //{
    //    writer.WriteLine("//////////////////////MultiMaterialObjects//////////////////////");

    //    Transform transf;
    //    string writeStr = "";
    //    for (int i = 0; i < multiMaterial.Length; ++i)
    //    {
    //        transf = multiMaterial[i].GetComponent<Transform>();
    //        writeStr = multiMaterial[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
    //        "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
    //        "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";

    //        writer.WriteLine(writeStr);

    //        Material[] materials = multiMaterial[i].GetComponent<MeshRenderer>().materials;
    //        writeStr = materials.Length.ToString();
    //        writer.WriteLine("MatrialCount: "+writeStr);
    //        for (int j = 0; j < materials.Length; ++j)
    //        {
    //            writeStr = "MaterialName: " + materials[j].name;
    //            writer.WriteLine(writeStr);

    //            writeStr = "MaterialFileName: " + materials[j].GetTexture("_MainTex").ToString() + "\r\n" +
    //                "NMaterialFileName: " + materials[j].GetTexture("_BumpMap").ToString() + "\r\n";
    //            writer.WriteLine(writeStr);
    //        }

    //    }
    //}
    private void WriteAABB(StreamWriter writer, GameObject obj)
    {
        string writeStr = "";
        BoxCollider tempBoxCollider = obj.GetComponent<BoxCollider>();
        writeStr = "extent " + (tempBoxCollider.size.x/2).ToString() + " " + (tempBoxCollider.size.y/2).ToString() + " " + (tempBoxCollider.size.z/2).ToString() + "\r\n"+
            "center " + tempBoxCollider.center.x + " " + tempBoxCollider.center.y + " " + tempBoxCollider.center.z + "\r\n";
        writer.WriteLine(writeStr);
    }
    
}

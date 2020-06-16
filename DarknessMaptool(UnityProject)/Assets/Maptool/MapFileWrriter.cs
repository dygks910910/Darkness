using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Diagnostics;
namespace YH_Cutstom
{

    public class MapFileWrriter
    {
        public void WriteData(string strPath,List<GameObject> staticObjs, List<GameObject> skinnedObjs, List<GameObject> instancedObjs)
        {
            //GameObject[] staticObjects = GameObject.FindGameObjectsWithTag("StaticObject");
            //GameObject[] skinnedObjects = GameObject.FindGameObjectsWithTag("SkinnedObject");
            //GameObject[] instancedObjects = GameObject.FindGameObjectsWithTag("InstancedObject");
            FileStream f = new FileStream(strPath, FileMode.Create, FileAccess.Write);
            StreamWriter writer = new StreamWriter(f);

            ///헤더정보 출력:
            WriteHeader(writer, staticObjs.Count, skinnedObjs.Count, instancedObjs.Count);
            //camera object +1해줌.
            WriteStaticObjects(writer, staticObjs);
            WriteinstancedObjects(writer, instancedObjs);
            WriteSkinnedObjects(writer, skinnedObjs);
            writer.Close();
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
        private void WriteStaticObjects(StreamWriter writer, List<GameObject> staticObjects)
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
            for (int i = 0; i < staticObjects.Count; ++i)
            {
                transf = staticObjects[i].GetComponent<Transform>();
                writeStr = staticObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
                "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
                "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";

                writer.WriteLine(writeStr);
            }

        }
        private void WriteSkinnedObjects(StreamWriter writer, List<GameObject> skinnedObjects)
        {
            writer.WriteLine("//////////////////////SkinnedObjects//////////////////////");

            Transform transf;
            string writeStr = "";
            for (int i = 0; i < skinnedObjects.Count; ++i)
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
        private void WriteinstancedObjects(StreamWriter writer, List<GameObject> instancedObjects)
        {
            writer.WriteLine("//////////////////////InstancedObjects//////////////////////");

            Transform transf;
            string writeStr = "";
            for (int i = 0; i < instancedObjects.Count; ++i)
            {
                transf = instancedObjects[i].GetComponent<Transform>();
                writeStr = instancedObjects[i].name + "\r\n" + "T: " + transf.position.x.ToString() + " " + transf.position.y.ToString() + " " + transf.position.z.ToString() + "\r\n" +
                "R: " + transf.rotation.x.ToString() + " " + transf.rotation.y.ToString() + " " + transf.rotation.z.ToString() + " " + transf.rotation.w.ToString() + "\r\n" +
                "S: " + transf.localScale.x.ToString() + " " + transf.localScale.y.ToString() + " " + transf.localScale.z.ToString() + "\r\n";

                writer.WriteLine(writeStr);
            }
        }
    }

}

#include "stdafx.h"
#include "LoadFBX.h"


CLoadFBX::CLoadFBX(char* fileName)
{
	std::cout << "\nA model has been built!";

	numIndices = 0;

	FbxManager *manager = FbxManager::Create();

	FbxIOSettings *ioSettings = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ioSettings);

	FbxImporter *importer = FbxImporter::Create(manager, "");
	importer->Initialize(fileName, -1, manager->GetIOSettings());

	FbxScene *scene = FbxScene::Create(manager, "tempName");

	importer->Import(scene);
	importer->Destroy();

	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) { this->GetFbxInfo(rootNode); }

}


CLoadFBX::~CLoadFBX()
{
}

void CLoadFBX::ShowDetails()
{
	std::cout << "\nName:" << Name;
	std::cout << "\nVertices Number:" << vertices.size();
	std::cout << "\nIndices Number:" << numIndices;
}

char * CLoadFBX::GetModelName()
{
	return Name;
}


void CLoadFBX::GetFbxInfo(FbxNode *Node)
{
	int numKids = Node->GetChildCount();
	FbxNode *childNode = 0;
	XMFLOAT3 tempVertex;
	XMFLOAT3 tempNormal;
	XMFLOAT2 tempUV;
	std::vector<XMFLOAT2> UVList;



	for (int i = 0; i < numKids; i++)
	{
		childNode = Node->GetChild(i);
		FbxMesh *mesh = childNode->GetMesh();

		if (mesh != NULL)
		{
			//================= Get Vertices ====================================
			int numVerts = mesh->GetControlPointsCount();
			vertices.resize(numVerts);
			for (int j = 0; j < numVerts; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				tempVertex = XMFLOAT3((float)vert.mData[1], 
					(float)vert.mData[0],
					(float)vert.mData[2]);
				vertices[i].Pos = tempVertex;

			}
			//================= Get Indices ====================================
			numIndices = mesh->GetPolygonVertexCount();
			indices = new int[numIndices];
			indices = mesh->GetPolygonVertices();
			//================= Get Normals ====================================


			FbxGeometryElementNormal* normalEl = mesh->GetElementNormal();
			if (normalEl)
			{
				numNormals = mesh->GetPolygonCount();
				normals = new float[numNormals];
				int vertexCounter = 0;
				for (int polyCounter = 0; polyCounter < mesh->GetPolygonCount(); polyCounter++)
				{
					for (int i = 0; i < 3; i++)
					{
						FbxVector4 normal = normalEl->GetDirectArray().GetAt(vertexCounter);
					
						tempNormal = XMFLOAT3(normal[0], normal[1], normal[2]);
						vertexCounter++;
					}
				}
			}
			FbxGeometryElementUV* uvEl = mesh->GetElementUV();
			if (uvEl)
			{
				if (uvEl->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
					uvEl->GetMappingMode() != FbxGeometryElement::eByControlPoint)
					return;

				//// Loop for each poly
				for (int Poly(0); Poly < mesh->GetPolygonCount(); Poly++)
				{
					// Get number of verts in this poly
					const int NumVertices = mesh->GetPolygonSize(Poly);

					// Loop for each vert in poly
					for (int Vertex(0); Vertex < NumVertices; Vertex++)
					{
						FbxVector2 fbxTexCoord;
						FbxStringList UVSetNameList;

						// Get the name of each set of UV coords
						mesh->GetUVSetNames(UVSetNameList);
						bool lUnmappedUV;
						// Get the UV coords for this vertex in this poly which belong to the first UV set
						// Note: Using 0 as index into UV set list as this example supports only one UV set
						if (mesh->GetPolygonVertexUV(Poly, Vertex,
							UVSetNameList.GetStringAt(0), fbxTexCoord, lUnmappedUV))
						{

							// Convert to floats
							float UVCoord[2];
							UVCoord[0] = static_cast<float>(fbxTexCoord[0]);
							UVCoord[1] = static_cast<float>(fbxTexCoord[1]);

							tempUV = XMFLOAT2(UVCoord[0], UVCoord[1]);

							UVList.push_back(tempUV);
							// Store UV
						}
					}
				}
				std::cout << mesh->GetTextureUVCount(FbxLayerElement::eTextureDiffuse);
				/////////////////////////////////////

				
			}
			this->GetFbxInfo(childNode);
			Vertex::Basic32 tempNode;
			tempNode.Pos = tempVertex;
			tempNode.Normal = tempNormal;
			tempNode.Tex = tempUV;

			vertices.push_back(tempNode);
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "³ë¸ÖÀÇ °¹¼ö:" << numNormals << std::endl;
	std::cout << "UV °¹¼ö: " << UVList.size() << std::endl;
	std::cout << "Index °¹¼ö: " << numIndices << std::endl;

// 	for (int i = 0; i < UVList.size(); ++i)
// 	{
// 		std::cout << UVList[i].x <<" " << UVList[i].y ;
// 	}
}

#include "FbxLoader.h"


CFbxLoader::CFbxLoader() :
	mpManager(nullptr), mpScene(nullptr), mpImporter(nullptr),mRootNode(0),
	mlFileMajor(0), mlFileMinor(0), mlFileRevision(0), mHasNormal(false), 
	mHasUV(false), mAllByControlPoint(true)
{
}


CFbxLoader::~CFbxLoader()
{
	
}
void CFbxLoader::Init(const char * pFileName)
{
	//매니저 생성
	mpManager = FbxManager::Create();
	if (!mpManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", mpManager->GetVersion());
	//io세팅 생성.
	FbxIOSettings* ios = FbxIOSettings::Create(mpManager, IOSROOT);
	mpManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	mpManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	mpScene = FbxScene::Create(mpManager, "My Scene");
	if (!mpScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
	std::string strWinMsg;
	// Create the importer.
	int lFileFormat = -1;
	mpImporter = FbxImporter::Create(mpManager, "");
	if (!mpManager->GetIOPluginRegistry()->DetectReaderFileFormat(pFileName, lFileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = mpManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
	}
	if (mpImporter->Initialize(pFileName, lFileFormat) == true)
	{
		// The file is going to be imported at 
		// the end of the first display callback.
		strWinMsg = "Importing file ";
		strWinMsg += pFileName;
		strWinMsg += "\nPlease wait!";

		// Set scene status flag to ready to load.
		//mStatus = MUST_BE_LOADED;
	}
	else
	{
		strWinMsg = "Unable to open file ";
		strWinMsg += pFileName;
		strWinMsg += "\nError reported: ";
		strWinMsg += mpImporter->GetStatus().GetErrorString();
		strWinMsg += "\nEsc to exit";
	}
	mpImporter->GetFileVersion(mlFileMajor, mlFileMinor, mlFileRevision);
	std::cout << strWinMsg << std::endl;
	std::cout << "파일버전:"<<mlFileMajor << "." << mlFileMinor << "." << mlFileRevision << std::endl;
	mRootNode =  mpScene->GetRootNode();
	mpImporter->Import(mpScene);
}

void CFbxLoader::Print()
{
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	mRootNode = mpScene->GetRootNode();
	std::cout << mRootNode->GetChildCount() << std::endl;
	if (mRootNode) 
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			PrintNode(mRootNode->GetChild(i));
	}
}

void CFbxLoader::PrintVertex()
{
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	mRootNode = mpScene->GetRootNode();
	std::cout << mRootNode->GetChildCount() << std::endl;
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			PrintVertexByNode(mRootNode->GetChild(i));
	}
}

void CFbxLoader::PrintVertexByNode(FbxNode* pNode)
{
	FbxMesh* pMesh = pNode->GetMesh();
	if (!pMesh->GetNode())
	{
		std::cout << "mesh err" << std::endl;
	}
	int iPolygonVertexCount = pMesh->GetPolygonCount();
	std::cout << "PolygonCount:" << iPolygonVertexCount << std::endl;
	FbxVector4 lcurrentVertex;
	FbxVector4* pcrtlPoint = pMesh->GetControlPoints();
	float * lVertices = new float[iPolygonVertexCount * 4];
	std::cout << "ControllPoint : " << std::endl;
	for (int lIndex = 0; lIndex < iPolygonVertexCount; ++lIndex)
	{
		// Save the vertex position.
		lcurrentVertex = pcrtlPoint[lIndex];
		lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(lcurrentVertex[0]);
		lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(lcurrentVertex[1]);
		lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(lcurrentVertex[2]);
		std::cout <<"(" <<lVertices[lIndex * VERTEX_STRIDE] << "," << lVertices[lIndex * VERTEX_STRIDE + 1] << "," <<
			lVertices[lIndex * VERTEX_STRIDE + 2] <<  ")" << std::endl;
	}
	

	delete[] lVertices;
}

void CFbxLoader::LoadElement(const FbxMesh* pMesh, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib)
{
	// pMesh가 NULL일 경우도 있기 때문에 NULL일 경우 return.
	if (!pMesh)
		return;
	if (!pMesh->GetNode())
		std::cout << "메쉬 오류" << std::endl;

	const int lPolygonCount = pMesh->GetPolygonCount();

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pMesh->GetElementMaterial())
	{
		lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			if (lMaterialIndice->GetCount() == lPolygonCount)
			{
				// Count the faces of each material
				// Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
				// if, in the loop above, we resized the mSubMeshes by more than one slot.

				// Record the offset (how many vertex)
			}
		}
	}

	// All faces will use the same material.
	

	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
	mHasNormal = pMesh->GetElementNormalCount() > 0;
	mHasUV = pMesh->GetElementUVCount() > 0;
	FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
	if (mHasNormal)
	{
		lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			mHasNormal = false;
		}
		if (mHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			mAllByControlPoint = false;
		}
	}
	if (mHasUV)
	{
		lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			mHasUV = false;
		}
		if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		{
			mAllByControlPoint = false;
		}
	}

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = pMesh->GetControlPointsCount();
	if (!mAllByControlPoint)
	{
		lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
	}
	vb.reserve(lPolygonVertexCount);
	//float * lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
	ib.reserve(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	//unsigned int * lIndices = new unsigned int[lPolygonCount * TRIANGLE_VERTEX_COUNT];
	float * lNormals = NULL;
	if (mHasNormal)
	{
		//lNormals = new float[lPolygonVertexCount * NORMAL_STRIDE];
	}
	float * lUVs = NULL;
	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	if (mHasUV && lUVNames.GetCount())
	{
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
		lUVName = lUVNames[0];
	}
	XMFLOAT3 tempVertex;
	XMFLOAT3 tempNormal;

	XMFLOAT2 tempfloat2;
	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pMesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;


	int lVertexCount = 0;
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		}

		// Where should I save the vertex attribute index, according to the material
	
		for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
		{
			const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

			if (mAllByControlPoint)
			{
				ib.push_back(static_cast<UINT>(lControlPointIndex));
				//lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				ib.push_back(static_cast<UINT>(lVertexCount));

				//lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = lControlPoints[lControlPointIndex];
				tempVertex.x = static_cast<float>(lCurrentVertex[0] * -1.0f);
				tempVertex.y = static_cast<float>(lCurrentVertex[1] );
				tempVertex.z = static_cast<float>(lCurrentVertex[2]);

			
				if (mHasNormal)
				{
					pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
					tempNormal.x = static_cast<float>(lCurrentNormal[0]);
					tempNormal.y = static_cast<float>(lCurrentNormal[1]);
					tempNormal.z = static_cast<float>(lCurrentNormal[2] );
				}
				if (mHasUV)
				{
					bool lUnmappedUV;
					pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
					/*
					2017 / 1 / 13 / 7:41
					작성자:박요한(dygks910910@daum.net)
					설명:왼손좌표계를 오른손좌표계로 변경하기위해서
					uv의 y를 뒤집어 줘야함.
					*/
					tempfloat2.x = static_cast<float>(lCurrentUV[0]);
					tempfloat2.y = 1.0f-static_cast<float>(lCurrentUV[1]);
					
					

				}
			}
			++lVertexCount;
			Vertex::Basic32 temp4Insert;
			temp4Insert.Pos = tempVertex;
			temp4Insert.Normal = tempNormal;
			temp4Insert.Tex = tempfloat2;
			vb.push_back(temp4Insert);
		}
	}
}

void CFbxLoader::PrintNode(FbxNode * pNode)
{
	//PrintTabs();

	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	// Print the contents of the node.
	printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
	);
	//numTabs++;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	/*numTabs--;
	PrintTabs();*/
	printf("</node>\n");
}

void CFbxLoader::PrintAttribute(FbxNodeAttribute * pAttribute)
{
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	//PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

FbxString CFbxLoader::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

void CFbxLoader::PrintElement()
{
	/*mRootNode = mpScene->GetRootNode();
	std::cout << mRootNode->GetChildCount() << std::endl;
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			LoadElement(mRootNode->GetChild(i)->GetMesh());*/
	//}
}

void CFbxLoader::Destroy()
{
	/*
	2017 / 1 / 7 / 1:42
	작성자:박요한(dygks910910@daum.net)
	설명:생성된 반대순으로 Destroy.하지않으면 릭발생가능성 있음.
	*/
	if (mpImporter)
		mpImporter->Destroy();
	if (mpScene)
		mpScene->Destroy();
	if (mpManager)
		mpManager->Destroy();
	
}

void CFbxLoader::LoadFBX(const char * pFileName,std::vector<Vertex::Basic32>& vb,std::vector<UINT>& ib)
{
	Init(pFileName);
	
	mRootNode = mpScene->GetRootNode();
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			LoadElement(mRootNode->GetChild(i)->GetMesh(),vb,ib);
		std::cout << vb.size() << std::endl;

	}
}


std::ostream & operator<<(std::ostream & os, const FbxVector4 & v4)
{
	os << "(" << static_cast<float>(v4[0]) << "," <<
		static_cast<float>(v4[1]) << "," <<
		static_cast<float>(v4[2]) << ")" << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const FbxVector2& v2)
{
	os << "(" << static_cast<float>(v2[0]) << "," <<
		static_cast<float>(v2[1]) << ")" << std::endl;
	return os;
}


#include "FbxLoader.h"

CFbxLoader::CFbxLoader() :
	mpManager(nullptr), mpScene(nullptr), mpImporter(nullptr), mRootNode(0),
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
#ifdef _DEBUG


	std::cout << strWinMsg << std::endl;
	std::cout << "파일버전:" << mlFileMajor << "." << mlFileMinor << "." << mlFileRevision << std::endl;
#endif // _DEBUG

	mRootNode = mpScene->GetRootNode();
	mpImporter->Import(mpScene);



}


void CFbxLoader::LoadElement(const FbxMesh * pMesh, GeometryGenerator::MeshData & meshData, const float & scaleFactor)
{
	// pMesh가 NULL일 경우도 있기 때문에 NULL일 경우 return.
	if (!pMesh)
		return;
	if (!pMesh->GetNode()) {
		MessageBox(0, TEXT("메쉬오류"), TEXT("오류"), 0);
		std::cout << "메쉬 오류" << std::endl;
	}
	const int lPolygonCount = pMesh->GetPolygonCount();



	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

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
	meshData.Vertices.reserve(lPolygonVertexCount);
	//meshData.Indices.reserve(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	float * lNormals = NULL;
	float * lUVs = NULL;
	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);

	const char * lUVName = NULL;
	if (mHasUV && lUVNames.GetCount())
	{
		lUVName = lUVNames[0];
#ifdef _DEBUG
		std::cout << "텍스처들:";
		std::cout << lUVName << std::endl;
#endif // _DEBUG
	}



	XMFLOAT3 tempVertex;
	XMFLOAT3 tempNormal;

	XMFLOAT2 tempfloat2;
	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pMesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	////////////최대 xyz 구하기////////////////
	XMVECTOR resultmax, resultmin;

	XMFLOAT3 vMinfx3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxfx3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	XMFLOAT3 vMinfy3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxfy3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	XMFLOAT3 vMinfz3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxfz3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMinx = XMLoadFloat3(&vMinfx3);
	XMVECTOR vMaxx = XMLoadFloat3(&vMaxfx3);
	XMVECTOR vMiny = XMLoadFloat3(&vMinfy3);
	XMVECTOR vMaxy = XMLoadFloat3(&vMaxfy3);
	XMVECTOR vMinz = XMLoadFloat3(&vMinfz3);
	XMVECTOR vMaxz = XMLoadFloat3(&vMaxfz3);
	XMFLOAT3 x, y, z;
	x.y = 0;
	x.z = 0;
	y.x = 0;
	y.z = 0;
	z.x = 0;
	z.y = 0;
	///////////////////////////////////////////
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

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
				meshData.Indices.push_back(static_cast<UINT>(lControlPointIndex));
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				meshData.Indices.push_back(static_cast<UINT>(lVertexCount));

				lCurrentVertex = lControlPoints[lControlPointIndex];
				tempVertex.x = static_cast<float>(lCurrentVertex[0] );
				tempVertex.y = static_cast<float>(lCurrentVertex[1]);
				tempVertex.z = static_cast<float>(lCurrentVertex[2]);

				x.x = tempVertex.x;
				y.y = tempVertex.y;
				z.z = tempVertex.z;

				XMVECTOR p = XMLoadFloat3(&tempVertex);
				vMin = XMVectorMin(vMin, p);
				vMax = XMVectorMax(vMax, p);

				//////////////////////////최대 xyz//////////////////
				XMVECTOR vx = XMLoadFloat3(&x);
				XMVECTOR vy = XMLoadFloat3(&y);
				XMVECTOR vz = XMLoadFloat3(&z);

				////x 최대 최소////
				vMinx = XMVectorMin(vMinx, vx);
				vMaxx = XMVectorMax(vMaxx, vx);
				////y 최대 최소////
				vMiny = XMVectorMin(vMiny, vy);
				vMaxy = XMVectorMax(vMaxy, vy);
				////z 최대 최소////
				vMinz = XMVectorMin(vMinz, vz);
				vMaxz = XMVectorMax(vMaxz, vz);
				////최종 최대 최소////
				resultmin = vMinx + vMiny + vMinz;
				resultmax = vMaxx + vMaxy + vMaxz;
				/////////////////////////////////////////////////////
				if (mHasNormal)
				{
					pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
					tempNormal.x = static_cast<float>(lCurrentNormal[0]);
					tempNormal.y = static_cast<float>(lCurrentNormal[1]);
					tempNormal.z = static_cast<float>(lCurrentNormal[2] * -1.0f);
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
					tempfloat2.y = 1.0f - static_cast<float>(lCurrentUV[1]);



				}
			}

			GeometryGenerator::Vertex temp4Insert;
			temp4Insert.Position = XMFLOAT3( tempVertex.x*scaleFactor, tempVertex.y * scaleFactor, tempVertex.z * scaleFactor);
			temp4Insert.Normal = tempNormal;
			temp4Insert.TexC = tempfloat2;
			//탄젠트 계산.
			XMVECTOR tangent;
			XMVECTOR c1 = XMVector3Cross(XMLoadFloat3(&tempNormal), XMVectorSet(0, 0, 1, 0));
			XMVECTOR c2 = XMVector3Cross(XMLoadFloat3(&tempNormal), XMVectorSet(0, 1, 0, 0));

			if (Length(c1) > Length(c2))
			{
				tangent = c1;
			}
			else
			{
				tangent = c2;
			}
			tangent = XMVector3Normalize(tangent);
			XMStoreFloat3(&temp4Insert.TangentU, tangent);
			meshData.Vertices.push_back(temp4Insert);
			++lVertexCount;
		}
	}

}

float CFbxLoader::Length(const XMVECTOR& c1)
{
	return sqrt(pow(XMVectorGetX(c1), 2) + pow(XMVectorGetY(c1), 2) + pow(XMVectorGetZ(c1), 2));
}

void CFbxLoader::ProcessMaterials(FbxNode* inNode, Material& material)
{
	unsigned int materialCount = inNode->GetMaterialCount();

	for (unsigned int i = 0; i < materialCount; ++i)
	{
		FbxSurfaceMaterial* surfaceMaterial = inNode->GetMaterial(i);
		ProcessMaterialAttribute(surfaceMaterial, i, material);
	}
}
void CFbxLoader::ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex, Material& material)
{
	FbxDouble3 double3;
	if (inMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		Material currMaterial;
		ZeroMemory(&currMaterial, sizeof(currMaterial));
		// Amibent Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Ambient;
		currMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		currMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		currMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Diffuse;
		currMaterial.Diffuse.x = static_cast<float>(double3.mData[0]);
		currMaterial.Diffuse.y = static_cast<float>(double3.mData[1]);
		currMaterial.Diffuse.z = static_cast<float>(double3.mData[2]);

		// Specular Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Specular;
		currMaterial.Specular.x = static_cast<float>(double3.mData[0]);
		currMaterial.Specular.y = static_cast<float>(double3.mData[1]);
		currMaterial.Specular.z = static_cast<float>(double3.mData[2]);

		// Reflection
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Reflection;
		currMaterial.Reflect.x = static_cast<float>(double3.mData[0]);
		currMaterial.Reflect.y = static_cast<float>(double3.mData[1]);
		currMaterial.Reflect.z = static_cast<float>(double3.mData[2]);

		material = currMaterial;

	}
	else if (inMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		Material currMaterial;
		ZeroMemory(&currMaterial, sizeof(currMaterial));

		// Amibent Color
		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Ambient;
		currMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		currMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		currMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Diffuse;
		currMaterial.Diffuse.x = static_cast<float>(double3.mData[0]);
		currMaterial.Diffuse.y = static_cast<float>(double3.mData[1]);
		currMaterial.Diffuse.z = static_cast<float>(double3.mData[2]);

	
		material = currMaterial;
	}
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

void CFbxLoader::LoadFBX(const char * pFileName, GeometryGenerator::MeshData & meshData, const float & scaleFactor)
{
	Init(pFileName);
	//fout << "Name: " << pFileName << std::endl;
	mRootNode = mpScene->GetRootNode();
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			LoadElement(mRootNode->GetChild(i)->GetMesh(), meshData,scaleFactor);
#ifdef _DEBUG
		std::cout << meshData.Vertices.size() << std::endl;
#endif // _DEBUG

	}
	Destroy();
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

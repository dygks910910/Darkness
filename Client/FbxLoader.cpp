
#include "FbxLoader.h"
extern XMFLOAT3 bextent;
extern XMFLOAT3 bcenter;
extern bool check;
std::ofstream fout("aabb.txt");

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
	std::cout << strWinMsg << std::endl;
	std::cout << "파일버전:" << mlFileMajor << "." << mlFileMinor << "." << mlFileRevision << std::endl;
	mRootNode = mpScene->GetRootNode();
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
		std::cout << "(" << lVertices[lIndex * VERTEX_STRIDE] << "," << lVertices[lIndex * VERTEX_STRIDE + 1] << "," <<
			lVertices[lIndex * VERTEX_STRIDE + 2] << ")" << std::endl;
	}


	delete[] lVertices;
}

void CFbxLoader::LoadElement(const FbxMesh* pMesh,GeometryGenerator::MeshData& meshData)
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
	// 	if (pMesh->GetElementMaterial())
	// 	{
	// 		lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
	// 		lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
	// 		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
	// 		{
	// 			FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
	// 			if (lMaterialIndice->GetCount() == lPolygonCount)
	// 			{
	// 				// Count the faces of each material
	// 				for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	// 				{
	// 					const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
	// 					if (mSubMeshes.Size() < lMaterialIndex + 1)
	// 					{
	// 						mSubMeshes.Resize(lMaterialIndex + 1);
	// 					}
	// 					if (mSubMeshes[lMaterialIndex] == NULL)
	// 					{
	// 						mSubMeshes[lMaterialIndex] = new SubMesh;
	// 					}
	// 					mSubMeshes[lMaterialIndex]->TriangleCount += 1;
	// 				}
	// 
	// 				// Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
	// 				// if, in the loop above, we resized the mSubMeshes by more than one slot.
	// 				for (int i = 0; i < mSubMeshes.Size(); i++)
	// 				{
	// 					if (mSubMeshes[i] == NULL)
	// 						mSubMeshes[i] = new SubMesh;
	// 				}
	// 
	// 				// Record the offset (how many vertex)
	// 				const int lMaterialCount = mSubMeshes.Size();
	// 				int lOffset = 0;
	// 				for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
	// 				{
	// 					mSubMeshes[lIndex]->IndexOffset = lOffset;
	// 					lOffset += mSubMeshes[lIndex]->TriangleCount * 3;
	// 					// This will be used as counter in the following procedures, reset to zero
	// 					mSubMeshes[lIndex]->TriangleCount = 0;
	// 				}
	// 				FBX_ASSERT(lOffset == lPolygonCount * 3);
	// 			}
	// 		}
	// 	}

		// All faces will use the same material.
	// 	if (mSubMeshes.Size() == 0)
	// 	{
	// 		mSubMeshes.Resize(1);
	// 		mSubMeshes[0] = new SubMesh();
	// 	}


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
	//float * lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
	meshData.Indices.reserve(lPolygonCount * TRIANGLE_VERTEX_COUNT);
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
		std::cout << "텍스처들:";
		std::cout << lUVName << std::endl;
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
				//lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				meshData.Indices.push_back(static_cast<UINT>(lVertexCount));

				//lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = lControlPoints[lControlPointIndex];
				tempVertex.x = static_cast<float>(lCurrentVertex[0] *-1.0f );
				tempVertex.y = static_cast<float>(lCurrentVertex[1] );
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
					tempNormal.z = static_cast<float>(lCurrentNormal[2]);
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
			temp4Insert.Position = tempVertex;
			temp4Insert.Normal = tempNormal;
			temp4Insert.TexC= tempfloat2;
			//탄젠트 계산.
			XMVECTOR tangent;
			XMVECTOR c1 = XMVector3Cross(XMLoadFloat3(&tempNormal), XMVectorSet(0, 0, 1,0));
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
			XMStoreFloat3(&temp4Insert.TangentU,tangent);
			meshData.Vertices.push_back(temp4Insert);
			++lVertexCount;
		}
		//mSubMeshes[lMaterialIndex]->TriangleCount += 1;
	}
	XMFLOAT3 testextent;
	XMFLOAT3 testcenter;
	XMStoreFloat3(&bcenter, 0.5*(resultmax + resultmin));
	XMStoreFloat3(&bextent, 0.5*(resultmax - resultmin));


		/*XMStoreFloat3(&bcenter, 0.5*(vMax + vMin));
		XMStoreFloat3(&bextent, 0.5*(vMax - vMin));*/
	fout << "extent " << bextent.x << ' ' << bextent.y << ' ' << bextent.z << std::endl;
	fout << "center " << bcenter.x << ' ' << bcenter.y << ' ' << bcenter.z << std::endl;
	fout << std::endl;

	std::cout << "extent" << bextent.x << " " << bextent.y << " " << bextent.z << std::endl;
	std::cout << "center" << bcenter.x << " " << bcenter.y << " " << bcenter.z << std::endl;
	GetUVName();
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
	FbxDouble double1;
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

		// Emissive Color
// 		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Emissive;
// 		currMaterial.mEmissive.x = static_cast<float>(double3.mData[0]);
// 		currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
// 		currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

		// Reflection
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Reflection;
		currMaterial.Reflect.x = static_cast<float>(double3.mData[0]);
		currMaterial.Reflect.y = static_cast<float>(double3.mData[1]);
		currMaterial.Reflect.z = static_cast<float>(double3.mData[2]);

		// Transparency Factor
// 		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->TransparencyFactor;
// 		currMaterial->mTransparencyFactor = double1;

		// Shininess
// 		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Shininess;
// 		currMaterial->mShininess = double1;

		// Specular Factor
// 		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->SpecularFactor;
// 		currMaterial->mSpecularPower = double1;


		// Reflection Factor
// 		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->ReflectionFactor;
// 		currMaterial->mReflectionFactor = double1;
// 
// 		mMaterialLookUp[inMaterialIndex] = currMaterial;
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

		// Emissive Color
// 		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Emissive;
// 		currMaterial->mEmissive.x = static_cast<float>(double3.mData[0]);
// 		currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
// 		currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

		// Transparency Factor
// 		double1 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->TransparencyFactor;
// 		currMaterial->mTransparencyFactor = double1;

	/*	mMaterialLookUp[inMaterialIndex] = currMaterial;*/
		material = currMaterial;
	}
}

void CFbxLoader::GetUVName()
{
	int mcount = mRootNode->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int index = 0; index < mcount; index++)
	{
		FbxSurfaceMaterial *material = (FbxSurfaceMaterial*)mRootNode->GetSrcObject<FbxSurfaceMaterial>(index);
		if (material)
		{
			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			// Check if it's layeredtextures
			int layered_texture_count = prop.GetSrcObjectCount<FbxLayeredTexture>();
			if (layered_texture_count > 0)
			{
				for (int j = 0; j < layered_texture_count; j++)
				{
					FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
					int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();
					for (int k = 0; k < lcount; k++)
					{
						FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
						// Then, you can get all the properties of the texture, include its name
						const char* texture_name = texture->GetName();
						std::cout << texture_name << std::endl;

					}
				}
			}
			else
			{
				// Directly get textures
				int texture_count = prop.GetSrcObjectCount<FbxTexture>();
				for (int j = 0; j < texture_count; j++)
				{
					const FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
					// Then, you can get all the properties of the texture, include its name
					const char* texture_name = texture->GetName();
					std::cout << texture_name << std::endl;

				}
			}
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

	mMaterialLookUp.clear();

	// 	if (mRootNode)
	// 	{
	// 		mRootNode->Destroy();
	// 	}
		//mSubMeshes.Clear();

}

void CFbxLoader::LoadFBX(const char* pFileName, GeometryGenerator::MeshData& mesh)
{
	Init(pFileName);
	fout << "Name: " << pFileName << std::endl;
	mRootNode = mpScene->GetRootNode();
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			LoadElement(mRootNode->GetChild(i)->GetMesh(), mesh);
		std::cout << mesh.Vertices.size() << std::endl;

	}
	Destroy();
	//ProcessMaterials(mRootNode, material);
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

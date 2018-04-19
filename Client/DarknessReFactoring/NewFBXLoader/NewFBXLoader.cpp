#include "NewFBXLoader.h"
//setw���
#include <iomanip>



NewFBXLoader::NewFBXLoader()
{
}


NewFBXLoader::~NewFBXLoader()
{
}

void NewFBXLoader::LoadFBX(const char * fileName, GeometryGenerator::MeshData & outMesh, const float& scaleFactor)
{
	Init(fileName);
	ProcessAll(outMesh, scaleFactor);
	Destroy();
}

void NewFBXLoader::Init(const char * fileName)
{
	mManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(mManager, IOSROOT);
	mManager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(mManager, "");
	bool status = importer->Initialize(fileName, -1, mManager->GetIOSettings());
	if (!status)
	{
		throw std::exception((std::string("import failed") + fileName).c_str());
	}


	mFbxScene = FbxScene::Create(mManager, "myScene");
	importer->Import(mFbxScene);

	//��ǥ�� ����.
	FbxAxisSystem sceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::DirectX.ConvertScene(mFbxScene);

	//��� ���� �ﰢ��ȭ��Ų��.
	FbxGeometryConverter converter(mManager);
	converter.Triangulate(mFbxScene, true);

	importer->Destroy();
}

void NewFBXLoader::ProcessAll(GeometryGenerator::MeshData& outMesh, const float& scaleFactor)
{
	FbxNode* mRootNode = mFbxScene->GetRootNode();
	const int nodeCounter = mRootNode->GetChildCount();
	if (mRootNode)
	{
		for (int i = 0; i < nodeCounter; i++)
		{
			ProcessNode(mRootNode->GetChild(i), outMesh, scaleFactor);
		}
	}
}

void NewFBXLoader::Destroy()
{
	mManager->Destroy();

}

void NewFBXLoader::ProcessNode(FbxNode * pNode, GeometryGenerator::MeshData& outMesh, const float& scaleFactor)
{
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT2 tex;
	XMFLOAT3 pos;

	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();
	unsigned int vertexCount = 0;
	//��� �Ӽ��� �����Ҷ���
	if (nodeAttribute)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//mesh�Ӽ��� ���� ��带 Ž���Ѵ�.
			FbxMesh* pMesh = pNode->GetMesh();

			for (unsigned int triCount = 0; triCount < pMesh->GetPolygonCount(); ++triCount)
			{
				//�ﰢ���� �������� �̷���.
				for (unsigned int polygonIndex = 0; polygonIndex < 3; ++polygonIndex)
				{
					int controlPointIndex = pMesh->GetPolygonVertex(triCount, polygonIndex);

					pos = ProcessPosistion(pMesh, controlPointIndex);
					normal = ProcessNormal(pMesh, controlPointIndex, vertexCount);
					tangent = ProcessTangent(pMesh, controlPointIndex, vertexCount);
					tex = ProcessUV(pMesh, vertexCount, pMesh->GetTextureUVIndex(triCount, polygonIndex));
					pMesh->GetControlPointAt(controlPointIndex).mData[0];
					InsertData(pos, tangent, normal, tex, outMesh, scaleFactor);

					vertexCount++;

				}
			}
		}
	}



	//��� ���Ž��
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		ProcessNode(pNode->GetChild(j), outMesh, scaleFactor);
	}
}
XMFLOAT3 NewFBXLoader::ProcessPosistion(FbxMesh * pMesh, const unsigned int & controlPointIndex)
{
	return  XMFLOAT3
	{
		static_cast<float>(pMesh->GetControlPointAt(controlPointIndex).mData[0]),
		static_cast<float>(pMesh->GetControlPointAt(controlPointIndex).mData[1]),
		static_cast<float>(pMesh->GetControlPointAt(controlPointIndex).mData[2])
	};
}
XMFLOAT3 NewFBXLoader::ProcessNormal(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	XMFLOAT3 float3;
	if (pMesh->GetElementNormalCount() < 1)
	{
		std::cout << "normal�̾���\n";
	}
	const ::FbxGeometryElementNormal* normal = pMesh->GetElementNormal();


	switch (normal->GetMappingMode())
	{
		//����Ʈ������Ʈ���� ������ǥ�� ����.
	case FbxGeometryElement::eByControlPoint:
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ controlPointIndex�� ���ϴ� �ε����� ����� �����´�.
			int index = normal->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex �� �׳� �����´�.
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//���������� ������ǥ�� ����.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ vertexCount�α��ϴ� �ε����� ����� �����´�.
			int index = normal->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount�� ���´�.
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(vertexCount).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(vertexCount).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(vertexCount).mData[2]);
			break;
		}
		}
	}
	}
	return float3;
}
XMFLOAT3 NewFBXLoader::ProcessBinormal(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	XMFLOAT3 float3;
	if (pMesh->GetElementBinormalCount() < 1)
	{
		std::cout << "Binormal�̾���\n";
		return  XMFLOAT3();
	}
	const ::FbxGeometryElementBinormal* binormal = pMesh->GetElementBinormal();


	switch (binormal->GetMappingMode())
	{
		//����Ʈ������Ʈ���� ������ǥ�� ����.
	case FbxGeometryElement::eByControlPoint:
		switch (binormal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ controlPointIndex�� ���ϴ� �ε����� ����� �����´�.
			int index = binormal->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex �� �׳� �����´�.
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//���������� ������ǥ�� ����.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (binormal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ vertexCount�α��ϴ� �ε����� ����� �����´�.
			int index = binormal->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount�� ���´�.
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(vertexCount).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(vertexCount).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(vertexCount).mData[2]);
			break;
		}
		}
	}
	}
	return float3;
}
XMFLOAT3 NewFBXLoader::ProcessTangent(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	XMFLOAT3 float3;
	if (pMesh->GetElementTangentCount() < 1)
	{
		FbxStringList uvSetNames;
		pMesh->GetUVSetNames(uvSetNames);
		std::cout << uvSetNames[0] << "\n";
		pMesh->GenerateTangentsData(uvSetNames[0]);

	}
	const ::FbxGeometryElementTangent* tangent = pMesh->GetElementTangent();


	switch (tangent->GetMappingMode())
	{
		//����Ʈ������Ʈ���� ������ǥ�� ����.
	case FbxGeometryElement::eByControlPoint:
		switch (tangent->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ controlPointIndex�� ���ϴ� �ε����� ����� �����´�.
			int index = tangent->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex �� �׳� �����´�.
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//���������� ������ǥ�� ����.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (tangent->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ vertexCount�α��ϴ� �ε����� ����� �����´�.
			int index = tangent->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount�� ���´�.
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexCount).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexCount).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexCount).mData[2]);
			break;
		}
		}
	}
	}
	return float3;
}
XMFLOAT2 NewFBXLoader::ProcessUV(FbxMesh* pMesh, const unsigned int& vertexCount, const unsigned int& textureUVIndex)
{
	XMFLOAT2 float2;
	if (pMesh->GetElementUVCount() < 1)
	{
		std::cout << "UVCount����\n";
	}
	const FbxGeometryElementUV* uv = pMesh->GetElementUV();


	switch (uv->GetMappingMode())
	{
		//����Ʈ������Ʈ���� ������ǥ�� ����.
	case FbxGeometryElement::eByControlPoint:
	{
		switch (uv->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ controlPointIndex�� ���ϴ� �ε����� ����� �����´�.
			int index = uv->GetIndexArray().GetAt(textureUVIndex);
			float2.x = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[0]);
			float2.y = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[1]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex �� �׳� �����´�.
			float2.x = static_cast<float>(uv->GetDirectArray().GetAt(textureUVIndex).mData[0]);
			float2.y = static_cast<float>(uv->GetDirectArray().GetAt(textureUVIndex).mData[1]);
			break;
		}
		}
		break;
	}
	//���������� ������ǥ�� ����.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (uv->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//���⼭ vertexCount�α��ϴ� �ε����� ����� �����´�.
			int index = uv->GetIndexArray().GetAt(vertexCount);
			float2.x = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[0]);
			float2.y = static_cast<float>(uv->GetDirectArray().GetAt(index).mData[1]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount�� ���´�.
			float2.x = static_cast<float>(uv->GetDirectArray().GetAt(vertexCount).mData[0]);
			float2.y = static_cast<float>(uv->GetDirectArray().GetAt(vertexCount).mData[1]);
			break;
		}
		}
		break;
	}
	}
	return float2;
}
FbxString NewFBXLoader::GetAttributeTypeName(FbxNodeAttribute::EType type)
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

void NewFBXLoader::PrintAttribute(FbxNodeAttribute * pAttribute)
{
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

bool NewFBXLoader::InsertData(const  XMFLOAT3& pos, const  XMFLOAT3& tangent,
	const  XMFLOAT3& normal, const  XMFLOAT2& tex, GeometryGenerator::MeshData& outMesh,
	const float& scaleFactor)
{
	XMFLOAT3 posScaled = {
		pos.x * scaleFactor,
		pos.y * scaleFactor ,
		pos.z* scaleFactor
	};

	GeometryGenerator::Vertex vtx(posScaled, normal, tangent, tex);

	auto finded = forMapping.find(vtx);
	if (finded == forMapping.end())
	{
		// 		//ã�� �����Ͱ� ������?
		int vtxArrLength = outMesh.Vertices.size();
		outMesh.Vertices.push_back(vtx);
		outMesh.Indices.push_back(vtxArrLength);
		forMapping.insert(std::make_pair(vtx, vtxArrLength));
		std::cout << vtx.TexC.x << "     " << vtx.TexC.y << "\n";

	}
	else
	{
		//ã�µ����Ͱ� ������?
		outMesh.Indices.push_back(finded->second);
	}
	return true;
}

bool operator<(XMFLOAT3 a, XMFLOAT3 b)
{
	return false;
}
// 
// std::ostream & operator<<(std::ostream & os, const Vertex & vtx)
// {
// 	os << "pos:" << vtx.pos.x << "     " << vtx.pos.y << "     " << vtx.pos.z << "\n";
// 	os << "normal:" << vtx.normal.x << "     " << vtx.normal.y << "     " << vtx.normal.z << "\n";
// 	os << "tex:" << vtx.tex.x << "     " << vtx.tex.y << "\n";
// 	os << "tangent:" << vtx.tangent.x << "     " << vtx.tangent.y << "     " << vtx.tangent.z << vtx.tangent.w << "\n\n";
// 	return os;
// 	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
// }
// 
// std::ostream & operator<<(std::ostream & os, const  XMFLOAT3 & vtx)
// {
// 	os << "pos:" << vtx.x << "     " << vtx.y << "     " << vtx.z << "\n";
// 	return os;
// }

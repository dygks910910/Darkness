#include "NewFBXLoader.h"
//setw사용
#include <iomanip>



NewFBXLoader::NewFBXLoader()
{
}


NewFBXLoader::~NewFBXLoader()
{
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

	//좌표축 변경.
	FbxAxisSystem sceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::DirectX.ConvertScene(mFbxScene);

	//모든 점을 삼각형화시킨다.
	FbxGeometryConverter converter(mManager);
	converter.Triangulate(mFbxScene, true);

	importer->Destroy();
}

void NewFBXLoader::ProcessAll()
{
	FbxNode* mRootNode = mFbxScene->GetRootNode();
	const int nodeCounter = mRootNode->GetChildCount();
	if (mRootNode)
	{
		for (int i = 0; i < nodeCounter; i++)
		{
			ProcessNode(mRootNode->GetChild(i));
		}
	}
}

void NewFBXLoader::Destroy()
{
	mManager->Destroy();

}

void NewFBXLoader::ProcessControlPoint(FbxMesh * pMesh)
{
	DirectX::XMFLOAT3 vertex;
	vtxArr.reserve(pMesh->GetControlPointsCount());
	for (int i = 0; i < pMesh->GetControlPointsCount(); ++i)
	{
		vertex = 
		{
			static_cast<float>(pMesh->GetControlPointAt(i).mData[0]),
			static_cast<float>(pMesh->GetControlPointAt(i).mData[1]),
			static_cast<float>(pMesh->GetControlPointAt(i).mData[2])
		};
		//std::cout << std::setw(5) << vertex.pos;
		vtxArr.push_back(vertex);
	}
	std::cout << "정점갯수:" << pMesh->GetControlPointsCount() << "\n";
}

void NewFBXLoader::ProcessNode(FbxNode * pNode)
{
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT2 tex;
	

	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();
	unsigned int vertexCount = 0;
	//노드 속성이 존재할때만
	if (nodeAttribute)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//mesh속성을 가진 노드를 탐색한다.
			FbxMesh* pMesh = pNode->GetMesh();
			ProcessControlPoint(pMesh);

			for (unsigned int polygonCount = 0; polygonCount < pMesh->GetPolygonCount(); ++polygonCount)
			{
				//삼각형은 점세개로 이뤄짐.
				for (unsigned int polygonIndex = 0; polygonIndex < 3; ++polygonIndex)
				{
					int controlPointIndex = pMesh->GetPolygonVertex(polygonCount, polygonIndex);
					normal = ProcessNormal(pMesh, controlPointIndex, vertexCount);
					tangent = ProcessTangent(pMesh, controlPointIndex, vertexCount);
					tex = ProcessUV(pMesh, controlPointIndex, pMesh->GetTextureUVIndex(polygonIndex, polygonIndex));

					DirectX::XMFLOAT3& currControllPoint = vtxArr[controlPointIndex];
					InsertData(currControllPoint, tangent, normal, tex);

					vertexCount++;

				}
			}
		}
	}



	//노드 재귀탐색
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		ProcessNode(pNode->GetChild(j));
	}
}
DirectX::XMFLOAT3 NewFBXLoader::ProcessNormal(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	DirectX::XMFLOAT3 float3;
	if (pMesh->GetElementNormalCount() < 1)
	{
		std::cout << "normal이없음\n";
	}
	const ::FbxGeometryElementNormal* normal = pMesh->GetElementNormal();


	switch (normal->GetMappingMode())
	{
		//각컨트롤포인트마다 매핑좌표가 있음.
	case FbxGeometryElement::eByControlPoint:
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 controlPointIndex로 구하는 인덱스로 노멀을 가져온다.
			int index = normal->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex 로 그냥 가져온다.
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//꼭지점마다 매핑좌표가 있음.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 vertexCount로구하는 인덱스로 노멀을 가져온다.
			int index = normal->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount로 얻어온다.
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
DirectX::XMFLOAT3 NewFBXLoader::ProcessBinormal(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	DirectX::XMFLOAT3 float3;
	if (pMesh->GetElementBinormalCount() < 1)
	{
		std::cout << "Binormal이없음\n";
		return DirectX::XMFLOAT3();
	}
	const ::FbxGeometryElementBinormal* binormal = pMesh->GetElementBinormal();


	switch (binormal->GetMappingMode())
	{
		//각컨트롤포인트마다 매핑좌표가 있음.
	case FbxGeometryElement::eByControlPoint:
		switch (binormal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 controlPointIndex로 구하는 인덱스로 노멀을 가져온다.
			int index = binormal->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex 로 그냥 가져온다.
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//꼭지점마다 매핑좌표가 있음.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (binormal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 vertexCount로구하는 인덱스로 노멀을 가져온다.
			int index = binormal->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(binormal->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount로 얻어온다.
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
DirectX::XMFLOAT3 NewFBXLoader::ProcessTangent(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & vertexCount)
{
	DirectX::XMFLOAT3 float3;
	if (pMesh->GetElementTangentCount() < 1)
	{
		//std::cout << "Tangent이없음\n";
		FbxStringList uvSetNames;
		pMesh->GetUVSetNames(uvSetNames);
		pMesh->GenerateTangentsData(uvSetNames[0]);

	}
	const ::FbxGeometryElementTangent* tangent = pMesh->GetElementTangent();


	switch (tangent->GetMappingMode())
	{
		//각컨트롤포인트마다 매핑좌표가 있음.
	case FbxGeometryElement::eByControlPoint:
		switch (tangent->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 controlPointIndex로 구하는 인덱스로 노멀을 가져온다.
			int index = tangent->GetIndexArray().GetAt(controlPointIndex);
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[2]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex 로 그냥 가져온다.
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(controlPointIndex).mData[2]);
			break;
		}
		}
		//꼭지점마다 매핑좌표가 있음.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (tangent->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 vertexCount로구하는 인덱스로 노멀을 가져온다.
			int index = tangent->GetIndexArray().GetAt(vertexCount);
			float3.x = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[0]);
			float3.y = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[1]);
			float3.z = static_cast<float>(tangent->GetDirectArray().GetAt(index).mData[2]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount로 얻어온다.
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
DirectX::XMFLOAT2 NewFBXLoader::ProcessUV(FbxMesh * pMesh, const unsigned int & controlPointIndex, const unsigned int & textureUVIndex)
{
	DirectX::XMFLOAT2 float2;
	if (pMesh->GetElementUVCount() < 1)
	{
		std::cout << "UVCount없음\n";
	}
	const ::FbxGeometryElementUV* normal = pMesh->GetElementUV();


	switch (normal->GetMappingMode())
	{
		//각컨트롤포인트마다 매핑좌표가 있음.
	case FbxGeometryElement::eByControlPoint:
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 controlPointIndex로 구하는 인덱스로 노멀을 가져온다.
			int index = normal->GetIndexArray().GetAt(controlPointIndex);
			float2.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float2.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			break;
		}

		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//controlPointIndex 로 그냥 가져온다.
			float2.x = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			float2.y = static_cast<float>(normal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			break;
		}
		}
		//꼭지점마다 매핑좌표가 있음.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::EReferenceMode::eIndexToDirect:
		{
			//여기서 vertexCount로구하는 인덱스로 노멀을 가져온다.
			int index = normal->GetIndexArray().GetAt(textureUVIndex);
			float2.x = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[0]);
			float2.y = static_cast<float>(normal->GetDirectArray().GetAt(index).mData[1]);
			break;
		}
		case FbxGeometryElement::EReferenceMode::eDirect:
		{
			//vertexCount로 얻어온다.
			float2.x = static_cast<float>(normal->GetDirectArray().GetAt(textureUVIndex).mData[0]);
			float2.y = static_cast<float>(normal->GetDirectArray().GetAt(textureUVIndex).mData[1]);
			break;
		}
		}
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

bool NewFBXLoader::InsertData(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& tangent,
	const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& tex)
{
	DirectX::XMFLOAT4 tmpTangent = { tangent.x,tangent.y ,tangent.z ,1 };
	Vertex vtx = { pos,normal,tex,tmpTangent };
 	auto finded = forMapping.find(vtx);
 	if (finded == forMapping.end())
 	{
// 		//찾는 데이터가 없으면?
		int vtxArrLength = m_Vtx.size();
		m_Vtx.push_back(vtx);
		m_Idx.push_back(vtxArrLength);
		forMapping.insert(std::make_pair(vtx, vtxArrLength));
 	}
 	else
 	{
 		//찾는데이터가 있으면?
		m_Idx.push_back(finded->second);
 	}
	return true;
}

bool operator<(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	return false;
}

std::ostream & operator<<(std::ostream & os, const Vertex & vtx)
{
	os << "pos:" << vtx.pos.x << "     " << vtx.pos.y << "     " << vtx.pos.z << "\n";
	os << "normal:" << vtx.normal.x << "     " << vtx.normal.y << "     " << vtx.normal.z << "\n";
	os << "tex:" << vtx.tex.x << "     " << vtx.tex.y << "\n";
	os << "tangent:" << vtx.tangent.x << "     " << vtx.tangent.y << "     " << vtx.tangent.z << vtx.tangent.w << "\n\n";
	return os;
	// TODO: 여기에 반환 구문을 삽입합니다.
}

std::ostream & operator<<(std::ostream & os, const DirectX::XMFLOAT3 & vtx)
{
	os << "pos:" << vtx.x << "     " << vtx.y << "     " << vtx.z << "\n";
	return os;
}

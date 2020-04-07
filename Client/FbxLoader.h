#pragma once
#include<fbxsdk.h>
#include<iostream>
#include<string>
#include<vector>
#include "Vertex.h"
#include <math.h>
#include<unordered_map>
#include "Common/LightHelper.h"
#include "Common/GeometryGenerator.h"
/*
2017 / 1 / 7 / 5:33
작성자:박요한(dygks910910@daum.net)
설명:UV,Normal,Vertex 등의 정보를 가져올수 있음
*/
class CFbxLoader
{
	// Three floats for every position.
	const int VERTEX_STRIDE = 4;
	//같음.
	const int TRIANGLE_VERTEX_COUNT = 3;
	// Three floats for every normal.
	const int NORMAL_STRIDE = 3;
	// Two floats for every UV.
	const int UV_STRIDE = 2;
public:
	CFbxLoader();
	~CFbxLoader();
	/*
	2017 / 1 / 7 / 2:05
	작성자:박요한(dygks910910@daum.net)
	설명:FbxManager,FbxScene,FBXImporter,RootNode Init.
	*/
	void Init(const char* pFileName);
	void Print();
	void PrintVertex();
	void PrintElement();
	void Destroy();
	void LoadFBX(const char* pFileName,GeometryGenerator::MeshData& meshData);
	template<class BOX>
	void LoadFBX(const char* pFileName, GeometryGenerator::MeshData& meshData,BOX& box);

private:
	FbxManager* mpManager;
	FbxScene* mpScene;
	FbxImporter* mpImporter;
	FbxNode* mRootNode;
	int mlFileMajor, mlFileMinor, mlFileRevision;
	std::unordered_map<unsigned int, Material*> mMaterialLookUp;
private:
	void GetUVName();
	void PrintNode(FbxNode* pNode);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	void PrintVertexByNode(FbxNode* pNode);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void LoadElement(const FbxMesh* pMesh,GeometryGenerator::MeshData& meshData);
	template<class BOX>
	void LoadElement(const FbxMesh* pMesh, GeometryGenerator::MeshData& meshData,BOX& box );

	float Length(const XMVECTOR& c1);
	void ProcessMaterials(FbxNode* inNode, Material& material);
	void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex, Material& material);


	bool mHasNormal;
	bool mHasUV;
	bool mAllByControlPoint; // Save data in VBO by control point or by polygon vertex.

	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};
};

std::ostream& operator<<(std::ostream& os, const FbxVector4& v4);
std::ostream& operator<<(std::ostream& os, const FbxVector2& v2);

template<class BOX>
inline void CFbxLoader::LoadFBX(const char * pFileName, GeometryGenerator::MeshData & meshData, BOX & box)
{
	Init(pFileName);
	mRootNode = mpScene->GetRootNode();
	if (mRootNode)
	{
		for (int i = 0; i < mRootNode->GetChildCount(); i++)
			LoadElement(mRootNode->GetChild(i)->GetMesh(), meshData,box);
		std::cout << meshData.Vertices.size() << std::endl;

	}
	Destroy();
}

template<class BOX>
inline void CFbxLoader::LoadElement(const FbxMesh * pMesh, GeometryGenerator::MeshData & meshData, BOX & box)
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
	meshData.Indices.reserve(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	float * lNormals = NULL;
	
	float * lUVs = NULL;
	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);

	const char * lUVName = NULL;
	if (mHasUV && lUVNames.GetCount())
	{
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
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				meshData.Indices.push_back(static_cast<UINT>(lVertexCount));

				lCurrentVertex = lControlPoints[lControlPointIndex];
				tempVertex.x = static_cast<float>(lCurrentVertex[0] * -1.0f);
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
		XMStoreFloat3(&bcenter, 0.5*(resultmax + resultmin));
		XMStoreFloat3(&bextent, 0.5*(resultmax - resultmin));
		box.Center = bcenter;
		box.Extents = bextent;
 		std::cout << "extent " << bextent.x << ' ' << bextent.y << ' ' << bextent.z << std::endl;
 		std::cout << "center " << bcenter.x << ' ' << bcenter.y << ' ' << bcenter.z << std::endl;
	GetUVName();
}

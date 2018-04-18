#pragma once
#include<fbxsdk.h>
#include<string>
#include<iostream>
#include<vector>
#include<unordered_map>
#include "structForFBX.h"
#include "GeometryGenerator.h"
class VertexHash;


class NewFBXLoader
{
	int numTabs = 0;
	FbxScene* mFbxScene = nullptr;
	FbxManager* mManager = nullptr;
	FbxNode* mRootNode = nullptr;


	//임시로 위치만 담아논 배열.
	std::vector< XMFLOAT3> vtxArr;

	std::unordered_map < GeometryGenerator::Vertex, unsigned int ,VertexHash,VertexCompare> forMapping;

public:
	NewFBXLoader();
	~NewFBXLoader();
	
	void LoadFBX(const char* fileName,GeometryGenerator::MeshData& outMesh,const float& scaleFactor=0.01f);
private:
	void ProcessControlPoint(FbxMesh* pMesh);
	void ProcessNode(FbxNode* pNode, GeometryGenerator::MeshData& outMesh,const float& scaleFactor);
	 XMFLOAT3 ProcessNormal(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	 XMFLOAT3 ProcessBinormal(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	 XMFLOAT3 ProcessTangent(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	 XMFLOAT2 ProcessUV(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& textureUVIndex);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	bool InsertData(const  XMFLOAT3& pos, const  XMFLOAT3& tangent,
		const  XMFLOAT3& normal, const  XMFLOAT2& tex, 
		GeometryGenerator::MeshData& outMesh,const float& scaleFactor);
	
	void Init(const char* fileName);
	void ProcessAll(GeometryGenerator::MeshData& outMesh,const float& scaleFactor);
	void Destroy();
};

/*std::ostream& operator<<(std::ostream& os, const Vertex& vtx);*/
//std::ostream& operator<<(std::ostream& os, const  XMFLOAT3& vtx);


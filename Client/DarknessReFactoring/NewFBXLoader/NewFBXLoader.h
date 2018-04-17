#pragma once
#pragma  comment(lib,"libfbxsdk-md.lib")
#include<fbxsdk.h>
#include<string>
#include<iostream>
#include<vector>
#include<DirectXMath.h>
#include<unordered_map>
#include "structForFBX.h"
class VertexHash;



class NewFBXLoader
{
	int numTabs = 0;
	FbxScene* mFbxScene = nullptr;
	FbxManager* mManager = nullptr;
	FbxNode* mRootNode = nullptr;


	//임시로 위치만 담아논 배열.
	std::vector<DirectX::XMFLOAT3> vtxArr;
	//최종인덱스 버퍼
	std::vector<unsigned int> m_Idx;
	//최종 버텍스 버퍼
	std::vector<Vertex> m_Vtx;

	std::unordered_map < Vertex, unsigned int ,VertexHash,VertexCompare> forMapping;

public:
	NewFBXLoader();
	~NewFBXLoader();
	
	void Init(const char* fileName);
	void ProcessAll();
	void Destroy();
private:
	void ProcessControlPoint(FbxMesh* pMesh);
	void ProcessNode(FbxNode* pNode);
	DirectX::XMFLOAT3 ProcessNormal(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	DirectX::XMFLOAT3 ProcessBinormal(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	DirectX::XMFLOAT3 ProcessTangent(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& vertexCount);
	DirectX::XMFLOAT2 ProcessUV(FbxMesh* pMesh, const unsigned int& controlPointIndex, const unsigned int& textureUVIndex);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	bool InsertData(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& tangent,
		const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& tex);

};

std::ostream& operator<<(std::ostream& os, const Vertex& vtx);
std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& vtx);


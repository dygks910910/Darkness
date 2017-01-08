#pragma once
#include<fbxsdk.h>
#include<iostream>
#include<string>
#include<vector>
#include "Vertex.h"
/*
2017 / 1 / 7 / 5:33
�ۼ���:�ڿ���(dygks910910@daum.net)
����:UV,Normal,Vertex ���� ������ �����ü� ����
*/
class CFbxLoader
{
	// Three floats for every position.
	const int VERTEX_STRIDE = 4;
	//����.
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
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:FbxManager,FbxScene,FBXImporter,RootNode Init.
	*/
	void Init(const char* pFileName);
	void Print();
	void PrintVertex();
	int GetSubMeshCount() const { return mSubMeshes.GetCount(); }
	void PrintElement();
	void Destroy();
	void LoadFBX(const char* pFileName, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib);
private:
	FbxManager* mpManager;
	FbxScene* mpScene;
	FbxImporter* mpImporter;
	FbxNode* mRootNode;
	int mlFileMajor, mlFileMinor, mlFileRevision;
private:
	void PrintNode(FbxNode* pNode);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	void PrintVertexByNode(FbxNode* pNode);
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void LoadElement(const FbxMesh* pMesh, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib);
	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};
	FbxArray<SubMesh*> mSubMeshes;
	bool mHasNormal;
	bool mHasUV;
	bool mAllByControlPoint; // Save data in VBO by control point or by polygon vertex.
};

std::ostream& operator<<(std::ostream& os, const FbxVector4& v4);
std::ostream& operator<<(std::ostream& os, const FbxVector2& v2);
#pragma once
#include "Utilities.h"
#include <unordered_map>
#include "MaterialFBX.h"
#include<iterator>
#include "d3dUtil.h"
class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter() {};
	bool Initialize();
	bool LoadScene(const char* inFileName);

	const int& GetVertexCnt() { return mVertices.size(); }
	const int& GetIndexCnt() { return mTriangleCount*3; }
	int SetVertex(Vertex* pVertex);
	int SetIndex(UINT* pIndex);
	void CleanupFbxManager();

private:
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	//삼각형들.
	std::vector<Triangle> mTriangles;
	//정점을 담고있음.
	std::vector<PNTIWVertex> mVertices;
	Skeleton mSkeleton;
	std::unordered_map<unsigned int, MaterialFBX*> mMaterialLookUp;
	FbxLongLong mAnimationLength;
	std::string mAnimationName;
	LARGE_INTEGER mCPUFreq;


private:
	void ProcessGeometry(FbxNode* inNode);
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessControlPoints(FbxNode* inNode);
	void ProcessJointsAndAnimations(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);
	void ProcessMesh(FbxNode* inNode);
	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outBinormal);
	void ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outTangent);
	void Optimize();
	int FindVertex(const PNTIWVertex& inTargetVertex, const std::vector<PNTIWVertex>& uniqueVertices);

	void AssociateMaterialToMesh(FbxNode* inNode);
	void ProcessMaterials(FbxNode* inNode);
	void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
	void PrintMaterial();
	void PrintTriangles();
	void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, MaterialFBX* ioMaterial);
	void WriteMeshToStream(std::ostream& inStream);
	void WriteAnimationToStream(std::ostream& inStream);
};
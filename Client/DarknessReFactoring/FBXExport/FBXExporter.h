#pragma once
#include "UtilitiesForFBX.h"
#include <unordered_map>
#include "MaterialForFBX.h"
#include"GeometryGenerator.h"
class FBXExporter
{
public:

	FBXExporter();
	~FBXExporter() {};
	bool Initialize();
	bool LoadScene(const char* inFileName, const char* inOutputPath);
	bool LoadScene(const char* inFileName);
	void ExportFBX();
	void ExportFBX(const float& scaleFactor);
	void LoadFBX(const char* inFileName, GeometryGenerator::MeshData* mesh, const float& scaleFactor);
private:
	GeometryGenerator::MeshData* mesh;
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	std::string mOutputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, myExporter::CtrlPoint*> mControlPoints; 
	unsigned int mTriangleCount;
	std::vector<myExporter::Triangle> mTriangles;
	std::vector<myExporter::PNTIWVertex> mVertices;
	myExporter::Skeleton mSkeleton;
	std::unordered_map<unsigned int, myExporter::Material*> mMaterialLookUp;
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
	int FindVertex(const myExporter::PNTIWVertex& inTargetVertex, const std::vector<myExporter::PNTIWVertex>& uniqueVertices);

	void AssociateMaterialToMesh(FbxNode* inNode);
	void ProcessMaterials(FbxNode* inNode);
	void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
	void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, myExporter::Material* ioMaterial);
	void PrintMaterial();
	void PrintTriangles();
	
	void CleanupFbxManager();
	void WriteMeshToStream(std::ostream& inStream);
	void WriteMeshToParameter(const float& scaleFactor);

	void WriteAnimationToStream(std::ostream& inStream);
};
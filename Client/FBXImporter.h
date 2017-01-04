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
	~FBXImporter();
	bool Initialize();
	bool LoadScene(const char* inFileName, const char* inOutputPath);

	void ImportFBX();
	int SetVertex(Vertex* pVertex);
	int SetIndex(UINT* pIndex);
	void CleanupFbxManager();

private:
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	std::string mOutputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	std::vector<Triangle> mTriangles;
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
	void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, MaterialFBX* ioMaterial);
	void PrintMaterial();
	void PrintTriangles();

	void WriteMeshToStream(std::ostream& inStream)
	{

		inStream << "<?xml version='1.0' encoding='UTF-8' ?>" << std::endl;
		inStream << "<itpmesh>" << std::endl;
		if (mHasAnimation)
		{
			inStream << "\t<!-- position, normal, skinning weights, skinning indices, texture-->" << std::endl;
			inStream << "\t<format>pnst</format>" << std::endl;
		}
		else
		{
			inStream << "\t<format>pnt</format>" << std::endl;
		}
		inStream << "\t<texture>" << mMaterialLookUp[0]->mDiffuseMapName << "</texture>" << std::endl;
		inStream << "\t<triangles count='" << mTriangleCount << "'>" << std::endl;

		for (unsigned int i = 0; i < mTriangleCount; ++i)
		{
			// We need to change the culling order
			inStream << "\t\t<tri>" << mTriangles[i].mIndices[0] << "," << mTriangles[i].mIndices[2] << "," << mTriangles[i].mIndices[1] << "</tri>" << std::endl;
		}
		inStream << "\t</triangles>" << std::endl;


		inStream << "\t<vertices count='" << mVertices.size() << "'>" << std::endl;
		for (unsigned int i = 0; i < mVertices.size(); ++i)
		{
			inStream << "\t\t<vtx>" << std::endl;
			inStream << "\t\t\t<pos>" << mVertices[i].mPosition.x << "," << mVertices[i].mPosition.y << "," << -mVertices[i].mPosition.z << "</pos>" << std::endl;
			inStream << "\t\t\t<norm>" << mVertices[i].mNormal.x << "," << mVertices[i].mNormal.y << "," << -mVertices[i].mNormal.z << "</norm>" << std::endl;
			if (mHasAnimation)
			{
				inStream << "\t\t\t<sw>" << static_cast<float>(mVertices[i].mVertexBlendingInfos[0].mBlendingWeight) << "," << static_cast<float>(mVertices[i].mVertexBlendingInfos[1].mBlendingWeight) << "," << static_cast<float>(mVertices[i].mVertexBlendingInfos[2].mBlendingWeight) << "," << static_cast<float>(mVertices[i].mVertexBlendingInfos[3].mBlendingWeight) << "</sw>" << std::endl;
				inStream << "\t\t\t<si>" << mVertices[i].mVertexBlendingInfos[0].mBlendingIndex << "," << mVertices[i].mVertexBlendingInfos[1].mBlendingIndex << "," << mVertices[i].mVertexBlendingInfos[2].mBlendingIndex << "," << mVertices[i].mVertexBlendingInfos[3].mBlendingIndex << "</si>" << std::endl;
			}
			inStream << "\t\t\t<tex>" << mVertices[i].mUV.x << "," << 1.0f - mVertices[i].mUV.y << "</tex>" << std::endl;
			inStream << "\t\t</vtx>" << std::endl;
		}

		inStream << "\t</vertices>" << std::endl;
		inStream << "</itpmesh>" << std::endl;
	}
	void WriteAnimationToStream(std::ostream& inStream);
};
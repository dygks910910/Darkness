#pragma once
#include "MathHelper.h"
#include<fbxsdk.h>
#include <string>
#include<unordered_map>
#include "Vertex.h"
#include "geome"
class NewFBXLoader
{
public:
	NewFBXLoader();
	~NewFBXLoader();

	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	std::string mOutputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	
	std::vector<unsigned int> indices;

	std::vector<Triangle> mTriangles;
	std::vector<PNTIWVertex> mVertices;

	LARGE_INTEGER mCPUFreq;

private:
	void GetControllPoints(FbxNode* inNode,GeometryGenerator::MeshData a);
};


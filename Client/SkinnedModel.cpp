#include "SkinnedModel.h"
#include "LoadM3d.h"
#include <iostream>
SkinnedModel::SkinnedModel(ID3D11Device* device, TextureMgr& texMgr,
	const std::string& modelFilename, const std::wstring& texturePath)
{
	std::vector<M3dMaterial> mats;
	M3DLoader m3dLoader;
	m3dLoader.LoadM3d(modelFilename, Vertices, Indices, Subsets, mats, SkinnedData);

	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size(),false);
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for (UINT i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);
		ID3D11ShaderResourceView* diffuseMapSRV = texMgr.CreateTexture(texturePath + mats[i].DiffuseMapName);
		DiffuseMapSRV.push_back(diffuseMapSRV);
		diffuseMapSRV = texMgr.CreateTexture(texturePath + L"true_clown_diffuse2.png");
		DiffuseMapSRV.push_back(diffuseMapSRV);
		diffuseMapSRV = texMgr.CreateTexture(texturePath + L"true_clown_diffuse3.png");
		DiffuseMapSRV.push_back(diffuseMapSRV);

		ID3D11ShaderResourceView* normalMapSRV = texMgr.CreateTexture(texturePath + mats[i].NormalMapName);
		NormalMapSRV.push_back(normalMapSRV);
	}
}
SkinnedModel::~SkinnedModel()
{
}
int a = 0;
void SkinnedModelInstance::Update(float dt)
{
	TimePos += dt;

	if (TimePos >= 0)
	{
		FinalTransforms = ((*mClipAnimbuf)[mClipnameAndTotalCount.first])[mAnimCnt];
		if (mClipnameAndTotalCount.first == "Death")
		{
			if (mAnimCnt > 287)
				mAnimCnt = 287;
			else
				mAnimCnt += 3;
		}
		else if (mClipnameAndTotalCount.first == "Attack1")
		{
			if (mAnimCnt == 168)
			{
				mAnimCnt = 168;
				mAttack = false;
				mAnimOneCheck = false;
			}
			else
				mAnimCnt += 3;
		}
		else if (mClipnameAndTotalCount.first == "Run")
			mAnimCnt += 5;
		else if (mClipnameAndTotalCount.first == "Walk")
			mAnimCnt += 4;
		else
			mAnimCnt+=3;
		// Loop animation
		if (mAnimCnt > mClipnameAndTotalCount.second - 1)
		{
			mAnimCnt = 0;
		}
		TimePos = 0.0f;
	}
}
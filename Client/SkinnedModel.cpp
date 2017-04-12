#include "SkinnedModel.h"
#include "LoadM3d.h"
#include <iostream>
SkinnedModel::SkinnedModel(ID3D11Device* device, TextureMgr& texMgr,
	const std::string& modelFilename, const std::wstring& texturePath)
{
	std::vector<M3dMaterial> mats;
	M3DLoader m3dLoader;
	m3dLoader.LoadM3d(modelFilename, Vertices, Indices, Subsets, mats, SkinnedData);

	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for(UINT i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);

		ID3D11ShaderResourceView* diffuseMapSRV = texMgr.CreateTexture(texturePath + mats[i].DiffuseMapName);
		DiffuseMapSRV.push_back(diffuseMapSRV);

		ID3D11ShaderResourceView* normalMapSRV = texMgr.CreateTexture(texturePath + mats[i].NormalMapName);
		NormalMapSRV.push_back(normalMapSRV);
	}
}
//std::ofstream fout("test2.txt");
SkinnedModel::~SkinnedModel()
{
}
//bool test = true;
int a = 0;
void SkinnedModelInstance::Update(float dt)
{
	TimePos += dt;
	if (TimePos >= 0.003)
	{
		//if (test)
		//{
			//Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);
		FinalTransforms = ((*mClipAnimbuf)[ClipName])[mAnimCnt];
		//FinalTransforms = TestFinalTransforms[a];
		mAnimCnt++;
		/*for (int i = 0; i < FinalTransforms.size(); ++i)
		{
			fout << FinalTransforms[i]._11 << ' ' << FinalTransforms[i]._12 << ' ' << FinalTransforms[i]._13 << ' ' << FinalTransforms[i]._14
				<< ' ' << FinalTransforms[i]._21 << ' ' << FinalTransforms[i]._22 << ' ' << FinalTransforms[i]._23 << ' ' << FinalTransforms[i]._24
				<< ' ' << FinalTransforms[i]._31 << ' ' << FinalTransforms[i]._32 << ' ' << FinalTransforms[i]._33 << ' ' << FinalTransforms[i]._34
				<< ' ' << FinalTransforms[i]._41 << ' ' << FinalTransforms[i]._42 << ' ' << FinalTransforms[i]._43 << ' ' << FinalTransforms[i]._44 << std::endl;
		}
		a++;*/

		// Loop animation
		if (mAnimCnt > mAnimTotalTime-1)//TimePos > Model->SkinnedData.GetClipEndTime(ClipName))
		{
			/*std::cout << a << std::endl;
			fout << a;*/
			//TimePos = 0.0f;
			mAnimCnt = 0;
			/*test = false;
			fout.close();*/
		}
		//}
		TimePos = 0.0f;
	}
}
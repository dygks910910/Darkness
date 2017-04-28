#ifndef SKINNEDMODEL_H
#define SKINNEDMODEL_H

#include "SkinnedData.h"
#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "Vertex.h"

class SkinnedModel
{
public:
	SkinnedModel(ID3D11Device* device, TextureMgr& texMgr,
		const std::string& modelFilename, const std::wstring& texturePath);
	~SkinnedModel();

	UINT SubsetCount;

	std::vector<Material> Mat;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTanSkinned> Vertices;
	std::vector<UINT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
	SkinnedData SkinnedData;
};

struct SkinnedModelInstance
{
	SkinnedModel* Model;
	float TimePos;
	XMFLOAT4X4 World;
	std::vector<XMFLOAT4X4> FinalTransforms;
	std::map < std::string, std::vector<XMFLOAT4X4>*>* mClipAnimbuf;
	std::pair<std::string, int> mClipnameAndTotalCount;
	int mAnimCnt;
	float mRotateAngle;
	XMVECTOR mLook;
	XMFLOAT3 mCharCamPos;
	int mAnimstate = 0;
	int mId;
	void Update(float dt);
};
#endif // SKINNEDMODEL_H
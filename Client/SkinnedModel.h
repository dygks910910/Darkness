#ifndef SKINNEDMODEL_H
#define SKINNEDMODEL_H

#include "SkinnedData.h"
#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include<Camera.h>

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
	bool isPlayer;
	XMFLOAT3 camPos;
	Camera* cam;
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
	bool mCollision = false;
	int selectedDiffuseMapIndex;
	int mAxis = 0;
	XMFLOAT3 mcammove = XMFLOAT3(0,0,0);
	bool mAttack = false;
	bool mAnimOneCheck = true;
	bool mWalkCheck = false;
	bool mRunStop = false;
	/////위아래왼쪽오른쪽 어떤게 눌렸는지 체크/////
	int mKeyState = 0;
	bool mAlive = true;
	void Update(float dt);

};
#endif // SKINNEDMODEL_H
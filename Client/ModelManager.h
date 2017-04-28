#pragma once
#include "NetworkMgr.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "FbxLoader.h"
#include "TextureMgr.h"
#include "Camera.h"
#include "RenderStates.h"
#include"LogoScene.h"
#include "SkinnedModel.h"

const int MAX_BUF_SIZE = 4000;

const int CS_UP = 1;
const int CS_DOWN = 2;
const int CS_LEFT = 3;
const int CS_RIGHT = 4;
const int CS_PACKET_START_ANIMATION = 5;

//cs_packet
struct cs_packet_player_move
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT3 campos;
	XMFLOAT3 camlook;
};



const int ANIMATION_STATE_IDLE = 0;
const int ANIMATION_STATE_ATTACK = 1;
const int ANIMATION_STATE_RUN = 2;
const int ANIMATION_STATE_WORK = 3;
struct cs_packet_player_anmation_start
{
	BYTE size;
	BYTE type;
	WORD id;
	UINT animationState;
};



class CModelManager
{
	ID3D11Device* mDevice;
	SkinnedModel* mCharacterModel;

	ID3D11Buffer* mStaticNormalMappingObjectVB;
	ID3D11Buffer* mStaticNormalMappingObjectIB;
	ID3D11Buffer* mStaticBasicObjectVB;
	ID3D11Buffer* mStaticBasicObjectIB;

	//ID3D11Buffer* mInstanceBuffer;

	std::vector<CStaticNomalModel> mStaticNormalModels;
	std::vector<CStaticBasicModel> mStaticBasicModels;
	std::vector<CInstanceBasicModel> mInstanceModels;
	std::vector<SkinnedModelInstance> mSkinnedModelInstance;

	std::map < std::string, std::vector<XMFLOAT4X4>*> mclipAnimbuf;
	std::pair<std::string, int> mClipnameAndTotalCounts[4];

	Material mGridMat;
	Material mBoxMat;
	//////////////////////////////////////////////////////////////////////////

	int gridVertexOffset;
	int gridIndexOffset;
	int gridIndexCount;

	int boxVertexOffset;
	int boxIndexOffset;
	int boxIndexCount;


	float mCheckAngle = 0;
	float mRotateAngle = 0.005;
	bool mOneCheck = true;
	// 	int clownIndexOffset;
	// 	int clownVertexOffset;
	// 	int clownIndexCount;

	int fenceIndexOffset, fenceVertexOffset, fenceIndexCount;
	int house1IndexOffset, house1VertexOffset, house1IndexCount;
	int house2IndexOffset, house2VertexOffset, house2IndexCount;
	int house3IndexOffset, house3VertexOffset, house3IndexCount;
	int house4IndexOffset, house4VertexOffset, house4IndexCount;
	int house5IndexOffset, house5VertexOffset, house5IndexCount;
	int house6IndexOffset, house6VertexOffset, house6IndexCount;

private:
	static CModelManager* model;
public:
	UINT id;

public:
	static CModelManager* GetInstance()
	{
		if (model == nullptr)
			model = new CModelManager();

		return model;

	}

	void DestroyInstance()
	{
		if (model)
		{
			delete model;
			model = nullptr;
		}
	}

private:
	CModelManager();
public:
	~CModelManager();
	ID3D11Buffer* GetStaticNormalMappingObjectVB() { return mStaticNormalMappingObjectVB; }
	ID3D11Buffer* GetStaticNormalMappingObjectIB() { return mStaticNormalMappingObjectIB; }
	ID3D11Buffer* GetStaticBasicObjectVB() { return mStaticBasicObjectVB; }
	ID3D11Buffer* GetStaticBasicObjectIB() { return mStaticBasicObjectIB; }
	//ID3D11Buffer* GetInstanceBuffer() { return mInstanceBuffer; }
	std::vector<CStaticNomalModel>& GetStaticNormalModels() { return mStaticNormalModels; }
	std::vector<CStaticBasicModel>& GetStaticBasicModels() { return mStaticBasicModels; }
	std::vector<SkinnedModelInstance>& GetSkinnedInstanceModels() { return mSkinnedModelInstance; }

	void Init(TextureMgr& texMgr, Camera& cam, ID3D11Device* device);
	void DrawStaticNormalModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawStaticBasicModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);

	void DrawStaticSsaoNormalModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawSkinnedModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);

	void DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech,
		const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj);
	void DrawInstancedModel(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void UpdateModel(const float& dt, Camera& camera);


	BYTE   send_buf[MAX_BUF_SIZE];
	WSABUF   send_wsa_buf;

private:
	void BuildShapeGeometryBuffers();
	void BuildBasicGeometryBuffer();
	void ReadMapData(TextureMgr& texMgr, Camera& cam);
};
enum ANIMATION_INFO
{
	IDLE,
	WARK,
	ATTACT,
	RUN
};

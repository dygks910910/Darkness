#pragma once
#include "NetworkMgr.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "FbxLoader.h"
#include "TextureMgr.h"
#include "Camera.h"
#include "RenderStates.h"
#include "SkinnedModel.h"
#include "xnacollision.h"
#include "ButtonClass.h"

enum Axis
{
	x = 0,
	z = 1
};

enum Keystate
{
	up = 1,
	down = 2,
	left = 3,
	right = 4,
	die = 5,
	run = 6

};

enum Animstate
{
	idle = 0,
	walk = 1,
	attack = 2,
	runnning = 3,
	death = 4
};

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
	CBitMap mLoadingScene2;


	ID3D11Device* mDevice;
	SkinnedModel* mCharacterModel;

	ID3D11Buffer* mStaticNormalMappingObjectVB;
	ID3D11Buffer* mStaticNormalMappingObjectIB;
	ID3D11Buffer* mStaticBasicObjectVB;
	ID3D11Buffer* mStaticBasicObjectIB;


	std::vector<CStaticNomalModel> mStaticNormalModels;
	std::vector<CStaticBasicModel> mStaticBasicModels;
	std::vector<CInstanceBasicModel> mInstanceModels;
	std::vector<SkinnedModelInstance> mSkinnedModelInstance;

	std::map < std::string, std::vector<XMFLOAT4X4>*> mclipAnimbuf;
	std::pair<std::string, int> mClipnameAndTotalCounts[5];

	Material mGridMat;
	Material mObjectMaterial;
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


	int angelStatueIndexOffset, angelStatueVertexOffset, angelStatueIndexCount;
	int wallIndexOffset, wallVertexOffset, wallIndexCount;
	int tower_conerIndexOffset, tower_conerVertexOffset, tower_conerIndexCount;
	int tower_roundIndexOffset, tower_roundVertexOffset, tower_roundIndexCount;
	int building_b_IndexOffset, building_b_VertexOffset, building_b_IndexCount;
	int building_c_IndexOffset, building_c_VertexOffset, building_c_IndexCount;
	int building_d_IndexOffset, building_d_VertexOffset, building_d_IndexCount;
	int building_e_IndexOffset, building_e_VertexOffset, building_e_IndexCount;
	int building_f_IndexOffset, building_f_VertexOffset, building_f_IndexCount;
	int barrel_IndexOffset, barrel_VertexOffset, barrel_IndexCount;

	
	int well_IndexOffset, well_VertexOffset, well_IndexCount;
	int food_a_IndexOffset, food_a_VertexOffset, food_a_IndexCount;
	int food_b_IndexOffset, food_b_VertexOffset, food_b_IndexCount;
	int hay_a_IndexOffset, hay_a_VertexOffset, hay_a_IndexCount;
	int hay_b_IndexOffset, hay_b_VertexOffset, hay_b_IndexCount;
	int hay_c_IndexOffset, hay_c_VertexOffset, hay_c_IndexCount;
	int hay_d_IndexOffset, hay_d_VertexOffset, hay_d_IndexCount;
	int sack_a_IndexOffset, sack_a_VertexOffset, sack_a_IndexCount;
	int sack_b_IndexOffset, sack_b_VertexOffset, sack_b_IndexCount;
	int sewers_entrance_IndexOffset, sewers_entrance_VertexOffset, sewers_entrance_IndexCount;
	int tent_indexOffset, tent_VertexOffset, tent_indexCount;
	int crate_indexOffset, crate_VertexOffset, crate_indexCount;

	XNA::OrientedBox angelStatueBox, wallBox, tower_cornerBox, tower_roundBox, building_bBox,
		building_cBox,
		building_dBox, building_eBox, building_fBox, barrelBox,
		wellBox, food_aBox, food_bBox, hay_aBox, hay_bBox, hay_cBox, hay_dBox,
		sack_aBox, sack_bBox, sewers_entranceBox, tentBox, crateBox;
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
	std::vector<CStaticNomalModel>& GetStaticNormalModels() { return mStaticNormalModels; }
	std::vector<CStaticBasicModel>& GetStaticBasicModels() { return mStaticBasicModels; }
	std::vector<SkinnedModelInstance>& GetSkinnedInstanceModels() { return mSkinnedModelInstance; }

	void Init(TextureMgr& texMgr, Camera* cam, ID3D11Device* device, ID3D11DeviceContext* dc, IDXGISwapChain* swapChain, ID3D11DepthStencilState* tDepthDisableState);
	void DrawStaticNormalModels(ID3D11DeviceContext* dc,ID3DX11EffectTechnique* tech,const XMFLOAT4X4& shadowTransform,const Camera& cam);

	void DrawStaticBasicModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);

	void DrawStaticSsaoNormalModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawSkinnedModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);

	void DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech,
		const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj);
	void DrawInstancedModel(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam);
	void UpdateModel(const float& dt, Camera& camera);


	BYTE   send_buf[MAX_BUF_SIZE];
	WSABUF   send_wsa_buf;

	XMFLOAT4X4 boundworld;

	int mMyId;

	//�г��� ���̵�
	int mNickId[8];

	std::wstring mMyNick[8] = { L"" };

	bool mRoomHeader;

	bool mCheck = false;

	bool mIsStart = false;


	/////��Ʈ��ũ initpos�������� Ȯ���ϴ� ����
	bool m_bFinishInit = false;

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

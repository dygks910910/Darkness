#pragma once
#include "Model.h"
#include "GeometryGenerator.h"
#include "FbxLoader.h"
#include "TextureMgr.h"
#include "Camera.h"
#include "RenderStates.h"
#include"LogoScene.h"
#include "SkinnedModel.h"
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

// 	int clownIndexOffset;
// 	int clownVertexOffset;
// 	int clownIndexCount;

// 	int fenceIndexOffset, fenceVertexOffset, fenceIndexCount;
// 	int house1IndexOffset, house1VertexOffset, house1IndexCount;
// 	int house2IndexOffset, house2VertexOffset, house2IndexCount;
// 	int house3IndexOffset, house3VertexOffset, house3IndexCount;
// 	int house4IndexOffset, house4VertexOffset, house4IndexCount;
// 	int house5IndexOffset, house5VertexOffset, house5IndexCount;
// 	int house6IndexOffset, house6VertexOffset, house6IndexCount;
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
	





public:
	CModelManager();
	~CModelManager();
	ID3D11Buffer* GetStaticNormalMappingObjectVB() { return mStaticNormalMappingObjectVB; }
	ID3D11Buffer* GetStaticNormalMappingObjectIB() { return mStaticNormalMappingObjectIB; }
	ID3D11Buffer* GetStaticBasicObjectVB() { return mStaticBasicObjectVB; }
	ID3D11Buffer* GetStaticBasicObjectIB() { return mStaticBasicObjectIB; }
	//ID3D11Buffer* GetInstanceBuffer() { return mInstanceBuffer; }
	std::vector<CStaticNomalModel>& GetStaticNormalModels() { return mStaticNormalModels; }
	std::vector<CStaticBasicModel>& GetStaticBasicModels() { return mStaticBasicModels; }


	void Init(TextureMgr& texMgr, Camera* cam, ID3D11Device* device);
	void DrawStaticNormalModels(ID3D11DeviceContext* dc,ID3DX11EffectTechnique* tech,const XMMATRIX& shadowTransform,const Camera& cam);
	void DrawStaticBasicModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);

	void DrawStaticSsaoNormalModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void DrawSkinnedModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);

	void DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, 
		const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj);
	void DrawInstancedModel(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam);
	void UpdateModel(const float& dt);
private:
	void BuildShapeGeometryBuffers();
	void BuildBasicGeometryBuffer();
	void ReadMapData(TextureMgr& texMgr,Camera* cam);
};
enum ANIMATION_INFO
{
	IDLE,
	WARK,
	ATTACT,
	RUN
};

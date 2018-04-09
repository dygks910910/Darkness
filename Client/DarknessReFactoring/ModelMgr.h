#pragma once
#include "Global.h"
#include "Model.h"
#include "Vertex.h"
#include "Effects.h"
#include "FbxLoader.h"
#include"BasicModel.h"
#include "TextureMgr.h"
#include "NormalModel.h"
class ModelMgr
{
	ID3D11Buffer* m_NomalVB;
	ID3D11Buffer* m_NomalIB;

	ID3D11Buffer* VB;
	ID3D11Buffer* IB;
	TextureMgr txtureMgr;
	NormalModel* a;
	int vtxOff;
	int idxCnt;
	int idxOff;
public:
	ModelMgr();
	~ModelMgr();
	void DrawAll(const Camera& cam);
	void Update(const float& dt);
	void tempLoad();
	void BuildFBXBasic32Buffers(const GeometryGenerator::MeshData& box);
	void BuildFBXNormalBuffers(const GeometryGenerator::MeshData& box);


};


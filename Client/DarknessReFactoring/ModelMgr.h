#pragma once
#include "Global.h"
#include "Model.h"
#include "Vertex.h"
#include "Effects.h"
#include "FbxLoader.h"
#include"BasicModel.h"
#include "TextureMgr.h"
class ModelMgr
{

	ID3D11Buffer* VB;
	ID3D11Buffer* IB;
	TextureMgr txtureMgr;
	Model* a;
	int vtxOff;
	int idxCnt;
	int idxOff;
public:
	ModelMgr();
	~ModelMgr();
	void DrawAll(const Camera& cam);
	void Update(const float& dt);
	void tempLoad();
	void BuildFBXBuffers(const GeometryGenerator::MeshData& box);

};


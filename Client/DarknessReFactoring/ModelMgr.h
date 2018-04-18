#pragma once
#include "Global.h"
#include "Vertex.h"
#include "Effects.h"
#include "TextureMgr.h"
#include "NormalModel.h"
#include<string>
#include<map>
#include"Common\GeometryGenerator.h"
#include "NewFBXLoader\NewFBXLoader.h"

class ModelMgr
{
	ID3D11Buffer* m_Normal_VB;
	ID3D11Buffer* m_Basic_VB;
	ID3D11Buffer* m_IB;

	TextureMgr txtureMgr;

	std::map<std::string, Model*> models;

	BasicModel* a;
	int vtxOff;
	int idxCnt;
	int idxOff;

	friend class BasicModel;
	friend class NormalModel;
public:
	ModelMgr();
	~ModelMgr();
	void DrawAll(const Camera& cam);
	void Update(const float& dt);
	void tempLoad();
	void BuildFBXBasic32Buffers(const GeometryGenerator::MeshData& box);
	void BuildFBXNormalBuffers(const GeometryGenerator::MeshData& box);
	void ReadMapData(char* fileName);


};


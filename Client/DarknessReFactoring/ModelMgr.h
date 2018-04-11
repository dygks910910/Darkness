#pragma once
#include "Global.h"
#include "Vertex.h"
#include "Effects.h"
#include "FbxLoader.h"
#include "TextureMgr.h"
#include "NormalModel.h"
#include<string>
#include<map>
class ModelMgr
{

	ID3D11Buffer* m_Normal_VB;
	ID3D11Buffer* m_Basic_VB;
	ID3D11Buffer* m_IB;

	TextureMgr txtureMgr;
	//NormalModel* a;

	std::map<std::string, Model*> models;

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


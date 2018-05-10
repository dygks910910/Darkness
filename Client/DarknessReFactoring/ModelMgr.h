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
	ID3D11Buffer* m_Basic_IB;
	ID3D11Buffer* m_Normal_IB;

	TextureMgr txtureMgr;

	std::map<std::string, Model*> models;

	//NormalModel* a;
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

private:
	Model *CreateNormalModel(const XMFLOAT3 & s, const XMFLOAT4 & r, const XMFLOAT3 & t,
		const std::string & diffuseFileName, const std::string & normalFileName, 
		const UINT& idxCount, const UINT& vtxOffset, const UINT& IdxOffset);
	Model *CreateBasicModel(const XMFLOAT3 & s, const XMFLOAT4 & r, const XMFLOAT3 & t,
		const std::string & diffuseFileName,const UINT& idxCount,
		const UINT& vtxOffset, const UINT& IdxOffset);
	bool MeshToArr(std::vector<Vertex::PosNormalTexTan>& vtxArr, std::vector<UINT>& idxArr, const GeometryGenerator::MeshData& mesh);
	bool MeshToArr(std::vector<Vertex::Basic32>& vtxArr, std::vector<UINT>& idxArr, const GeometryGenerator::MeshData& mesh);


};


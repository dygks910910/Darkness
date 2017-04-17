#pragma once
#include<vector>
#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include<Camera.h>
#include "LoadM3d.h"
class CStaticModel
{
public:
	CStaticModel(ID3D11Device* device, TextureMgr& texMgr,
		const std::string& modelFilename, const std::wstring& texturePath);
	UINT SubsetCount;

	std::vector<Material> Mat;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTan> Vertices;
	std::vector<UINT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
	~CStaticModel();
};

struct StaticModelInstance
{
	bool isPlayer;
	CStaticModel* Model;
	XMFLOAT4X4 World;
	std::vector<XMFLOAT4X4> FinalTransforms;
};

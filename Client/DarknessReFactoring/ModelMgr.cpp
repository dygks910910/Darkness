#include "ModelMgr.h"



ModelMgr::ModelMgr()
{
	

	CFbxLoader loader;
	GeometryGenerator::MeshData tmep;
	loader.LoadFBX("Darkness fbx\\house 1.fbx", tmep, 0.1f);
	XMFLOAT4X4 tempMtx;
	XMStoreFloat4x4(&tempMtx, XMMatrixIdentity());
	
	
	BuildFBXBuffers(tmep);
	a = new BasicModel();
	a->SetIdxOffset(idxOff);
	a->SetIdxCnt(idxCnt);
	a->SetVtxOffset(vtxOff);
	ID3D11ShaderResourceView* tempSRV = txtureMgr.CreateTexture(L"Textures/WoodCrate01.dds");
	a->SetTexture(tempSRV);
	
	a->AddInstance(tempMtx);
	XMStoreFloat4x4(&tempMtx, XMMatrixTranslation(0, 3, 0));
	a->AddInstance(tempMtx);
	a->BuildInstanceBuffer();
}


ModelMgr::~ModelMgr()
{
	delete a;
}

void ModelMgr::DrawAll(const Camera& cam)
{
	a->Draw(cam, VB, IB);
}

void ModelMgr::Update(const float & dt)
{
	
}

void ModelMgr::tempLoad()
{
	
}

void ModelMgr::BuildFBXBuffers(const GeometryGenerator::MeshData & box)
{
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	vtxOff= 0;

	// Cache the index count of each object.
	idxCnt = box.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	idxOff = 0;

	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount =idxCnt;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &VB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &IB));
}

#include "Plane.h"



CPlane::CPlane()
{
}


CPlane::~CPlane()
{
}

void CPlane::Init(ID3D11Device * d3ddevice)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(10.0f, 0.5f, 10.0f, box);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].Tex = box.Vertices[i].TexC;
	}

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
	for (int i = 0; i < box.Vertices.size(); ++i)
	{
		vMin = XMVectorMin(vMin, XMLoadFloat3(&box.Vertices[i].Position));
		vMax = XMVectorMax(vMax, XMLoadFloat3(&box.Vertices[i].Position));
	}
	XMStoreFloat3(&mColisionBox.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&mColisionBox.Extents, 0.5f*(vMax - vMin));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(d3ddevice->CreateBuffer(&vbd, &vinitData, &mObjVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];
	HR(d3ddevice->CreateBuffer(&ibd, &iinitData, &mObjIB));

	HR(D3DX11CreateShaderResourceViewFromFile(d3ddevice,
		L"Textures/WireFence.dds", 0, 0, &mObjMapSRV, 0));

	mObjMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mObjMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mObjMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	XMMATRIX boxScail = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 3.0f, 120.0f);
	XMStoreFloat4x4(&mObjWorld, boxScail*boxOffset);
}

void CPlane::Draw(ID3D11DeviceContext * md3dImmediateContext, Camera mCam)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* boxTech;
	boxTech = Effects::BasicFX->Light0TexAlphaClipTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	boxTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mObjVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mObjIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mObjWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.View()*mCam.Proj();

		//Effects::BasicFX->SetWorld(world);
		//Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(mObjMat);
		Effects::BasicFX->SetDiffuseMap(mObjMapSRV);

		//md3dImmediateContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		boxTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}
}
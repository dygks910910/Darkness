#include "Building.h"



CBuilding::CBuilding()
{
}


CBuilding::~CBuilding()
{
	

}

void CBuilding::SetWorld(const XMFLOAT4X4 & worldmtx)
{
	mObjWorld = worldmtx;
}

void CBuilding::Init(ID3D11Device * d3ddevice)
{
	md3dDevice = d3ddevice;
	CFbxLoader loader;
	std::vector<Vertex::Basic32> vb;
	std::vector<UINT> ib;

	loader.LoadFBX("Darkness FBX\\crawler.fbx",vb,ib);
	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vb.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vb[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mObjVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * ib.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &ib[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mObjIB));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Darkness fbx/crawler_diffuse 1.png", 0, 0, &mObjMapSRV, 0));

	mObjMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mObjMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mObjMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 8.0f, 120.0f);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(0, 45, 0);
	XMStoreFloat4x4(&mObjWorld, rotation*boxOffset );
	indecesCount = ib.size();
	vb.clear();
	ib.clear();
	loader.Destroy();
}

void CBuilding::Draw(ID3D11DeviceContext * md3dImmediateContext, const Camera& mCam)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* boxTech;
	boxTech = Effects::BasicFX->Light0TexTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	boxTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mObjVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mObjIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mObjWorld);
		//XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.View()*mCam.Proj();

		//Effects::BasicFX->SetWorld(world);
		//Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		//Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(mObjMat);
		Effects::BasicFX->SetDiffuseMap(mObjMapSRV);

		//md3dImmediateContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		boxTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(indecesCount, 0, 0);
	}
}

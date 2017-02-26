#include "Building.h"


std::ostream& operator<<(std::ostream& os, const XMFLOAT3& float3)
{
	os << "(" << float3.x << "," << float3.y << "," << float3.z << ")" << std::endl;
	return os;
}

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

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
	for (int i = 0; i < vb.size(); ++i)
	{
		vMin = XMVectorMin(vMin, XMLoadFloat3(&vb[i].Pos));
		vMax = XMVectorMax(vMax, XMLoadFloat3(&vb[i].Pos));
	}
	XMStoreFloat3(&mColisionBox.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&mColisionBox.Extents, 0.5f*(vMax - vMin));

	mCoord.Init(d3ddevice, mObjWorld, 5);
	std::cout <<"colisionBox Center:"<< mColisionBox.Center << std::endl;
	
	


	indecesCount = ib.size();
	vb.clear();
	ib.clear();
	loader.Destroy();
}

void CBuilding::Draw(ID3D11DeviceContext* dc, const Camera& mCam)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->RSSetState(RenderStates::SolidRS);
	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* boxTech;
	boxTech = Effects::BasicFX->Light0TexTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	boxTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &mObjVB, &stride, &offset);
		dc->IASetIndexBuffer(mObjIB, DXGI_FORMAT_R32_UINT, 0);

			// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mObjWorld);
		//XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.ViewProj();

		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		//Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(mObjMat);
		Effects::BasicFX->SetDiffuseMap(mObjMapSRV);

		//md3dImmediateContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		boxTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(indecesCount, 0, 0);
	}

	dc->IASetInputLayout(InputLayouts::Line);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	XMMATRIX worldmtx = XMLoadFloat4x4(&mObjWorld);
	worldmtx =  XMMatrixTranslation(mColisionBox.Center.x, mColisionBox.Center.y, mColisionBox.Center.z) * worldmtx;
	mCoord.SetWorld(mObjWorld);
	mCoord.Draw(dc, mCam);
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

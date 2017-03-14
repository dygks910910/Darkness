#include "FBXObject.h"



void CFBXObject::SetWorld(const XMFLOAT4X4 & worldmtx)
{
	mObjWorld = worldmtx;
#if defined(DEBUG) || defined(_DEBUG)  
	mCoord.SetWorld(worldmtx);
#endif
}

void CFBXObject::Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr)
{
	ID3D11Buffer* tempIB;
	ID3D11Buffer* tempVB;
	
	std::vector<Vertex::Basic32> vb;
	std::vector<UINT> ib;
	if (modelMgr->CheckHasModel(mFilename.c_str()))
	{
		mModel = modelMgr->CreateModelFBX(mFilename.c_str(), vb, ib);
	}
	else {
		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//
		mModel = modelMgr->CreateModelFBX(mFilename.c_str(), vb, ib);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::Basic32) * vb.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vb[0];
		HR(device->CreateBuffer(&vbd, &vinitData, &tempVB));
		mModel->SetVB(tempVB);
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
		HR(device->CreateBuffer(&ibd, &iinitData, &tempIB));
		mModel->SetIB(tempIB);
		mModel->SetIndexCount(ib.size());
	}

	mObjMapSRV =  textureMgr->CreateTexture(mDiffuseMapName);
	
	mObjMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mObjMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mObjMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);


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

	
	//std::cout <<"colisionBox Center:"<< mColisionBox.Center << std::endl;
#if defined(DEBUG) || defined(_DEBUG)  
	mCoord.Init(device, mObjWorld, 5);
#endif
	vb.clear();
	ib.clear();
}

void CFBXObject::Draw(ID3D11DeviceContext * dc, Camera mCam)
{
	ID3D11Buffer* ib = mModel->GetIB();
	ID3D11Buffer* vb = mModel->GetVB();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->RSSetState(RenderStates::NoCullRS);

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* boxTech;
	boxTech = Effects::BasicFX->Light2TexTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	boxTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		dc->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mObjWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.ViewProj();

		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetMaterial(mObjMat);
		Effects::BasicFX->SetDiffuseMap(mObjMapSRV);

		boxTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mModel->GetIndexCount(), 0, 0);
	}
	dc->IASetInputLayout(InputLayouts::Line);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	XMMATRIX worldmtx = XMLoadFloat4x4(&mObjWorld);
	worldmtx = XMMatrixTranslation(mColisionBox.Center.x, mColisionBox.Center.y, mColisionBox.Center.z) * worldmtx;
	mCoord.SetWorld(mObjWorld);
	mCoord.Draw(dc, mCam);
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

CFBXObject::CFBXObject()
{
}


CFBXObject::~CFBXObject()
{
	mFilename.clear();
	mDiffuseMapName.clear();
}

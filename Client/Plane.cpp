#include "Plane.h"



void CPlane::Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr)
{
	ID3D11Buffer* tempVB;
	ID3D11Buffer* tempIB;
	if (modelMgr->CheckHasModel("Plane"))
	{
		mModel = modelMgr->GetModel("Plane");
	}
	else
	{
		mModel = new CModel;
		Vertex::Basic32 verteces[4];

		verteces[0].Pos = XMFLOAT3(+PLANE_SIZE / 2, 0, -PLANE_SIZE / 2);
		verteces[0].Normal = XMFLOAT3(0, 1, 0);
		verteces[0].Tex = XMFLOAT2(0, 1);

		verteces[1].Pos = XMFLOAT3(-PLANE_SIZE / 2, 0, -PLANE_SIZE / 2);
		verteces[1].Normal = XMFLOAT3(0, 1, 0);
		verteces[1].Tex = XMFLOAT2(0, -1);

		verteces[2].Pos = XMFLOAT3(+PLANE_SIZE / 2, 0, +PLANE_SIZE / 2);
		verteces[2].Normal = XMFLOAT3(0, 1, 0);
		verteces[2].Tex = XMFLOAT2(1, 0);

		verteces[3].Pos = XMFLOAT3(-PLANE_SIZE / 2, 0, +PLANE_SIZE / 2);
		verteces[3].Normal = XMFLOAT3(0, 1, 0);
		verteces[3].Tex = XMFLOAT2(-1, 0);

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::Basic32) * 4;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = verteces;
		HR(device->CreateBuffer(&vbd, &vinitData, &tempVB));


		UINT indices[6] = { 0,1,2,2,1,3 };
		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * 6;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		HR(device->CreateBuffer(&ibd, &iinitData, &tempIB	));
		
		
		mModel->SetVB(tempVB);
		mModel->SetIB(tempIB);
		mModel->SetIndexCount(6);
		modelMgr->InsertModel("Plane", mModel);
	}
	

	HR(D3DX11CreateShaderResourceViewFromFile(device,
		L"Textures/floor.dds", 0, 0, &mObjMapSRV, 0));

	mObjMat.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mObjMat.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mObjMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mObjMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	
}

void CPlane::Draw(ID3D11DeviceContext * dc, Camera mCam)
{

	ID3D11Buffer* tempVB = mModel->GetVB();
	ID3D11Buffer* tempIB = mModel->GetIB();

	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->RSSetState(RenderStates::SolidRS);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* boxTech;
	boxTech = Effects::BasicFX->Light2TexTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	boxTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &tempVB, &stride, &offset);
		dc->IASetIndexBuffer(tempIB, DXGI_FORMAT_R32_UINT, 0);

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
		boxTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mModel->GetIndexCount(), 0, 0);
	}
}

CPlane::CPlane()
{
}


CPlane::~CPlane()
{
}

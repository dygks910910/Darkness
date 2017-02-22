#include "Cordinate.h"



void CCordinate::Init(ID3D11Device* device,
	const XMFLOAT4X4& world,
	const float& lineLength)
{
	Vertex::Line vertex[6];
	//-x축
	vertex[0].Pos = XMFLOAT3(-lineLength,0,0);
	vertex[0].color = XMFLOAT4(255, 0, 0, 0);
	//+x축
	vertex[1].Pos= XMFLOAT3(lineLength,0,0);

	vertex[1].color = XMFLOAT4(255, 0, 0, 0);

	//-y축
	vertex[2].Pos = XMFLOAT3(0, -lineLength,0);
	vertex[2].color = XMFLOAT4(0, 255, 0, 0);

	//+y축
	vertex[3].Pos = XMFLOAT3(0, lineLength,0);
	vertex[3].color = XMFLOAT4(0, 255, 0, 0);

	//-z축
	vertex[4].Pos= XMFLOAT3(0,0, -lineLength);
	vertex[4].color = XMFLOAT4(0, 0, 255, 0);

	//+z축
	vertex[5].Pos= XMFLOAT3(0,0, lineLength);
	vertex[5].color = XMFLOAT4(0, 0, 255, 0);


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Line) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertex;

	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	UINT indeces[6];
	for (int i = 0; i < 6; ++i)
	{
		indeces[i] = i;
	}
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indeces;
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));

	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

}

void CCordinate::Draw(ID3D11DeviceContext * md3dImmediateContext, Camera mCam)
{

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* lineTech;
	lineTech = Effects::LineFX->mTech;
	UINT stride = sizeof(Vertex::Line);
	UINT offset = 0;
	lineTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX worldViewProj = world*mCam.View()*mCam.Proj();

		Effects::LineFX->SetWorldViewProj(worldViewProj);

		lineTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
	}
}


CCordinate::CCordinate()
{
}


CCordinate::~CCordinate()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

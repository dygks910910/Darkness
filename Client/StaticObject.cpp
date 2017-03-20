#include "StaticObject.h"

void CStaticObject::DrawToShadowMap(ID3D11DeviceContext * dc, Camera mCam)
{
	ID3D11Buffer* ib = mModel->GetIB();
	ID3D11Buffer* vb = mModel->GetVB();
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* animatedSmapTech = Effects::BuildShadowMapFX->BuildShadowMapSkinnedTech;
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	dc->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);


	D3DX11_TECHNIQUE_DESC techDesc;
	smapTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		dc->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mObjWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.ViewProj();

		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		smapTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mModel->GetIndexCount(), 0, 0);
	}
}

void CStaticObject::DrawToSsaoNormalDepthMap(ID3D11DeviceContext * dc, Camera mCam)
{
	ID3D11Buffer* ib = mModel->GetIB();
	ID3D11Buffer* vb = mModel->GetVB();
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;

	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormalDepthFX->NormalDepthTech;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	dc->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		world = XMLoadFloat4x4(&mObjWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mModel->GetIndexCount(), 0, 0);
	}
}

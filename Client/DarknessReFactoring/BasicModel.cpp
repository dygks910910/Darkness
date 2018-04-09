#include "BasicModel.h"



BasicModel::BasicModel()
{
}


BasicModel::~BasicModel()
{

}

void BasicModel::Draw(const Camera & cam, ID3D11Buffer* vb, ID3D11Buffer* ib)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	md3dImmediateContext->IASetInputLayout(InputLayouts::InstancedBasic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT instanceStride[2] = { sizeof(Vertex::Basic32), sizeof(XMFLOAT4X4) };
	UINT instanceOffset[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { vb,mInstanceBuffer };

	XMMATRIX view = cam.View();
	XMMATRIX proj = cam.Proj();
	XMMATRIX viewProj = cam.ViewProj();

	Effects::InstancedBasicFX->SetEyePosW(cam.GetPosition());

	ID3DX11EffectTechnique* activeTech = Effects::InstancedBasicFX->Light0TexTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, instanceStride, instanceOffset);
		md3dImmediateContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::InstancedBasicFX->SetDiffuseMap(mTexSRV);
 		Effects::InstancedBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::InstancedBasicFX->SetWorld(world);
 		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		//Effects::InstancedBasicFX->SetMaterial(mObjMatrial);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexedInstanced(mIndexCount, mInstanceWorld.size(),
			mIndexOffset, mVertexOffset, 0);
	}
	HR(mSwapChain->Present(0, 0));
}

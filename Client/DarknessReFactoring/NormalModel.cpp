#include "NormalModel.h"
#include "Material.h"


NormalModel::NormalModel()
{
	mTexNormalSRV = nullptr;
	
}


NormalModel::~NormalModel()
{
	ReleaseCOM(mTexSRV);
}

void NormalModel::Draw(const Camera & cam, ID3D11Buffer * vb, ID3D11Buffer * ib)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::InstancedNormal);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT instanceStride[2] = { sizeof(Vertex::PosNormalTexTan), sizeof(XMFLOAT4X4) };
	UINT instanceOffset[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { vb,mInstanceBuffer };

	XMMATRIX worldInvTranspose;
	XMMATRIX viewProj;
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::InstancedNormalFX->SetEyePosW(cam.GetPosition());

	ID3DX11EffectTechnique *tech =  Effects::InstancedNormalFX->Light3TexReflectTech;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, instanceStride, instanceOffset);
		md3dImmediateContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
		
		XMMATRIX world = XMMatrixIdentity();

		worldInvTranspose = MathHelper::InverseTranspose(world);
		viewProj = cam.View()*cam.Proj();

		//Effects::InstancedNormalFX->SetWorld(world);
		Effects::InstancedNormalFX->SetWorldInvTranspose(worldInvTranspose);
		//Effects::InstancedNormalFX->SetWorldViewProj(worldViewProj);
		Effects::InstancedNormalFX->SetMaterial(MeterialList::Brick);
		Effects::InstancedNormalFX->SetViewProj(viewProj);
		Effects::InstancedNormalFX->SetTexTransform(XMMatrixIdentity());
		Effects::InstancedNormalFX->SetDiffuseMap(mTexSRV);
		Effects::InstancedNormalFX->SetNormalMap(mTexNormalSRV);

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexedInstanced(mIndexCount, mInstanceWorld.size(),
			mIndexOffset, mVertexOffset, 0);
	}
	HR(mSwapChain->Present(0, 0));

}

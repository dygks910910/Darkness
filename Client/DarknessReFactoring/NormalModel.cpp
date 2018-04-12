#include "NormalModel.h"
#include "Material.h"
#include"ModelMgr.h"

NormalModel::NormalModel()
{
	mTexNormalSRV = nullptr;
	
}


NormalModel::~NormalModel()
{
	ReleaseCOM(mTexSRV);
}

void NormalModel::Draw(const Camera & cam, const ModelMgr* mgr)
{
	

	md3dImmediateContext->IASetInputLayout(InputLayouts::InstancedNormal);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT instanceStride[2] = { sizeof(Vertex::PosNormalTexTan), sizeof(XMFLOAT4X4) };
	UINT instanceOffset[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { mgr->m_Basic_VB,mInstanceBuffer };

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
		md3dImmediateContext->IASetIndexBuffer(mgr->m_IB , DXGI_FORMAT_R32_UINT, 0);
		
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
		md3dImmediateContext->DrawIndexedInstanced(m_Info.mIndexCount, m_Info.mInstanceWorld.size(),
			m_Info.mIndexOffset, m_Info.mVertexOffset, 0);
	}
}

#include "StaticNormalModel.h"



CStaticNomalModel::CStaticNomalModel(const XMFLOAT4X4 & world,
	const Material & matrial,
	const int & indexCount,
	const int & vertexOffset,
	const int & indexOffset,
	ID3D11ShaderResourceView * texSRV,
	ID3D11ShaderResourceView * texNormalSRV,
	const char* name)
	: mObjName(name), mWorld(world), mObjMatrial(matrial), mIndexOffset(indexOffset), mVertexOffset(vertexOffset),
	mIndexCount(indexCount), mTexSRV(texSRV),mTexNormalSRV(texNormalSRV)
{

}


CStaticNomalModel::~CStaticNomalModel()
{
}

void CStaticNomalModel::Draw(ID3D11DeviceContext* dc,
	ID3DX11EffectTechnique* tech,const XMMATRIX& shadowTransform,const Camera& cam)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	D3DX11_TECHNIQUE_DESC techDesc;


		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			// Draw the grid.
			world = XMLoadFloat4x4(&mWorld);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*cam.View()*cam.Proj();

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixIdentity());
			Effects::NormalMapFX->SetMaterial(mObjMatrial);
			Effects::NormalMapFX->SetDiffuseMap(mTexSRV);
			Effects::NormalMapFX->SetNormalMap(mTexNormalSRV);

			tech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
		}
	
}

CStaticBasicModel::CStaticBasicModel(const XMFLOAT4X4 & world, const Material & matrial, const int & indexCount, const int & vertexOffset, const int & indexOffset, ID3D11ShaderResourceView * texSRV, const char * name)
	: mObjName(name), mWorld(world), mObjMatrial(matrial), mIndexOffset(indexOffset), mVertexOffset(vertexOffset),
	mIndexCount(indexCount), mTexSRV(texSRV)
{

}

CStaticBasicModel::~CStaticBasicModel()
{
}

void CStaticBasicModel::Draw(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&mWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*cam.View()*cam.Proj();

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		//Effects::BasicFX->SetWorldViewProjTex(worldViewProj);
		Effects::BasicFX->SetMaterial(mObjMatrial);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetDiffuseMap(mTexSRV);

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount,mIndexOffset, mVertexOffset);
	}

}

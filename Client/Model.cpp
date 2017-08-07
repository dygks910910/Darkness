#include "Model.h"




CStaticNomalModel::CStaticNomalModel(const XMFLOAT4X4 & world,
	const Material & matrial,
	const int & indexCount,
	const int & vertexOffset,
	const int & indexOffset,
	ID3D11ShaderResourceView * texSRV,
	ID3D11ShaderResourceView * texNormalSRV,
	const char* name)
	: CStaticBasicModel(world,matrial,indexCount,vertexOffset,indexOffset,texSRV,name), mTexNormalSRV(texNormalSRV)
{
}


CStaticNomalModel::~CStaticNomalModel()
{

}

void CStaticNomalModel::Draw(ID3D11DeviceContext* dc,
	ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
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
		Effects::NormalMapFX->SetShadowTransform(world*XMLoadFloat4x4(&shadowTransform));
		if (mObjName == "grid")
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(40, 40,0));
		else
			Effects::NormalMapFX->SetTexTransform(XMMatrixIdentity());

		Effects::NormalMapFX->SetMaterial(mObjMatrial);
		Effects::NormalMapFX->SetDiffuseMap(mTexSRV);
		Effects::NormalMapFX->SetNormalMap(mTexNormalSRV);

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
	}

}

void CStaticNomalModel::DrawSsao(ID3D11DeviceContext* dc,
	ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam)
{
	XMMATRIX world;
	XMMATRIX worldView;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldInvTransposeView;
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
		worldView = world*cam.View();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldInvTransposeView = worldInvTranspose*cam.View();

		worldViewProj = world*cam.View()*cam.Proj();

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
	}

}

void CStaticBasicModel::DrawSsao(ID3D11DeviceContext* dc,
	ID3DX11EffectTechnique* tech, const XMMATRIX& shadowTransform, const Camera& cam)
{
	XMMATRIX world;
	XMMATRIX worldView;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldInvTransposeView;
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
		worldView = world*cam.View();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldInvTransposeView = worldInvTranspose*cam.View();

		worldViewProj = world*cam.View()*cam.Proj();

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
	}

}
void CStaticNomalModel::DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj)
{
	CStaticBasicModel::DrawToShadowMap(dc, tech, lightView, lightProj);
}


CStaticBasicModel::CStaticBasicModel(const XMFLOAT4X4 & world, const Material & matrial, const int & indexCount, const int & vertexOffset, const int & indexOffset, ID3D11ShaderResourceView * texSRV, const char * name)
	: mObjName(name), mWorld(world), mObjMatrial(matrial), mIndexOffset(indexOffset), mVertexOffset(vertexOffset),
	mIndexCount(indexCount), mTexSRV(texSRV)
{

}

CStaticBasicModel::CStaticBasicModel(const XMFLOAT4X4 & world, const Material & matrial, const int & indexCount, const int & vertexOffset, const int & indexOffset, ID3D11ShaderResourceView * texSRV, const char * name, const XNA::OrientedBox& box,
	ID3D11Device* device)
	: mObjName(name), mWorld(world), mObjMatrial(matrial), mIndexOffset(indexOffset), mVertexOffset(vertexOffset),
	mIndexCount(indexCount), mTexSRV(texSRV),mBox(box)
{

	Vertex::Basic32 vertices[8];
	vertices[0].Pos = XMFLOAT3(-mBox.Extents.x +mBox.Center.x, 
		-mBox.Extents.y+mBox.Center.y,
		-mBox.Extents.z + mBox.Center.z);
	vertices[1].Pos = XMFLOAT3(-mBox.Extents.x + mBox.Center.x,
		+mBox.Extents.y + mBox.Center.y,
		-mBox.Extents.z + mBox.Center.z);
	vertices[2].Pos = XMFLOAT3(+mBox.Extents.x + mBox.Center.x,
		+mBox.Extents.y + mBox.Center.y,
		-mBox.Extents.z + mBox.Center.z);
	vertices[3].Pos = XMFLOAT3(+mBox.Extents.x + mBox.Center.x,
		-mBox.Extents.y + mBox.Center.y,
		-mBox.Extents.z + mBox.Center.z);

	vertices[4].Pos = XMFLOAT3(-mBox.Extents.x + mBox.Center.x,
		-mBox.Extents.y + mBox.Center.y,
		+mBox.Extents.z + mBox.Center.z);
	vertices[5].Pos = XMFLOAT3(-mBox.Extents.x + mBox.Center.x,
		+mBox.Extents.y + mBox.Center.y,
		+mBox.Extents.z + mBox.Center.z);
	vertices[6].Pos = XMFLOAT3(+mBox.Extents.x + mBox.Center.x,
		+mBox.Extents.y + mBox.Center.y,
		+mBox.Extents.z + mBox.Center.z);
	vertices[7].Pos = XMFLOAT3(+mBox.Extents.x + mBox.Center.x,
		-mBox.Extents.y+ mBox.Center.y,
		+mBox.Extents.z + mBox.Center.z);


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	

	
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) *36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &mBoxIB));

}

CStaticBasicModel::~CStaticBasicModel()
{
}

void CStaticBasicModel::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
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
		
		Effects::BasicFX->SetDiffuseMap(mTexSRV);
		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::BasicFX->SetMaterial(mObjMatrial);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetShadowTransform(world*XMLoadFloat4x4(&shadowTransform));
		
		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
	}

}


void CStaticBasicModel::DrawBox(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const Camera& cam)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	
	UINT stride = sizeof(Vertex::Basic32);
	UINT  offset = 0;
	dc->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	dc->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);
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

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(36, 0, 0);
	}
}

void CStaticBasicModel::DrawToShadowMap(ID3D11DeviceContext* dc,
	ID3DX11EffectTechnique* tech, const XMFLOAT4X4& lightView, const XMFLOAT4X4& lightProj)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = XMLoadFloat4x4(&lightView);
	XMMATRIX proj = XMLoadFloat4x4(&lightProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&mWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);
	}
}

CInstanceBasicModel::CInstanceBasicModel(const Material& matrial,
	const int& indexCount,
	const int& vertexOffset,
	const int& indexOffset,
	ID3D11ShaderResourceView* texSRV,
	const char* name,
	ID3D11Device* device)
	:mMatrial(matrial),mIndexCount(indexCount),mVertexOffset(vertexOffset),
	mTexSRV(texSRV),mName(name)
{

	BuildInstanceBuffer(device);
}

CInstanceBasicModel::CInstanceBasicModel()
{
}

CInstanceBasicModel::~CInstanceBasicModel()
{
}

void CInstanceBasicModel::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
{
	XMMATRIX view = cam.View();
	XMMATRIX proj = cam.Proj();
	XMMATRIX viewProj = cam.ViewProj();

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
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::InstancedBasicFX->SetDiffuseMap(mTexSRV);
		Effects::InstancedBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::InstancedBasicFX->SetWorld(world);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetMaterial(mMatrial);

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexedInstanced(mIndexCount,mInstanceWorld.size(),
			mIndexOffset,mVertexOffset, 0);
	}
}

void CInstanceBasicModel::DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = XMLoadFloat4x4(&lightView);
	XMMATRIX proj = XMLoadFloat4x4(&lightProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMMatrixIdentity();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexedInstanced(mIndexCount,mInstanceWorld.size(),mIndexOffset,mVertexOffset,0);
	}
}

void CInstanceBasicModel::BuildInstanceBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC basicvbd;
	basicvbd.Usage = D3D11_USAGE_IMMUTABLE;
	basicvbd.ByteWidth = sizeof(XMFLOAT4X4) * mInstanceWorld.size();
	basicvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	basicvbd.CPUAccessFlags = 0;
	basicvbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA basicvinitData;
	basicvinitData.pSysMem = &mInstanceWorld[0];
	HR(device->CreateBuffer(&basicvbd, &basicvinitData, &mInstanceBuffer));
}

void CInstanceBasicModel::SetDrawInfomation(const int & indexCount, const int & vertexOffset, const int & indexOffset)
{
	mIndexCount = indexCount;
	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;
}

void CInstanceBasicModel::Destroy()
{
	ReleaseCOM(mInstanceBuffer);
}

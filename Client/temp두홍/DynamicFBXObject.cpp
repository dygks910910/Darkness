#include "DynamicFBXObject.h"



void CDynamicFBXObject::SetWorld(const XMFLOAT4X4 & worldmtx)
{
	mObjWorld = worldmtx;
#if defined(DEBUG) || defined(_DEBUG)  
	mCoord.SetWorld(worldmtx);
#endif
}

void CDynamicFBXObject::Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr)
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
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(Vertex::Basic32) * vb.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vb[0];
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;
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
	for (int i = 0; i < 6; ++i)
		mAnimLimitCount[i] = mModel->mAnimCount[i];
	mObjMapSRV = textureMgr->CreateTexture(mDiffuseMapName);

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

void CDynamicFBXObject::Draw(ID3D11DeviceContext * dc, Camera mCam)
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
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
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

void CDynamicFBXObject::DrawToShadowMap(ID3D11DeviceContext * dc, Camera mCam)
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
float timeDelta;

void CDynamicFBXObject::Update(ID3D11DeviceContext * dc, int num)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	ID3D11Buffer* vb = mModel->GetVB();

	/*static std::map<std::string, std::vector<Vertex::Basic32>*>::iterator AnimIter = mResultAnimData.begin();

	if (mAnimationState == AnimState::crawl)
	{
	AnimIter = mResultAnimData.find("crawl");
	mAnimationLimtCount = mAnimationStateNum[AnimState::crawl];
	}*/
	if (mAnimationCount + 1 >= mAnimLimitCount[num])
		mAnimationCount = 0;
	//Sleep(30);

	static float timeElapsed = 0.0f;
	static DWORD lastTime = timeGetTime();


	DWORD curTime = timeGetTime();


	timeDelta = (curTime - lastTime)*0.001f;

	timeElapsed += timeDelta;

	if (timeElapsed >= (0.03f * (float)1))
	{
		result = dc->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		// Get a pointer to the data in the vertex buffer.
		//verticesPtr = (std::vector<Vertex::Basic32>*)mappedResource.pData;

		// Copy the data into the vertex buffer.

		memcpy(mappedResource.pData, &mModel->mAnimData[mAnimName[num]][mAnimationCount][0], sizeof(Vertex::Basic32) * mModel->mAnimData[mAnimName[num]][mAnimationCount].size());
		// Unlock the vertex buffer.
		dc->Unmap(vb, 0);
		timeElapsed = 0.0f;
		mAnimationCount++;
	}
	lastTime = curTime;
	//ReleaseCOM(mBuildingVB);
	//D3D11_BUFFER_DESC vbd;
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = sizeof(Vertex::Basic32) * AnimIter->second[mAnimationCount].size();
	////vbd.ByteWidth = sizeof(Vertex::Basic32) * vbtest[mAnimationCount].size();
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.CPUAccessFlags = 0;
	//vbd.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA vinitData;
	//vinitData.pSysMem = &AnimIter->second[mAnimationCount][0];
	////vinitData.pSysMem = &vbtest[mAnimationCount][0];
	//HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBuildingVB));
	////ttt++;
}


CDynamicFBXObject::CDynamicFBXObject()
{
}


CDynamicFBXObject::~CDynamicFBXObject()
{
	mFilename.clear();
	mDiffuseMapName.clear();
}

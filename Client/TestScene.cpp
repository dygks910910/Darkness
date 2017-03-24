#include "TestScene.h"


CTestScene::CTestScene()
{
}


CTestScene::~CTestScene()
{
	// 	for (auto p = mvDynamicObject.begin(); p != mvDynamicObject.end();)
	// 	{
	// 		delete *p;
	// 		p=mvDynamicObject.erase(p);
	// 	}
	// 	for (auto p = mvStaticObject.begin(); p != mvStaticObject.end();)
	// 	{
	// 		delete *p;
	// 		p = mvStaticObject.erase(p);
	// 	}
	SafeDelete(mSky);
	// 	SafeDelete(mSsao);
	SafeDelete(mSmap);
	ReleaseCOM(mStaticNormalMappingObjectVB);
	ReleaseCOM(mStaticNormalMappingObjectIB);
	ReleaseCOM(mStaticBasicObjectIB);
	ReleaseCOM(mStaticBasicObjectVB);
}

bool CTestScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain,
	const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight)
{
	mSmap = new ShadowMap(device, 800, 600);
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	//mRenderTargetView = renderTargetView;
	//mDSV = dsv;
	mClientHeight = clientHeight;
	mClientWidth = clientWidth;
	//////////////////////////////////////////////////////////////////////////
	//월드세팅


	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mHouseWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));
	mCam.SetPosition(0.0f, 2.0f, -15.0f);

	XMMATRIX clownScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	XMMATRIX clownRotation = XMMatrixRotationRollPitchYaw(-90, -180, 0);
	XMMATRIX clownOffset = XMMatrixTranslation(0.0f, 5, 0.0f);
	XMStoreFloat4x4(&mClownWorld, clownScale*clownRotation*clownOffset);


	//////////////////////////////////////////////////////////////////////////
	//재질,텍스처불러오기.
	mTexMgr.Init(mDevice);

	mGridMat.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mGridMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mBoxMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mBoxMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

// 	mStoneTexSRV = mTexMgr.CreateTexture(L"Textures/floor.dds");
// 	mBrickTexSRV = mTexMgr.CreateTexture(L"Textures/bricks.dds");
// 	mStoneNormalTexSRV = mTexMgr.CreateTexture(L"Textures/floor_nmap.dds");
// 	mBrickNormalTexSRV = mTexMgr.CreateTexture(L"Textures/bricks_nmap.dds");
// 
// 	mClownNormalTexSRV = mTexMgr.CreateTexture(L"true_clown_normals.png");
// 	mClownTexSRV = mTexMgr.CreateTexture(L"true_clown_diffuse1.png");

	//버퍼 빌드
	BuildShapeGeometryBuffers();
	//////////////////////////////////////////////////////////////////////////
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(10.0f*10.0f + 15.0f*15.0f);

	//mTextureMgr.Init(device);
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	mDirLights[0].Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.47735f, -0.47735f, -0.57735f);




	mSky = new Sky(device, L"Textures/desertcube1024.dds", 5000.0f);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	//월드좌표계 그려주기.
	XMFLOAT4X4 temp4x4;
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device, temp4x4, 5000);

	return true;
}

void CTestScene::UpdateScene(const float & dt)
{
	// 	mTimer.Tick();
		//
		// Control the camera.
		//
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(20.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-20.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-20.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(20.0f*dt);

	//
	// Walk/fly mode
	//

// 	if (GetAsyncKeyState('R') & 0x8000)
	{
		// 		mFire.Reset();
		// 		mRain.Reset();
	}
	// 
	// Clamp camera to terrain surface in walk mode.
	//

	//
	// Reset particle systems.
	//
	BuildShadowTransform();
	mCam.UpdateViewMatrix();
}

void CTestScene::Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{

	mSmap->BindDsvAndSetNullRenderTarget(mDc);
	DrawSceneToShadowMap();
	mDc->RSSetState(0);

	
	mDc->RSSetState(0);

	ID3D11RenderTargetView* renderTargets[1] = { rtv };

	//std::cout << mRenderTargetView << std::endl;

	mDc->OMSetRenderTargets(1, renderTargets, dsv);
	mDc->RSSetViewports(1, viewPort);

	mDc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));
	mDc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mCam.GetPosition());
	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::NormalMapFX->SetShadowMap(mSmap->DepthMapSRV());

	// Figure out which technique to use for different geometry.
	ID3DX11EffectTechnique* activeTech = Effects::NormalMapFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSphereTech = Effects::BasicFX->Light3ReflectTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light3TexTech;
	// 	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;

	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	mDc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	mDc->IASetVertexBuffers(0, 1, &mStaticNormalMappingObjectVB, &stride, &offset);
	mDc->IASetIndexBuffer(mStaticNormalMappingObjectIB, DXGI_FORMAT_R32_UINT, 0);

	if (GetAsyncKeyState('1') & 0x8000)
		mDc->RSSetState(RenderStates::WireframeRS);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (auto p : mStaticNormalModels)
	{
		p.Draw(mDc, activeTech, shadowTransform, mCam);
	}


	stride = sizeof(Vertex::Basic32);
	offset = 0;

	mDc->IASetInputLayout(InputLayouts::Basic32);
	mDc->IASetVertexBuffers(0, 1, &mStaticBasicObjectVB, &stride, &offset);
	mDc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
	for (auto p : mStaticBasicModels)
	{
		p.Draw(mDc, activeSkullTech, shadowTransform, mCam);
	}


	mCordWorld.Draw(mDc, mCam);
	//float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	

	mDc->RSSetState(0);
	mSky->Draw(mDc, mCam);
	//불 파티클.
	mDc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default



	// restore default states.
	mDc->RSSetState(0);
	mDc->OMSetDepthStencilState(0, 0);
	mDc->OMSetBlendState(0, blendFactor, 0xffffffff);


	HR(mSwapChain->Present(0, 0));
}
void CTestScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CTestScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CTestScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CTestScene::OnResize(const float& aspectRatio)
{
	mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 0.3f, 3000.0f);
	//XNA::ComputeFrustumFromProjection(&mCamFrustum, &mCam.Proj());
	// 	if (mSsao)
	// 	{
	// 		mSsao->OnSize(mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());
	// 	}
}

void CTestScene::DrawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mCam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* animatedSmapTech = Effects::BuildShadowMapFX->BuildShadowMapSkinnedTech;

	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;


	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	mDc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	mDc->IASetVertexBuffers(0, 1, &mStaticNormalMappingObjectVB, &stride, &offset);
	mDc->IASetIndexBuffer(mStaticNormalMappingObjectIB, DXGI_FORMAT_R32_UINT, 0);

	if (GetAsyncKeyState('1') & 0x8000)
		mDc->RSSetState(RenderStates::WireframeRS);

	D3DX11_TECHNIQUE_DESC techDesc;
	smapTech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	// Draw the grid.
	//	world = XMLoadFloat4x4(&mGridWorld);
	//	worldInvTranspose = MathHelper::InverseTranspose(world);
	//	worldViewProj = world*view*proj;

	//	Effects::BuildShadowMapFX->SetWorld(world);
	//	Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
	//	Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
	//	Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

	//	smapTech->GetPassByIndex(p)->Apply(0, mDc);
	//	mDc->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	//	// Draw the box.
	//	world = XMLoadFloat4x4(&mBoxWorld);
	//	worldInvTranspose = MathHelper::InverseTranspose(world);
	//	worldViewProj = world*view*proj;

	//	Effects::BuildShadowMapFX->SetWorld(world);
	//	Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
	//	Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
	//	Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

	//	smapTech->GetPassByIndex(p)->Apply(0, mDc);
	//	mDc->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	//}
}
void CTestScene::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void CTestScene::BuildShapeGeometryBuffers()
{

	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData clown;





	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(15.0f, 15.0f, 11, 11, grid);
	CFbxLoader loader;

	loader.LoadFBX("true_clownTri.FBX", clown);
	loader.Destroy();



	boxVertexOffset = 0;
	gridVertexOffset = box.Vertices.size();
	clownVertexOffset = gridVertexOffset + grid.Vertices.size();
	


	boxIndexCount = box.Indices.size();
	gridIndexCount = grid.Indices.size();
	clownIndexCount = clown.Indices.size();
	



	boxIndexOffset = 0;
	gridIndexOffset = boxIndexCount;
	clownIndexOffset = gridIndexOffset + gridIndexCount;



	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		clown.Vertices.size();

	UINT totalIndexCount =
		boxIndexCount +
		gridIndexCount +
		clownIndexCount;

	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			box.Vertices[i].TangentU.x,
			box.Vertices[i].TangentU.y,
			box.Vertices[i].TangentU.z,
			1.0f);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex = grid.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			grid.Vertices[i].TangentU.x,
			grid.Vertices[i].TangentU.y,
			grid.Vertices[i].TangentU.z,
			1.0f);
	}

	for (size_t i = 0; i < clown.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = clown.Vertices[i].Position;
		vertices[k].Normal = clown.Vertices[i].Normal;
		vertices[k].Tex = clown.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			clown.Vertices[i].TangentU.x,
			clown.Vertices[i].TangentU.y,
			clown.Vertices[i].TangentU.z,
			1.0f);
	}

	

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(mDevice->CreateBuffer(&vbd, &vinitData, &mStaticNormalMappingObjectVB));

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), clown.Indices.begin(), clown.Indices.end());
	

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(mDevice->CreateBuffer(&ibd, &iinitData, &mStaticNormalMappingObjectIB));


	//////////////////////////////////////////////////////////////////////////
	//basic데이터 추출
	BuildBasicGeometryBuffer();

	//////////////////////////////////////////////////////////////////////////
	//맵 데이터 읽기!!!!

	std::ifstream ifs;
	ifs.open("MapData.txt");
	XMFLOAT3 scale;
	XMFLOAT4 rotation;
	XMFLOAT3 position;


	char objectName[50];
	char cIgnore[100];
	int StaticObjCount;
	ifs >> cIgnore >> StaticObjCount;

	for (int i = 0; i < StaticObjCount; ++i)
	{
		ifs >> objectName >> cIgnore >> cIgnore >> cIgnore >> cIgnore >> position.x >> position.y >> position.z >> cIgnore;
		ifs >> cIgnore >> cIgnore >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w >> cIgnore;
		ifs >> cIgnore >> cIgnore >> scale.x >> scale.y >> scale.z >> cIgnore;


		if (!strcmp(objectName, "Cube"))
		{

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

			mStaticNormalModels.push_back(CStaticNomalModel(M,
				mBoxMat,
				boxIndexCount,
				boxVertexOffset,
				boxIndexOffset,
				mTexMgr.CreateTexture(L"Textures\\bricks.dds"),
				mTexMgr.CreateTexture(L"Textures\\bricks_nmap.dds"),
				"box"
			));

		}
		else if (!strcmp(objectName, "Clown"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticNormalModels.push_back(CStaticNomalModel(M,
				mBoxMat,
				clownIndexCount,
				clownVertexOffset,
				clownIndexOffset,
				mTexMgr.CreateTexture(L"true_clown_diffuse1.png"),
				mTexMgr.CreateTexture(L"true_clown_normals.png"),
				"clown"
			));

		}
		else if (!strcmp(objectName, "MainCamera"))
		{
			mCam.SetPosition(position.x, position.y, position.z);
		}
		else if (!strcmp(objectName, "Plane"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticNormalModels.push_back(CStaticNomalModel(M,
				mGridMat,
				gridIndexCount,
				gridVertexOffset,
				gridIndexOffset,
				mTexMgr.CreateTexture(L"Textures\\floor.dds"),
				mTexMgr.CreateTexture(L"Textures\\floor_nmap.dds"),
				"grid"
			));


		}
		else if (!strcmp(objectName, "fence1"))
		{
			// 			temppFBXObject = new CFBXObject;
			// 			temppFBXObject->SetFilename("fence1.FBX");
			// 			temppFBXObject->SetDiffuseFileName(L"diff_fence_gate.dds");
			// 
			// 			XMVECTOR S = XMLoadFloat3(&scale);
			// 			XMVECTOR P = XMLoadFloat3(&position);
			// 			XMVECTOR Q = XMLoadFloat4(&rotation);
			// 			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			// 
			// 			XMFLOAT4X4 M;
			// 			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			// 			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			// 			temppFBXObject->SetWorld(M);
			// 
			// 			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_1"))
		{
			// 			temppFBXObject = new CFBXObject;
			// 			temppFBXObject->SetFilename("house 1.fbx");
			// 			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house1IndexCount,
				house1VertexOffset,
				house1IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house1"
			));
		}
		else if (!strcmp(objectName, "house_2"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house2IndexCount,
				house2VertexOffset,
				house2IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house2"
			));
		}
		else if (!strcmp(objectName, "house_3"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house3IndexCount,
				house3VertexOffset,
				house3IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house3"
			));
		}
		else if (!strcmp(objectName, "house_4"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house4IndexCount,
				house4VertexOffset,
				house4IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house4"
			));
		}
		else if (!strcmp(objectName, "house_5"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			ID3D11ShaderResourceView* tempSRV = mTexMgr.CreateTexture(L"house diffuse.dds");
			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house5IndexCount,
				house5VertexOffset,
				house5IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house5"
			));
		}
		else if (!strcmp(objectName, "house_6"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house6IndexCount,
				house6VertexOffset,
				house6IndexOffset,
				mTexMgr.CreateTexture(L"house diffuse.dds"),
				"house6"
			));
		}

		std::cout << objectName << std::endl;
		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		std::cout << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << std::endl;
		std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
		ZeroMemory(&objectName, sizeof(objectName));
		ZeroMemory(&cIgnore, sizeof(cIgnore));
		ZeroMemory(&position, sizeof(position));
		ZeroMemory(&rotation, sizeof(rotation));
		ZeroMemory(&scale, sizeof(scale));
	}
	ifs.close();


}
void CTestScene::BuildBasicGeometryBuffer()
{
 	GeometryGenerator::MeshData fence, house1, house2, house3, house4, house5, house6;
 	CFbxLoader loader;
 
 	loader.LoadFBX("fence1.FBX", fence);
 	loader.Destroy();
 	loader.LoadFBX("house 1.fbx", house1);
 	loader.Destroy();
 	loader.LoadFBX("house 2.fbx", house2);
 	loader.Destroy();
 	loader.LoadFBX("house 3.fbx", house3);
 	loader.Destroy();
 	loader.LoadFBX("house 4.fbx", house4);
 	loader.Destroy();
 	loader.LoadFBX("house 5.fbx", house5);
 	loader.Destroy();
 	loader.LoadFBX("house 6.fbx", house6);
 	loader.Destroy();
 
 	 house1VertexOffset = 0;
 	 house2VertexOffset = house1.Vertices.size();
 	 house3VertexOffset = house2VertexOffset + house2.Vertices.size();
 	house4VertexOffset = house3VertexOffset + house3.Vertices.size();
 	house5VertexOffset = house4VertexOffset + house4.Vertices.size();
 	house6VertexOffset = house5VertexOffset + house5.Vertices.size();
 	fenceVertexOffset = house6VertexOffset + house6.Vertices.size();
 
 	house1IndexCount = house1.Indices.size();
 	house2IndexCount = house2.Indices.size();
 	house3IndexCount = house3.Indices.size();
 	house4IndexCount = house4.Indices.size();
 	house5IndexCount = house5.Indices.size();
 	house6IndexCount = house6.Indices.size();
 	fenceIndexCount = fence.Indices.size();
 
 	house1IndexOffset = 0;
 	house2IndexOffset = house1IndexCount;
 	house3IndexOffset = house2IndexOffset + house2IndexCount;
 	house4IndexOffset = house3IndexOffset + house3IndexCount;
 	house5IndexOffset = house4IndexOffset + house4IndexCount;
 	house6IndexOffset = house5IndexOffset + house5IndexCount;
 	fenceIndexOffset = house6IndexOffset + house6IndexCount;
 
 	UINT totalVertexCount =
 		house1.Vertices.size() +
 		house2.Vertices.size() +
 		house3.Vertices.size() + 
 		house4.Vertices.size() + 
 		house5.Vertices.size() + 
 		house6.Vertices.size()  + 
 		fence	.Vertices.size();
 
 	UINT totalIndexCount =
 		house1IndexCount +
 		house2IndexCount +
 		house3IndexCount + 
 		house4IndexCount +
 		house5IndexCount + 
 		house6IndexCount +
 		fenceIndexCount;
 
 	std::vector<Vertex::Basic32> vertices(totalVertexCount);
 
 	UINT k = 0;
 	for (size_t i = 0; i < house1.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house1.Vertices[i].Position;
 		vertices[k].Normal = house1.Vertices[i].Normal;
 		vertices[k].Tex = house1.Vertices[i].TexC;
 	
 	}
 	for (size_t i = 0; i < house2.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house2.Vertices[i].Position;
 		vertices[k].Normal = house2.Vertices[i].Normal;
 		vertices[k].Tex = house2.Vertices[i].TexC;
 
 	}
 	for (size_t i = 0; i < house3.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house3.Vertices[i].Position;
 		vertices[k].Normal = house3.Vertices[i].Normal;
 		vertices[k].Tex = house3.Vertices[i].TexC;
 
 	}
 	for (size_t i = 0; i < house4.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house4.Vertices[i].Position;
 		vertices[k].Normal = house4.Vertices[i].Normal;
 		vertices[k].Tex = house4.Vertices[i].TexC;
 
 	}
 	for (size_t i = 0; i < house5.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house5.Vertices[i].Position;
 		vertices[k].Normal = house5.Vertices[i].Normal;
 		vertices[k].Tex = house5.Vertices[i].TexC;
 
 	}
 	for (size_t i = 0; i < house6.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = house6.Vertices[i].Position;
 		vertices[k].Normal = house6.Vertices[i].Normal;
 		vertices[k].Tex = house6.Vertices[i].TexC;
 
 	}
 	for (size_t i = 0; i < fence.Vertices.size(); ++i, ++k)
 	{
 		vertices[k].Pos = fence.Vertices[i].Position;
 		vertices[k].Normal = fence.Vertices[i].Normal;
 		vertices[k].Tex = fence.Vertices[i].TexC;
 	}
 
 	D3D11_BUFFER_DESC vbd;
 	vbd.Usage = D3D11_USAGE_IMMUTABLE;
 	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
 	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 	vbd.CPUAccessFlags = 0;
 	vbd.MiscFlags = 0;
 	D3D11_SUBRESOURCE_DATA vinitData;
 	vinitData.pSysMem = &vertices[0];
 	HR(mDevice->CreateBuffer(&vbd, &vinitData, &mStaticBasicObjectVB));
 
 	//
 	// Pack the indices of all the meshes into one index buffer.
 	//
 	std::vector<UINT> indices;
 	indices.insert(indices.end(), house1.Indices.begin(), house1.Indices.end());
 	indices.insert(indices.end(), house2.Indices.begin(), house2.Indices.end());
 	indices.insert(indices.end(), house3.Indices.begin(), house3.Indices.end());
 	indices.insert(indices.end(), house4.Indices.begin(), house4.Indices.end());
 	indices.insert(indices.end(), house5.Indices.begin(), house5.Indices.end());
 	indices.insert(indices.end(), house6.Indices.begin(), house6.Indices.end());
 	indices.insert(indices.end(), fence.Indices.begin(), fence.Indices.end());
 
 
 
 	D3D11_BUFFER_DESC ibd;
 	ibd.Usage = D3D11_USAGE_IMMUTABLE;
 	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
 	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
 	ibd.CPUAccessFlags = 0;
 	ibd.MiscFlags = 0;
 	D3D11_SUBRESOURCE_DATA iinitData;
 	iinitData.pSysMem = &indices[0];
 	HR(mDevice->CreateBuffer(&ibd, &iinitData, &mStaticBasicObjectIB));


}
//
//void CTestScene::ReadMapData(ID3D11Device* device)
//{
//	std::ifstream ifs;
//	ifs.open("MapData.txt");
//	XMFLOAT3 scale;
//	XMFLOAT4 rotation;
//	XMFLOAT3 position;
//
//	CDynamicObject* pTempDynamicObject;
//	CStaticObject* pTempStaticObject;
//	char objectName[50];
//	char cIgnore[100];
//	int StaticObjCount;
//	ifs >> cIgnore >> StaticObjCount;
//	CFBXObject* temppFBXObject;
//
//	for (int i = 0; i < StaticObjCount; ++i)
//	{
//		ifs >> objectName >> cIgnore >> cIgnore >> cIgnore >> cIgnore >> position.x >> position.y >> position.z >> cIgnore;
//		ifs >> cIgnore >> cIgnore >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w >> cIgnore;
//		ifs >> cIgnore >> cIgnore >> scale.x >> scale.y >> scale.z >> cIgnore;
//
//
//		if (!strcmp(objectName, "Cube"))
//		{
//			pTempStaticObject = new CBox;
//			pTempStaticObject->Init(device, &mModelMgr, &mTextureMgr);
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			pTempStaticObject->SetWorld(M);
//			mvStaticObject.push_back(pTempStaticObject);
//		}
//		else if (!strcmp(objectName, "Clown"))
//		{
//			temppFBXObject = new CFBXObject;
		//temppFBXObject->SetFilename("true_clownTri.FBX");
//			temppFBXObject->SetDiffuseFileName(L"true_clown_diffuse1.png");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//
//		}
//		else if (!strcmp(objectName, "MainCamera"))
//		{
//			mCam.SetPosition(position.x, position.y, position.z);
//		}
//		else if (!strcmp(objectName, "Plane"))
//		{
//			pTempStaticObject = new CPlane;
//			pTempStaticObject->Init(device, &mModelMgr, &mTextureMgr);
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			pTempStaticObject->SetWorld(M);
//			mvStaticObject.push_back(pTempStaticObject);
//		}
//		else if (!strcmp(objectName, "Baker_house"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("Baker_houseYup.FBX");
//			temppFBXObject->SetDiffuseFileName(L"Baker_house.png");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//
//		}
//		else if (!strcmp(objectName, "House"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("House.FBX");
//			temppFBXObject->SetDiffuseFileName(L"House.jpg");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "fence1"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("fence1.FBX");
//			temppFBXObject->SetDiffuseFileName(L"diff_fence_gate.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_1"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 1.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_2"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 2.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_3"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 3.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_4"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 4.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_5"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 5.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//		else if (!strcmp(objectName, "house_6"))
//		{
//			temppFBXObject = new CFBXObject;
//			temppFBXObject->SetFilename("house 6.fbx");
//			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");
//
//			XMVECTOR S = XMLoadFloat3(&scale);
//			XMVECTOR P = XMLoadFloat3(&position);
//			XMVECTOR Q = XMLoadFloat4(&rotation);
//			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//			XMFLOAT4X4 M;
//			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
//			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
//			temppFBXObject->SetWorld(M);
//
//			mvStaticObject.push_back(temppFBXObject);
//		}
//
//		std::cout << objectName << std::endl;
//		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
//		std::cout << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << std::endl;
//		std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
//		ZeroMemory(&objectName, sizeof(objectName));
//		ZeroMemory(&cIgnore, sizeof(cIgnore));
//		ZeroMemory(&position, sizeof(position));
//		ZeroMemory(&rotation, sizeof(rotation));
//		ZeroMemory(&scale, sizeof(scale));
//	}
//	ifs.close();
//}



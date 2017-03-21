#include "TestScene.h"


CTestScene::CTestScene()
{
}


CTestScene::~CTestScene()
{
	for (auto p = mvDynamicObject.begin(); p != mvDynamicObject.end();)
	{
		delete *p;
		p=mvDynamicObject.erase(p);
	}
	for (auto p = mvStaticObject.begin(); p != mvStaticObject.end();)
	{
		delete *p;
		p = mvStaticObject.erase(p);
	}
	SafeDelete(mSky);
// 	SafeDelete(mSsao);
// 	SafeDelete(mSmap);
}

bool CTestScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc, 
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	const D3D11_VIEWPORT& viewPort)
{
// 	mSmap = new ShadowMap(device, 800,600);

	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(10.0f*10.0f + 15.0f*15.0f);

	mTextureMgr.Init(device);
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
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

	mSky = new Sky(device, L"Textures/sunsetcube1024.dds", 5000.0f);
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	//������ǥ�� �׷��ֱ�.
	XMFLOAT4X4 temp4x4;
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device, temp4x4, 5000);
	/*mSsao = new Ssao(device, dc, 800, 600, mCam.GetFovY(), mCam.GetFarZ());*/
	mScreenViewport = viewPort;
	std::ifstream ifs;
	ifs.open("MapData.txt");
	XMFLOAT3 scale;
	XMFLOAT4 rotation;
	XMFLOAT3 position;

	CDynamicObject* pTempDynamicObject;
	CStaticObject* pTempStaticObject;
	char objectName[50];
	char cIgnore[100];
	int StaticObjCount;
	ifs >> cIgnore >> StaticObjCount;
	CFBXObject* temppFBXObject;

	for (int i = 0; i < StaticObjCount; ++i)
	{
		ifs >> objectName >> cIgnore>>  cIgnore >> cIgnore >> cIgnore >> position.x >> position.y>> position.z	>> cIgnore;
		ifs >> cIgnore >> cIgnore >> cIgnore >> rotation.x >> rotation.y>> rotation.z >> rotation.w >> cIgnore;
		ifs >> cIgnore >> cIgnore >> scale.x	 >> scale.y >> scale.z >> cIgnore;


		if (!strcmp(objectName, "Cube"))
		{
			pTempStaticObject = new CBox;
			pTempStaticObject->Init(device, &mModelMgr, &mTextureMgr);
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			pTempStaticObject->SetWorld(M);
			mvStaticObject.push_back(pTempStaticObject);
		}
		else if (!strcmp(objectName, "Clown"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("true_clownTri.FBX");
			temppFBXObject->SetDiffuseFileName(L"true_clown_diffuse1.png");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);

		}
		else if (!strcmp(objectName, "MainCamera"))
		{
			mCam.SetPosition(position.x, position.y, position.z);
		}
		else if (!strcmp(objectName, "Plane"))
		{
			pTempStaticObject = new CPlane;
			pTempStaticObject->Init(device, &mModelMgr, &mTextureMgr);
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			pTempStaticObject->SetWorld(M);
			mvStaticObject.push_back(pTempStaticObject);
		}
		else if (!strcmp(objectName, "Baker_house"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("Baker_houseYup.FBX");
			temppFBXObject->SetDiffuseFileName(L"Baker_house.png");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);

		}
		else if (!strcmp(objectName, "House"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("House.FBX");
			temppFBXObject->SetDiffuseFileName(L"House.jpg");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "fence1"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("fence1.FBX");
			temppFBXObject->SetDiffuseFileName(L"diff_fence_gate.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName , "house_1"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 1.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_2"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 2.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_3"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 3.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_4"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 4.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_5"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 5.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		else if (!strcmp(objectName, "house_6"))
		{
			temppFBXObject = new CFBXObject;
			temppFBXObject->SetFilename("house 6.fbx");
			temppFBXObject->SetDiffuseFileName(L"house diffuse.dds");

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			temppFBXObject->Init(device, &mModelMgr, &mTextureMgr);
			temppFBXObject->SetWorld(M);

			mvStaticObject.push_back(temppFBXObject);
		}
		
		std::cout << objectName << std::endl;
		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		std::cout << rotation.x << " " << rotation.y << " " << rotation.z<< " " << rotation.w<< std::endl;
		std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
		ZeroMemory(&objectName, sizeof(objectName));
		ZeroMemory(&cIgnore, sizeof(cIgnore));
		ZeroMemory(&position, sizeof(position));
		ZeroMemory(&rotation	, sizeof(rotation));
		ZeroMemory(&scale, sizeof(scale));
	}
	ifs.close();
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device,temp4x4, 5000);
	mModelMgr.PrintModelCount();
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

	mCam.UpdateViewMatrix();
}

void CTestScene::Draw(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView,
	ID3D11DepthStencilView * depthStencilView)
{

// 	mSmap->BindDsvAndSetNullRenderTarget(dc);
// 	DrawSceneToShadowMap(device, dc, swapChain, renderTargetView, depthStencilView);
// 	dc->RSSetState(0);

	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
// 	mSsao->SetNormalDepthRenderTarget(depthStencilView);
// 	DrawSceneToSsaoNormalDepthMap(device,dc,swapChain,
// 		renderTargetView,depthStencilView);

	//
	// Now compute the ambient occlusion.
	//

// 	mSsao->ComputeSsao(mCam);
// 	mSsao->BlurAmbientMap(2);

	ID3D11RenderTargetView* renderTargets[1] = { renderTargetView };
	dc->OMSetRenderTargets(1, renderTargets, depthStencilView);
	dc->RSSetViewports(1, &mScreenViewport);
	dc->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));


	/*dc->OMSetDepthStencilState(RenderStates::EqualsDSS, 0);*/

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
// 	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());
// 	Effects::BasicFX->SetSsaoMap(mSsao->AmbientSRV());

// 	Effects::NormalMapFX->SetDirLights(mDirLights);
// 	Effects::NormalMapFX->SetEyePosW(mCam.GetPosition());
// 	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());
// 	Effects::NormalMapFX->SetShadowMap(mSmap->DepthMapSRV());
// 	Effects::NormalMapFX->SetSsaoMap(mSsao->AmbientSRV());
	for (auto p : mvDynamicObject)
	{
		p->Draw(dc, mCam);
	}
	for (auto p : mvStaticObject)
	{
		p->Draw(dc, mCam);
	}

	mCordWorld.Draw(dc, mCam);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	
	dc->RSSetState(0);
	mSky->Draw(dc, mCam);
	//�� ��ƼŬ.
	dc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	


	// restore default states.
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);


	HR(swapChain->Present(0, 0));
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
	XNA::ComputeFrustumFromProjection(&mCamFrustum, &mCam.Proj());
// 	if (mSsao)
// 	{
// 		mSsao->OnSize(800, 600, mCam.GetFovY(), mCam.GetFarZ());
// 	}
}

void CTestScene::DrawSceneToShadowMap(ID3D11Device * device, ID3D11DeviceContext * dc, IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView, ID3D11DepthStencilView * depthStencilView)
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

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto p : mvStaticObject)
	{
		p->DrawToShadowMap(dc, mCam);
	}

	dc->RSSetState(0);
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

void CTestScene::DrawSceneToSsaoNormalDepthMap(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* depthStencilView)
{
	for (auto p : mvStaticObject)
	{
		p->DrawToSsaoNormalDepthMap(dc, mCam);
	}
	
}

#include "TestScene.h"


CTestScene::CTestScene()
{
}


CTestScene::~CTestScene()
{
	SafeDelete(mSky);
	SafeDelete(mSmap);
	ReleaseCOM(mRainTexSRV);
}

bool CTestScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain,
	const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight)
{
	mSmap = new ShadowMap(device, clientWidth ,clientHeight);
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	mClientHeight = clientHeight;
	mClientWidth = clientWidth;

	

	//////////////////////////////////////////////////////////////////////////
	//월드세팅

	
	mSceneBounds.Center = XMFLOAT3(0, 0, 0);
	mSceneBounds.Radius = 110;
	mLightRotationAngle = 0;


	//////////////////////////////////////////////////////////////////////////
	//재질,텍스처불러오기.
	mTexMgr.Init(mDevice);
	mModelMgr.Init(mTexMgr, mCam, device);
	

		//버퍼 빌드
	//BuildShapeGeometryBuffers();

	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////


	
	//////////////////////////////////////////////////////////////////////////
	//mTextureMgr.Init(device);
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mDirLights[0].Ambient = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	// Shadow acne gets worse as we increase the slope of the polygon (from the
	// perspective of the light).
	//mDirLights[0].Direction = XMFLOAT3(5.0f/sqrtf(50.0f), -5.0f/sqrtf(50.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(125.0f), -5.0f/sqrtf(125.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(116.0f), -4.0f/sqrtf(116.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(109.0f), -3.0f/sqrtf(109.0f), 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);
	
	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;


	mSky = new Sky(device, L"Textures/desertcube1024.dds", 5000.0f);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	
	//월드좌표계 그려주기.
	XMFLOAT4X4 temp4x4;
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device, temp4x4, 5000);


	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(mDevice);
	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(mDevice, mDc, raindrops);
	mRain.Init(mDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);

	mTimer.Start();

	return true;
}

void CTestScene::UpdateScene(const float & dt)
{
	mTimer.Tick();
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
	mRain.Update(dt, mTimer.TotalTime());

	mCam.UpdateViewMatrix();
}

void CTestScene::Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{

	mSmap->BindDsvAndSetNullRenderTarget(mDc);

	DrawSceneToShadowMap();

	mDc->RSSetState(0);


	//
	// Restore the back and depth buffer to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { rtv };
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

	Effects::InstancedBasicFX->SetDirLights(mDirLights);
	Effects::InstancedBasicFX->SetEyePosW(mCam.GetPosition());

	ID3DX11EffectTechnique* activeNormalMappingTech = Effects::NormalMapFX->Light3TexTech;
	ID3DX11EffectTechnique* activeBasicTech = Effects::BasicFX->Light3TexReflectTech;
	ID3DX11EffectTechnique* activeInstanceTech = Effects::InstancedBasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;

	// Figure out which technique to use for different geometry.

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;


	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 


	if (GetAsyncKeyState('1') & 0x8000)
		mDc->RSSetState(RenderStates::WireframeRS);
	D3DX11_TECHNIQUE_DESC techDesc;


	//
	// Draw the spheres with cubemap reflection.
	//
	mModelMgr.DrawStaticBasicModels(mDc, activeBasicTech,
		XMLoadFloat4x4(&mShadowTransform), mCam);
	mModelMgr.DrawStaticNormalModels(mDc, activeNormalMappingTech,
		XMLoadFloat4x4(&mShadowTransform), mCam);
	mModelMgr.DrawInstancedModel(mDc, activeInstanceTech, XMLoadFloat4x4(&mShadowTransform), mCam);

	//////////////////////////////////////////////////////////////////////////
	//draw Animation

	mCordWorld.Draw(mDc, mCam);

	mSky->Draw(mDc, mCam);

	mDc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	mDc->IASetInputLayout(InputLayouts::Particle);
	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	mRain.SetEyePos(mCam.GetPosition());
	mRain.SetEmitPos(mCam.GetPosition());
	mRain.Draw(mDc, mCam);

	// restore default states, as the SkyFX changes them in the effect file.


	mDc->RSSetState(0);
	mDc->OMSetDepthStencilState(0, 0);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mDc->PSSetShaderResources(0, 16, nullSRV);
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

void CTestScene::OnResize()
{
	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 0.01f, 3000.0f);
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

	mModelMgr.DrawToShadowMap(mDc, smapTech, mLightView, mLightProj);
	
	

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

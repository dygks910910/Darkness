#include "SceneMainGame.h"



CSceneMainGame::CSceneMainGame():mSky(0)/*, mRandomTexSRV(0), mFlareTexSRV(0), mRainTexSRV(0)*/, mWalkCamMode(false)
{
}

CSceneMainGame::~CSceneMainGame()
{
	/*ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);
	ReleaseCOM(mRainTexSRV);*/
	SafeDelete(mSky);
	ReleaseCOM(mDepthStencilState);
	ReleaseCOM(mDepthDisableState);
	mMiniMap.Shutdown();
}


bool CSceneMainGame::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView, 
	D3D11_VIEWPORT* viewPort)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	mCam.SetPosition(0.0f, 2.0f, 100.0f);
	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.407f, -0.407f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.47735f, -0.47735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.47735f, -0.47735f, -0.57735f);

	//draw를 위한 세팅.
	mBox.Init(device);
	XMMATRIX boxOffset = XMMatrixIdentity();
	//mBuilding.Init(device);
	mSky = new Sky(device, L"Textures/sunsetcube1024.dds", 5000.0f);

	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	mTerrain.Init(device, dc, tii);

	//mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(device);

// 	std::vector<std::wstring> flares;
// 	flares.push_back(L"Textures\\flare0.dds");
	//mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, flares);

	//mFire.Init(device, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	//mFire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	//std::vector<std::wstring> raindrops;
	//raindrops.push_back(L"Textures\\raindrop.dds");
	//mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, raindrops);

	//mRain.Init(device, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);


	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	///*
	//2017 / 1 / 13 / 4:11
	//작성자:박요한(dygks910910@daum.net)
	//설명:미니맵을 그리기 위한 작업.
	//*/
	mMinimapViewport.TopLeftX = 700;
	mMinimapViewport.TopLeftY = 500;
	mMinimapViewport.Width = 100;
	mMinimapViewport.Height = 100;
	mMinimapViewport.MaxDepth = 1;
	mMinimapViewport.MinDepth = 0;

	mMainGameViewport = viewPort;

	//D3D11_VIEWPORT viewportArr[2];
	//viewportArr[0] = *mMainGameViewport;
	//viewportArr[1] = mMinimapViewport;
	mCrawler.Init(device);

	//ID3D11Texture2D* backBuffer;
	//HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	//HR(device->CreateRenderTargetView(backBuffer, 0, &mMinimapRTV));
	//ReleaseCOM(backBuffer);
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(device->CreateDepthStencilState(&depthDisabledStencilDesc, &mDepthDisableState));
	depthDisabledStencilDesc.DepthEnable = true;
	
	HR(device->CreateDepthStencilState(&depthDisabledStencilDesc, &mDepthStencilState));
	dc->OMSetDepthStencilState(mDepthStencilState, 1);
	mMiniMap.Initialize(device, 800, 600, mCam.View(), 100, 100);
	return true;
}

void CSceneMainGame::UpdateScene(const float & dt)
{
	mTimer.Tick();
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(10.0f*dt);

	//
	// Walk/fly mode
	//
	if (GetAsyncKeyState('2') & 0x8000)
		mWalkCamMode = true;
	if (GetAsyncKeyState('3') & 0x8000)
		mWalkCamMode = false;

	if (GetAsyncKeyState('R') & 0x8000)
	{
// 		mFire.Reset();
// 		mRain.Reset();
	}
	// 
	// Clamp camera to terrain surface in walk mode.
	//
	if (mWalkCamMode)
	{
		XMFLOAT3 camPos = mCam.GetPosition();
		//float y = mTerrain.GetHeight(camPos.x, camPos.z);

		//mCam.SetPosition(camPos.x, y + 2.0f, camPos.z);
	}

	//
	// Reset particle systems.
	//


// 	mFire.Update(dt, mTimer.TotalTime());
// 	mRain.Update(dt, mTimer.TotalTime());

	mCam.UpdateViewMatrix();
}

void CSceneMainGame::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* depthStencilView)
{

	dc->RSSetViewports(1, mMainGameViewport);
	dc->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	dc->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	
	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//기본입력배치.
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/*
	2017 / 1 / 10 / 7:00
	작성자:박요한(dygks910910@daum.net)
	설명:컬링이필요없는경우사용.ex)와이어펜스.
	*/
	dc->RSSetState(RenderStates::NoCullRS);
	mBox.Draw(dc, mCam);

	/*
	2017 / 1 / 10 / 7:00
	작성자:박요한(dygks910910@daum.net)
	설명:투명도가 없는곳에서 사용.
	*/
	dc->RSSetState(RenderStates::SolidRS);
	mCrawler.Draw(dc,mCam);
	/*mBuilding.Draw(dc, mMinimapCam);*/
	


	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	mTerrain.Draw(dc, mCam, mDirLights);
	
	


	dc->RSSetState(0);

	mSky->Draw(dc, mCam);

	// Draw particle systems last so it is blended with scene.
	//파티클 입력배치.
	dc->IASetInputLayout(InputLayouts::Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//불 파티클.
// 	mFire.SetEyePos(mCam.GetPosition());
// 	mFire.Draw(dc, mCam);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	
	
	//비 파티클.
// 	mRain.SetEyePos(mCam.GetPosition());
// 	mRain.SetEmitPos(mCam.GetPosition());
// 	mRain.Draw(dc, mCam);

	//zBuffer off
	dc->OMSetDepthStencilState(mDepthDisableState, 1);
	//XMMATRIX tempMtx = XMMatrixTranslation(0, 0, -100);
	mMiniMap.Render(dc,  mCam);


	//zBuffer on
	dc->OMSetDepthStencilState(mDepthStencilState, 1);
	// restore default states.
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);

	
	HR(swapChain->Present(0, 0));

}

void CSceneMainGame::OnMouseDown(WPARAM btnState, int x,
	int y, const HWND& mhMainWnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CSceneMainGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneMainGame::OnMouseMove(WPARAM btnState, int x, int y)
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

void CSceneMainGame::OnResize(const float& aspectRatio)
{
	mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 1.0f, 3000.0f);
	/*mMinimapCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 1.0f, 3000.0f);*/
}

void CSceneMainGame::DrawScene(const Camera & camera, bool drawCenterSphere)
{
}



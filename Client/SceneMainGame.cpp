#include "SceneMainGame.h"



CSceneMainGame::CSceneMainGame():mSky(0), mRandomTexSRV(0), mFlareTexSRV(0), mRainTexSRV(0), mWalkCamMode(false)
{
}

CSceneMainGame::~CSceneMainGame()
{
	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);
	ReleaseCOM(mRainTexSRV);
	SafeDelete(mSky);
}


bool CSceneMainGame::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView, 
	D3D11_VIEWPORT* viewPort)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	mMinimapCam.SetPosition(XMFLOAT3(0, 100, 100));
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

	//draw�� ���� ����.
	mBox.Init(device);
	XMMATRIX boxOffset = XMMatrixIdentity();
	mBuilding.Init(device);
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

	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(device);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, flares);

	mFire.Init(device, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	mFire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc, raindrops);

	mRain.Init(device, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);


	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);
	/*
	2017 / 1 / 13 / 4:11
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:�̴ϸ��� �׸��� ���� �۾�.
	*/
	//D3D11_VIEWPORT viewportMinimap;
	//viewportMinimap.TopLeftX = 700;
	//viewportMinimap.TopLeftY = 500;
	//viewportMinimap.Width = 100;
	//viewportMinimap.Height = 100;
	//viewportMinimap.MaxDepth = 1	;
	//viewportMinimap.MinDepth = 0;

	//
	//ID3D11Texture2D* backBuffer;
	//swapChain->GetBuffer(1, __uuidof(ID3D11Texture2D),
	//	reinterpret_cast<void**>(&backBuffer));
	//device->CreateRenderTargetView(backBuffer,0, &mRenderTV);
	//ReleaseCOM(backBuffer);

	//D3D11_TEXTURE2D_DESC depthStencilDesc;

	//depthStencilDesc.Width = 100;
	//depthStencilDesc.Height = 100;
	//depthStencilDesc.MipLevels = 1;
	//depthStencilDesc.ArraySize = 1;
	//depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//// No MSAA
	//depthStencilDesc.SampleDesc.Count = 1;
	//depthStencilDesc.SampleDesc.Quality = 0;
	//depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	//depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//depthStencilDesc.CPUAccessFlags = 0;
	//depthStencilDesc.MiscFlags = 0;
	//ID3D11RenderTargetView* renderTVarr[2] = {renderTargetView,mRenderTV};

	//HR(device->CreateTexture2D(&depthStencilDesc, 0,&mMinimapDSBuffer));
	//HR(device->CreateDepthStencilView(mMinimapDSBuffer, 0, &mDepstencilV));
	//dc->OMSetRenderTargets(2, renderTVarr, mDepstencilV);
	//ReleaseCOM(renderTVarr[0]);
	//ReleaseCOM(renderTVarr[1]);

	//mMinimapViewport = viewportMinimap;
	//mMainGameViewport = viewPort;

	//D3D11_VIEWPORT mViewportArr[2];

	//mViewportArr[0] = *viewPort;
	//mViewportArr[1] = viewportMinimap;

	//dc->RSSetViewports(2,mViewportArr);

	mTimer.Start();
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
		mFire.Reset();
		mRain.Reset();
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


	mFire.Update(dt, mTimer.TotalTime());
	mRain.Update(dt, mTimer.TotalTime());

	mCam.UpdateViewMatrix();
}

void CSceneMainGame::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* depthStencilView)
{
	//�̴ϸ� ���.

	dc->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//�⺻�Է¹�ġ.
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*
	2017 / 1 / 10 / 7:00
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:�ø����ʿ���°����.ex)���̾��潺.
	*/
	dc->RSSetState(RenderStates::NoCullRS);
	mBox.Draw(dc, mCam);

	/*
	2017 / 1 / 10 / 7:00
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:������ ���°����� ���.
	*/
	dc->RSSetState(RenderStates::SolidRS);
	mBuilding.Draw(dc, mCam);
	


	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	mTerrain.Draw(dc, mCam, mDirLights);
	
	


	dc->RSSetState(0);

	mSky->Draw(dc, mCam);

	// Draw particle systems last so it is blended with scene.
	//��ƼŬ �Է¹�ġ.
	dc->IASetInputLayout(InputLayouts::Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�� ��ƼŬ.
	mFire.SetEyePos(mCam.GetPosition());
	mFire.Draw(dc, mCam);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	//�� ��ƼŬ.
	mRain.SetEyePos(mCam.GetPosition());
	mRain.SetEmitPos(mCam.GetPosition());
	mRain.Draw(dc, mCam);



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
}



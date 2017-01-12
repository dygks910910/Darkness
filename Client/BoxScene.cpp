#include "BoxScene.h"



CBoxScene::CBoxScene()
{
}


CBoxScene::~CBoxScene()
{
// 	ReleaseCOM(mRandomTexSRV);
// 	ReleaseCOM(mFlareTexSRV);
// 	ReleaseCOM(mRainTexSRV);
	SafeDelete(mSky);
}

bool CBoxScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
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

	//draw�� ���� ����.
	mBox.Init(device);
	XMMATRIX boxOffset = XMMatrixIdentity();
	//mBuilding.Init(md3dDevice);
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

// 	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
// 	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);

// 	mFire.Init(md3dDevice, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
// 	mFire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

// 	std::vector<std::wstring> raindrops;
// 	raindrops.push_back(L"Textures\\raindrop.dds");
// 	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, raindrops);
// 
// 	mRain.Init(md3dDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);


	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	mTimer.Start();
	return true;
}

void CBoxScene::UpdateScene(const float & dt)
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

void CBoxScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* depthStencilView)
{
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
	/*mBuilding.Draw(md3dImmediateContext, mCam);*/



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
// 	mFire.SetEyePos(mCam.GetPosition());
// 	mFire.Draw(md3dImmediateContext, mCam);
// 	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
// 																	   //�� ��ƼŬ.
// 	mRain.SetEyePos(mCam.GetPosition());
// 	mRain.SetEmitPos(mCam.GetPosition());
// 	mRain.Draw(md3dImmediateContext, mCam);



	// restore default states.
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(swapChain->Present(0, 0));
}

void CBoxScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CBoxScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

}

void CBoxScene::OnMouseMove(WPARAM btnState, int x, int y)
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

void CBoxScene::OnResize(const float & aspectRatio)
{
	mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 1.0f, 3000.0f);

}

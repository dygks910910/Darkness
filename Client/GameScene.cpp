#include "GameScene.h"
#define Animnum 10
bool camset = false;
XMFLOAT3 camtest;
CGameScene::CGameScene()
{
}


CGameScene::~CGameScene()
{

	SafeDelete(mSky);
	SafeDelete(mSmap);
 	SafeDelete(mSsao);
	ReleaseCOM(mRainTexSRV);
	ReleaseCOM(mRandomTexSRV);
	mMinimap.Shutdown();
	CModelManager::GetInstance()->DestroyInstance();


}

bool CGameScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain,
	const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight)
{

	send_wsa_buf1.buf = reinterpret_cast<char*>(send_buf1);
	send_wsa_buf1.len = MAX_BUF_SIZE;

	mSmap = new ShadowMap(device, clientWidth, clientHeight);
	mClientHeight = clientHeight;
	mClientWidth = clientWidth;
	//////////////////////////////////////////////////////////////////////////
	//���弼��

	mSceneBounds.Center = XMFLOAT3(0, 0, 0);
	mSceneBounds.Radius = 110;
	mLightRotationAngle = 0;

	//////////////////////////////////////////////////////////////////////////
	//����,�ؽ�ó�ҷ�����.
	mTexMgr.Init(device);
	CModelManager::GetInstance()->Init(mTexMgr, &mCam, device);
	//////////////////////////////////////////////////////////////////////////
	//zbufferOff�� ���� ����

// 	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
// 	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
// 
// 	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
// 	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
// 	depthDisabledStencilDesc.DepthEnable = false;
// 	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
// 	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
// 	depthDisabledStencilDesc.StencilEnable = true;
// 	depthDisabledStencilDesc.StencilReadMask = 0xFF;
// 	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
// 	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
// 	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
// 	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
// 	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
// 	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
// 	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
// 	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
// 	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
// 
// 	HR(device->CreateDepthStencilState(&depthDisabledStencilDesc, &ds));
// 	depthDisabledStencilDesc.DepthEnable = true;
// 
// 	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
// 	// Initialize the description of the stencil state.
// 	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
// 
// 	// Set up the description of the stencil state.
// 	depthStencilDesc.DepthEnable = true;
// 	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
// 	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
// 
// 	depthStencilDesc.StencilEnable = true;
// 	depthStencilDesc.StencilReadMask = 0xFF;
// 	depthStencilDesc.StencilWriteMask = 0xFF;
// 
// 	// Stencil operations if pixel is front-facing.
// 	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
// 	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
// 	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
// 	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
// 
// 	// Stencil operations if pixel is back-facing.
// 	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
// 	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
// 	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
// 	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
// 
// 	// Create the depth stencil state.
// 	HR(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
// 	dc->OMSetDepthStencilState(mDepthStencilState, 1);

	//���� ����
	//BuildShapeGeometryBuffers();

	//////////////////////////////////////////////////////////////////////////
	int total = 0;

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

	mSsao = new Ssao(device, dc, mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());


	mSky = new Sky(device, L"Textures/Skybox/StarSky.dds", 512.0f);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(2.0f);
	Effects::BasicFX->SetFogRange(15.0f);

	//������ǥ�� �׷��ֱ�.
	XMFLOAT4X4 temp4x4;
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
// 	mCordWorld.Init(device, temp4x4, 5000);

	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(device);
	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(device, dc	, raindrops);
	mRain.Init(device, Effects::RainFX, mRainTexSRV, mRandomTexSRV,10000);

	mTimer.Reset();
	mTimer.Start();

	mMinimap.Initialize(device, mClientWidth, mClientHeight, mCam.othMtx(), 100, 100);
	mDrawText.Init(device, dc);
	OnResize();
	return true;
}
bool testcamera = true;
std::string CGameScene::UpdateScene(const float dt, MSG& msg)
{
	//�̴ϸ� ��ġ ������Ʈ.
	//�ӽ÷� ����ϴ� ��Ű�ϵ�𵨹迭�� 5��° ��.�� �÷��̾���.
  	mMinimap.PositionUpdate(CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._41,
  		CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._43);
	//mMinimap.PositionUpdate(0,0);
	mTimer.Tick();
	//
	// Control the camera.
	//

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		mCam.Walk(20.0f*dt);

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		mCam.Walk(-20.0f*dt);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		mCam.Strafe(-20.0f*dt);

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		mCam.Strafe(20.0f*dt);

	if (GetAsyncKeyState('Z') & 0x8000)
	{
		testcamera = false;
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		testcamera = true;
	}
	

	//
	// Reset particle systems.
	//
	BuildShadowTransform();
	mRain.Update(dt, mTimer.TotalTime());

 	if (NetworkMgr::GetInstance()->isGameOver)
 	{
 		return SceneName::endingScene;
 	}
 	if (!mbTimeOver) {
 		countDownSec -= mTimer.DeltaTime();
 		
 		//�����ð��� 0���ϰ� �Ǹ�?
 		if (countDownMin < 0)
 		{
#ifdef _DEBUG
 			std::cout << "gameOver" << std::endl;
#endif
 			countDownMin = 0;
 			countDownSec = 0;
 			mbTimeOver = true;
 		}
 		//�ʰ� 0���ϰ� �Ǹ�?
 		else if (countDownSec - mTimer.DeltaTime() <= 0)
 		{
 			countDownSec = 60;
 			countDownMin -= 1;
 		}
 		wchar_t tempString[10];
 		wsprintf(tempString, TEXT("%d:%d"), (int)countDownMin, (int)countDownSec);
 		timerString = tempString;
 	}
	CModelManager::GetInstance()->UpdateModel(dt, mCam);
	// 	mCharacterInstance1.Update(dt);
	// 
	// 	for (int i = 0; i < Animnum; ++i)
	// 		mCharacterInstances[i].Update(dt);

	mCam.UpdateViewMatrix();
	return "";
}

void CGameScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{
#ifdef _DEBUG
	//std::cout << camtest.x << std::endl;
#endif // _DEBUG

	if (camtest.x !=0 && camset ==false)
	{
		XMFLOAT3 charpo, up;
		up.x = 0;
		up.y = 1;
		up.z = 0;

		charpo.x = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._41;
		charpo.y = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._42+1;
		charpo.z = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._43;

		mCam.SetPosition(camtest);
		mCam.LookAt(camtest, charpo, up);

		camset = true;

		cs_packet_draw_start* draw = reinterpret_cast<cs_packet_draw_start*>(&send_buf1);
		draw->size = sizeof(cs_packet_draw_start);
		draw->type = CS_PACKET_CLIENT_DRAW_START;
		send_wsa_buf1.len = sizeof(cs_packet_draw_start);
		DWORD io_byte2;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf1, 1, &io_byte2, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;


	}


	mSmap->BindDsvAndSetNullRenderTarget(dc);

	DrawSceneToShadowMap(dc);
	//DrawSceneToSsaoNormalDepthMap();


	dc->RSSetState(0);
	//////////////////////////////////////////////////////////////
	//ID3DX11EffectTechnique* animatedTech = Effects::SsaoNormalDepthFX->NormalDepthSkinnedTech;
	//ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;
	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);
	//
	// Restore the back and depth buffer to the OM stage.
	//
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//mDc->RSSetViewports(1, viewPort);
	mSsao->SetNormalDepthRenderTarget(dsv);

	//DrawSceneToSsaoNormalDepthMap();

 	mSsao->ComputeSsao(mCam);
	//mSsao->BlurAmbientMap(2);

	ID3D11RenderTargetView* renderTargets[1] = { rtv };
	dc->OMSetRenderTargets(1, renderTargets, dsv);
	dc->RSSetViewports(1, viewPort);


	dc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Black));


	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());
	//Effects::BasicFX->SetSsaoMap(mSsao->AmbientSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mCam.GetPosition());
	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::NormalMapFX->SetShadowMap(mSmap->DepthMapSRV());
 	Effects::NormalMapFX->SetSsaoMap(mSsao->AmbientSRV());

	Effects::InstancedBasicFX->SetDirLights(mDirLights);
	Effects::InstancedBasicFX->SetEyePosW(mCam.GetPosition());
	//Effects::InstancedBasicFX->SetSsaoMap(mSsao->AmbientSRV());
	Effects::InstancedBasicFX->SetEyePosW(mCam.GetPosition());

	ID3DX11EffectTechnique* activeNormalMappingTech = Effects::NormalMapFX->Light1TexTech;
	ID3DX11EffectTechnique* activeBasicTech = Effects::BasicFX->Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* activeInstanceTech = Effects::InstancedBasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Figure out which technique to use for different geometry.


	//XMMATRIX worldView;
	//XMMATRIX worldInvTransposeView;
	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 


	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	//
	// Draw the spheres with cubemap reflection.
	//
	CModelManager::GetInstance()->DrawStaticBasicModels(dc, activeBasicTech,
		mShadowTransform, mCam);
	CModelManager::GetInstance()->DrawStaticNormalModels(dc, activeNormalMappingTech,
		mShadowTransform, mCam);
	CModelManager::GetInstance()->DrawInstancedModel(dc, activeInstanceTech,
		mShadowTransform, mCam);
	//////////////////////////////////////////////////////////////////////////
	//draw Animation
	CModelManager::GetInstance()->DrawSkinnedModels(dc, activeSkinnedTech,mShadowTransform, mCam);

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
// 	mCordWorld.Draw(mDc, mCam);
	mSky->Draw(dc, mCam);

	CModelManager::GetInstance()->DrawInstancedModel(dc, activeInstanceTech, mShadowTransform, mCam);

	mDrawText(timerString, 75, mClientWidth / 2-100, 0, FontColorForFW::RED);
 	mMinimap.Render(dc, mCam);
	if (NetworkMgr::GetInstance()->isGameStart)
	{
		//std::cout << "������ ���۵�.";
		mTimer.Start();
	}
	else
	{
		mTimer.Stop();
		mDrawText(L"�ٸ� �÷��̾ ��ٸ�����...", 40, mClientWidth / 2 - 200, mClientHeight / 2, FontColorForFW::WHITE);
	}


	dc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	dc->IASetInputLayout(InputLayouts::Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	mRain.SetEyePos(mCam.GetPosition());
	mRain.SetEmitPos(mCam.GetPosition());
	mRain.Draw(dc, mCam);

	// restore default states, as the SkyFX changes them in the effect file.

	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	dc->PSSetShaderResources(0, 16, nullSRV);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(swapChain->Present(0, 0));
}

void CGameScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mAnimOneCheck = false;
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mAttack = true;

	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CGameScene::OnMouseUp(WPARAM btnState, int x, int y)
{

	//CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mAttack = false;

	ReleaseCapture();
}
float sumdx = 0;
void CGameScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	//if (!camset)
	//{
	//	mCam.SetPosition(camtest);
	//	camset = true;
	//}
	if ((btnState & MK_RBUTTON) != 0)
	{
		if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mAlive)
		{
			XMMATRIX	matRot;
			XMFLOAT3 objectpos, campos, dist, up;
			XMVECTOR eye, upper;
			//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			up.x = 0;
			up.y = 1;
			up.z = 0;
			///////////////������Ʈ ��ġ ���/////////////////
			

			//////////////ī�޶� ��ġ ���////////////////////
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
			float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
			//////////////ī�޶�� ��ü �Ÿ� ���ϱ�///////////
			

			/*if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mRotateAngle == 0)
				sumdx = 0;*/

			sumdx += dx;

			CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mRotateAngle = sumdx;

			matRot = XMMatrixRotationY(dx);
			objectpos.x = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._41;
			objectpos.y = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._42 + 1;
			objectpos.z = CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].World._43;

			campos = mCam.GetPosition();

			dist.x = campos.x - objectpos.x;
			dist.y = campos.y - objectpos.y;
			dist.z = campos.z - objectpos.z;

			eye = XMVector3TransformCoord(XMLoadFloat3(&dist), matRot);
			upper = XMVector3TransformCoord(XMLoadFloat3(&up), matRot);

			XMStoreFloat3(&dist, eye);
			XMStoreFloat3(&up, upper);

			CModelManager::GetInstance()->GetSkinnedInstanceModels()[NetworkMgr::GetInstance()->getId()].mCharCamPos = dist;

			campos.x = dist.x + objectpos.x;
			campos.y = dist.y + objectpos.y;
			campos.z = dist.z + objectpos.z;
			std::cout << objectpos.x - campos.x << ' ' << objectpos.y - campos.y << ' ' << objectpos.z - campos.z << std::endl;
			mCam.LookAt(campos, objectpos, up);
			///////////////////////////////////////////
		}
		else
		{
			float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
			float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
			mCam.Pitch(dy);
			mCam.RotateY(dx);
		}

	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CGameScene::OnResize()
{
	float a = AspectRatio();
	mCam.SetLens(0.36f*MathHelper::Pi, AspectRatio(), 0.01f, 3000.0f);
	//XNA::ComputeFrustumFromProjection(&mCamFrustum, &mCam.Proj());
	// 	if (mSsao)
	// 	{
	// 		mSsao->OnSize(mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());
	// 	}
}

void CGameScene::DrawSceneToShadowMap(ID3D11DeviceContext* dc)
{
	D3DX11_TECHNIQUE_DESC techDesc;

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

	//mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	CModelManager::GetInstance()->DrawToShadowMap(dc, smapTech, mLightView, mLightProj);


	//mDc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	//animatedSmapTech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	// Instance 1

	//	world = XMLoadFloat4x4(&mCharacterInstance1.World);
	//	worldInvTranspose = MathHelper::InverseTranspose(world);
	//	worldViewProj = world*view*proj;

	//	Effects::BuildShadowMapFX->SetWorld(world);
	//	Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
	//	Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
	//	Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());
	//	Effects::BuildShadowMapFX->SetBoneTransforms(
	//		&mCharacterInstance1.FinalTransforms[0],
	//		mCharacterInstance1.FinalTransforms.size());


	//	animatedSmapTech->GetPassByIndex(p)->Apply(0, mDc);

	//	for (UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
	//	{
	//		mCharacterInstance1.Model->ModelMesh.Draw(mDc, subset);
	//	}

	//}

}
void CGameScene::BuildShadowTransform()
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

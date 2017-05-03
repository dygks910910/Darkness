#include "GameScene.h"
#define Animnum 10

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

}

bool CGameScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain,
	const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight)
{
	mSmap = new ShadowMap(device, clientWidth, clientHeight);
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	mClientHeight = clientHeight;
	mClientWidth = clientWidth;
	//////////////////////////////////////////////////////////////////////////
	//���弼��


	mSceneBounds.Center = XMFLOAT3(0, 0, 0);
	mSceneBounds.Radius = 110;
	mLightRotationAngle = 0;


	//////////////////////////////////////////////////////////////////////////
	//����,�ؽ�ó�ҷ�����.
	mTexMgr.Init(mDevice);
// <<<<<<< HEAD
// 	mModelMgr.Init(mTexMgr, &mCam, device);
// =======
	CModelManager::GetInstance()->Init(mTexMgr, &mCam, device);
// >>>>>>> SpotLight


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


	mSky = new Sky(device, L"Darkness fbx/Skybox/Day Sun Twilight Clear.png", 1000.0f);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(2.0f);
	Effects::BasicFX->SetFogRange(15.0f);

	//������ǥ�� �׷��ֱ�.
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
// <<<<<<< HEAD
// 
// void CTestScene::UpdateScene(const float& dt)
// =======
bool testcamera = true;
std::string CGameScene::UpdateScene(const float dt, MSG& msg)
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

	if (GetAsyncKeyState('Z') & 0x8000)
	{
		testcamera = false;
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		testcamera = true;
	}
	// 	if (GetAsyncKeyState('X') & 0x8000)
	// 	{
	// 		mCharacterInstance1.ClipName = "Attack1";
	// 		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[2];
	// 		mCharacterInstance1.mAnimCnt = 0;
	// 	}
	// 	if (GetAsyncKeyState('V') & 0x8000)
	// 	{
	// 		mCharacterInstance1.ClipName = "Run";
	// 		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[3];
	// 		mCharacterInstance1.mAnimCnt = 0;
	// 	}
	// 	if (GetAsyncKeyState('B') & 0x8000)
	// 	{
	// 		mCharacterInstance1.ClipName = "Walk";
	// 		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[1];
	// 		mCharacterInstance1.mAnimCnt = 0;
	// 	}
	/*if (GetAsyncKeyState('N') & 0x8000)
	mCharacterInstance1.ClipName = "Jump";
	if (GetAsyncKeyState('M') & 0x8000)
	mCharacterInstance1.ClipName = "Death";*/


	//
	// Walk/fly mode
	//

	// 	if (GetAsyncKeyState('R') & 0x8000)
	// 	{
	// 		// 		mFire.Reset();
	// 		// 		mRain.Reset();
	// 	}
	// 
	// Clamp camera to terrain surface in walk mode.
	//

	//
	// Reset particle systems.
	//
	BuildShadowTransform();
	mRain.Update(dt, mTimer.TotalTime());

	CModelManager::GetInstance()->UpdateModel(dt, mCam);
	// 	mCharacterInstance1.Update(dt);
	// 
	// 	for (int i = 0; i < Animnum; ++i)
	// 		mCharacterInstances[i].Update(dt);

	mCam.UpdateViewMatrix();
	return "";
}

bool camset = false;
void CGameScene::Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{

	if (!camset)
	{
		mCam.SetPosition(camtest);
		camset = true;
	}

	mSmap->BindDsvAndSetNullRenderTarget(mDc);

	DrawSceneToShadowMap();
	//DrawSceneToSsaoNormalDepthMap();


	mDc->RSSetState(0);
	//////////////////////////////////////////////////////////////
	//ID3DX11EffectTechnique* animatedTech = Effects::SsaoNormalDepthFX->NormalDepthSkinnedTech;
	//ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;
	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);
	//
	// Restore the back and depth buffer to the OM stage.
	//
	mDc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//mDc->RSSetViewports(1, viewPort);
	mSsao->SetNormalDepthRenderTarget(dsv);

	//DrawSceneToSsaoNormalDepthMap();

 	mSsao->ComputeSsao(mCam);
	//mSsao->BlurAmbientMap(2);

	ID3D11RenderTargetView* renderTargets[1] = { rtv };
	mDc->OMSetRenderTargets(1, renderTargets, dsv);
	mDc->RSSetViewports(1, viewPort);


	mDc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));

	//mDc->OMSetDepthStencilState(RenderStates::EqualsDSS, 0);


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

	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Figure out which technique to use for different geometry.


	//XMMATRIX worldView;
	//XMMATRIX worldInvTransposeView;
	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 


	if (GetAsyncKeyState('1') & 0x8000)
		mDc->RSSetState(RenderStates::WireframeRS);


	//
	// Draw the spheres with cubemap reflection.
	//
	CModelManager::GetInstance()->DrawStaticBasicModels(mDc, activeBasicTech,
		mShadowTransform, mCam);
	CModelManager::GetInstance()->DrawStaticNormalModels(mDc, activeNormalMappingTech,
		mShadowTransform, mCam);
	CModelManager::GetInstance()->DrawInstancedModel(mDc, activeInstanceTech,
		mShadowTransform, mCam);
	//////////////////////////////////////////////////////////////////////////
	//draw Animation
	CModelManager::GetInstance()->DrawSkinnedModels(mDc, activeSkinnedTech,mShadowTransform, mCam);

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	mDc->HSSetShader(0, 0, 0);
	mDc->DSSetShader(0, 0, 0);

	mCordWorld.Draw(mDc, mCam);

	mSky->Draw(mDc, mCam);

	CModelManager::GetInstance()->DrawInstancedModel(mDc, activeInstanceTech, mShadowTransform, mCam);
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
//////////////////////0406/////////////////////////


void CGameScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CGameScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
float sumdx = 0;
void CGameScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		if (testcamera)
		{
			XMMATRIX	matRot;
			XMFLOAT3 objectpos, campos, dist, up;
			XMVECTOR eye, upper;
			//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			up.x = 0;
			up.y = 1;
			up.z = 0;
			///////////////������Ʈ ��ġ ���/////////////////
			objectpos.x = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World._41;
			objectpos.y = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World._42 + 1;
			objectpos.z = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World._43;

			//////////////ī�޶� ��ġ ���////////////////////
			campos = mCam.GetPosition();
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
			float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
			//////////////ī�޶�� ��ü �Ÿ� ���ϱ�///////////
			dist.x = campos.x - objectpos.x;
			dist.y = campos.y - objectpos.y;
			dist.z = campos.z - objectpos.z;

			if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mRotateAngle == 0)
				sumdx = 0;

			sumdx += dx;
			//std::cout << "sum" << sumdx << std::endl;

			CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mRotateAngle = sumdx;

			matRot = XMMatrixRotationY(dx);
			eye = XMVector3TransformCoord(XMLoadFloat3(&dist), matRot);
			upper = XMVector3TransformCoord(XMLoadFloat3(&up), matRot);

			XMStoreFloat3(&dist, eye);
			XMStoreFloat3(&up, upper);

			campos.x = dist.x + objectpos.x;
			campos.y = dist.y + objectpos.y;
			campos.z = dist.z + objectpos.z;
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
	mCam.SetLens(0.36f*MathHelper::Pi, AspectRatio(), 0.01f, 3000.0f);
	//XNA::ComputeFrustumFromProjection(&mCamFrustum, &mCam.Proj());
	// 	if (mSsao)
	// 	{
	// 		mSsao->OnSize(mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());
	// 	}
}

void CGameScene::DrawSceneToShadowMap()
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

	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	CModelManager::GetInstance()->DrawToShadowMap(mDc, smapTech, mLightView, mLightProj);


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

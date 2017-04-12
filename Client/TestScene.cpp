#include "TestScene.h"

#define Animnum 10


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
	SafeDelete(mCharacterModel);
	SafeDelete(mSky);
	SafeDelete(mSmap);
	SafeDelete(mSsao);
	ReleaseCOM(mRainTexSRV);
	ReleaseCOM(mRandomTexSRV);
	

	
// 	for (auto p = mclipAnimbuf.begin(); p != mclipAnimbuf.end(); ++p)
// 	{
// 		p->second->clear();
// 		SafeDelete(p->second);
// 	}
	
}

bool CTestScene::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
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
	int total = 0;
	std::ifstream fin("Idle.txt");
	
	mCharacterModel = new SkinnedModel(device, mTexMgr, "Models\\ResultIdle.txt", L"Textures\\");

	mCharacterInstance1.Model = mCharacterModel;
	mCharacterInstance2.Model = mCharacterModel;
	mCharacterInstance1.TimePos = 0.0f;
	mCharacterInstance2.TimePos = 0.0f;
	mCharacterInstance1.ClipName = "Idle";
	mCharacterInstance2.ClipName = "Idle";
	mCharacterInstance1.FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());
	mCharacterInstance2.FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());
	std::vector<std::vector<XMFLOAT4X4>> testfinalTransform;

	for (int k = 0; k < 4; ++k)
	{
		fin >> total;
		mAnimTotalCnt[k] = total;
		//mCharacterInstance1.TestFinalTransforms = new std::vector<XMFLOAT4X4>[total];
		testfinalTransform.resize(total);
		for (int i = 0; i < total; ++i)
		{
			testfinalTransform[i].resize(mCharacterModel->SkinnedData.BoneCount());
			for (int j = 0; j < mCharacterModel->SkinnedData.BoneCount(); ++j)
			{
				fin >> testfinalTransform[i][j]._11 >> testfinalTransform[i][j]._12 >> testfinalTransform[i][j]._13 >> testfinalTransform[i][j]._14
					>> testfinalTransform[i][j]._21 >> testfinalTransform[i][j]._22 >> testfinalTransform[i][j]._23 >> testfinalTransform[i][j]._24
					>> testfinalTransform[i][j]._31 >> testfinalTransform[i][j]._32 >> testfinalTransform[i][j]._33 >> testfinalTransform[i][j]._34
					>> testfinalTransform[i][j]._41 >> testfinalTransform[i][j]._42 >> testfinalTransform[i][j]._43 >> testfinalTransform[i][j]._44;
			}
			mclipAnimbuf.insert(std::pair<std::string, std::vector<XMFLOAT4X4>>(mClipname[k], testfinalTransform[i]));
		}
	}
	mCharacterInstance1.mClipAnimbuf = &mclipAnimbuf;
	mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[0];
	mCharacterInstance1.mAnimCnt = 0;
	for (int i = 0; i < Animnum; ++i)
	{
		mCharacterInstances[i].Model = mCharacterModel;
		mCharacterInstances[i].TimePos = 0.0f;
		//mCharacterInstances[i].ClipName = "Scene";
		mCharacterInstances[i].FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());
		//mCharacterInstances[i].TestFinalTransforms = new std::vector<XMFLOAT4X4>[total];
		mCharacterInstances[i].mClipAnimbuf = &mclipAnimbuf;
		int a = rand() % 4;
		mCharacterInstances[i].ClipName = mClipname[a];
		mCharacterInstances[i].mAnimTotalTime = mAnimTotalCnt[a];
		mCharacterInstances[i].mAnimCnt = 0;

		/*for (int j = 0; j < total; ++j)
			mCharacterInstances[i].TestFinalTransforms[j].resize(mCharacterModel->SkinnedData.BoneCount());
		mCharacterInstances[i].TestFinalTransforms = mCharacterInstance1.TestFinalTransforms;*/
	}
	fin.close();

	// Reflect to change coordinate system from the RHS the data was exported out as.
	XMMATRIX modelScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX modelRot = XMMatrixRotationY(MathHelper::Pi);
	XMMATRIX modelOffset = XMMatrixTranslation(-2.0f, 2.0f, -7.0f);
	XMStoreFloat4x4(&mCharacterInstance1.World, modelScale*modelRot*modelOffset);

	modelOffset = XMMatrixTranslation(2.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&mCharacterInstance2.World, modelScale*modelRot*modelOffset);

	for (int i = 0; i < Animnum; ++i)
	{
		modelOffset = XMMatrixTranslation(2.0f, 1.0f*i, -7.0f);
		XMStoreFloat4x4(&mCharacterInstances[i].World, modelScale*modelRot*modelOffset);
	}
	//////////////////////////////////////////////////////////////////////////////////////

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

void CTestScene::UpdateScene(const float dt)
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
		mCharacterInstance1.ClipName = "Idle";
		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[0];
		mCharacterInstance1.mAnimCnt = 0;
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		mCharacterInstance1.ClipName = "Attack1";
		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[2];
		mCharacterInstance1.mAnimCnt = 0;
	}
	if (GetAsyncKeyState('V') & 0x8000)
	{
		mCharacterInstance1.ClipName = "Run";
		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[3];
		mCharacterInstance1.mAnimCnt = 0;
	}
	if (GetAsyncKeyState('B') & 0x8000)
	{
		mCharacterInstance1.ClipName = "Walk";
		mCharacterInstance1.mAnimTotalTime = mAnimTotalCnt[1];
		mCharacterInstance1.mAnimCnt = 0;
	}
	/*if (GetAsyncKeyState('N') & 0x8000)
		mCharacterInstance1.ClipName = "Jump";
	if (GetAsyncKeyState('M') & 0x8000)
		mCharacterInstance1.ClipName = "Death";*/


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


	mCharacterInstance1.Update(dt);

	for (int i = 0; i < Animnum; ++i)
		mCharacterInstances[i].Update(dt);

	mCam.UpdateViewMatrix();
}

void CTestScene::Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{

	mSmap->BindDsvAndSetNullRenderTarget(mDc);

	DrawSceneToShadowMap();
	//DrawSceneToSsaoNormalDepthMap();


	mDc->RSSetState(0);
	//////////////////////////////////////////////////////////////
	//ID3DX11EffectTechnique* animatedTech = Effects::SsaoNormalDepthFX->NormalDepthSkinnedTech;
	//ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);


	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	//
	// Restore the back and depth buffer to the OM stage.
	//
	mDc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDc->RSSetViewports(1, viewPort);
	//mSsao->SetNormalDepthRenderTarget(dsv);

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

	ID3DX11EffectTechnique* activeNormalMappingTech = Effects::NormalMapFX->Light3TexTech;
	ID3DX11EffectTechnique* activeBasicTech = Effects::BasicFX->Light3TexReflectTech;
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


	mModelMgr.DrawStaticBasicModels(mDc, activeBasicTech,
		XMLoadFloat4x4(&mShadowTransform), mCam);
	mModelMgr.DrawStaticNormalModels(mDc, activeNormalMappingTech,
		XMLoadFloat4x4(&mShadowTransform), mCam);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	D3DX11_TECHNIQUE_DESC techDesc;

	mDc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);
	
	activeSkinnedTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		world = XMLoadFloat4x4(&mCharacterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*mCam.View()*mCam.Proj();

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		Effects::NormalMapFX->SetBoneTransforms(
			&mCharacterInstance1.FinalTransforms[0],
			mCharacterInstance1.FinalTransforms.size());

		for (UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			Effects::NormalMapFX->SetMaterial(mCharacterInstance1.Model->Mat[subset]);
			Effects::NormalMapFX->SetDiffuseMap(mCharacterInstance1.Model->DiffuseMapSRV[subset]);
			Effects::NormalMapFX->SetNormalMap(mCharacterInstance1.Model->NormalMapSRV[subset]);

			activeSkinnedTech->GetPassByIndex(p)->Apply(0, mDc);
			mCharacterInstance1.Model->ModelMesh.Draw(mDc, subset);
		}
		for (int i = 0; i < Animnum; ++i)
		{
				world = XMLoadFloat4x4(&mCharacterInstances[i].World);
				worldInvTranspose = MathHelper::InverseTranspose(world);
				worldViewProj = world*mCam.View()*mCam.Proj();

				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
				Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
				Effects::NormalMapFX->SetBoneTransforms(
					&mCharacterInstances[i].FinalTransforms[0],
					mCharacterInstances[i].FinalTransforms.size());

				for (UINT subset = 0; subset < mCharacterInstances[i].Model->SubsetCount; ++subset)
				{
					Effects::NormalMapFX->SetMaterial(mCharacterInstances[i].Model->Mat[subset]);
					Effects::NormalMapFX->SetDiffuseMap(mCharacterInstances[i].Model->DiffuseMapSRV[subset]);
					Effects::NormalMapFX->SetNormalMap(mCharacterInstances[i].Model->NormalMapSRV[subset]);

					activeSkinnedTech->GetPassByIndex(p)->Apply(0, mDc);
					mCharacterInstances[i].Model->ModelMesh.Draw(mDc, subset);
				}

		}
	}
	
	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	mDc->HSSetShader(0, 0, 0);
	mDc->DSSetShader(0, 0, 0);

	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//////////////////////////////////////////////////////////////////////////
	//인스턴스버퍼 출력
	//mDc->IASetInputLayout(InputLayouts::InstancedBasic32);





	//////////////////////////////////////////////////////////////////////
	//mDc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	//animatedTech->GetDesc(&techDesc);

	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	// Instance 1

	//	world = XMLoadFloat4x4(&mCharacterInstance1.World);
	//	worldInvTranspose = MathHelper::InverseTranspose(world);
	//	worldView = world*view;
	//	worldInvTransposeView = worldInvTranspose*view;
	//	worldViewProj = world*view*proj;

	//	Effects::SsaoNormalDepthFX->SetWorldView(worldView);
	//	Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
	//	Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
	//	Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
	//	Effects::SsaoNormalDepthFX->SetBoneTransforms(
	//		&mCharacterInstance1.FinalTransforms[0],
	//		mCharacterInstance1.FinalTransforms.size());

	//	animatedTech->GetPassByIndex(p)->Apply(0, mDc);

	//	for (UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
	//	{
	//		mCharacterInstance1.Model->ModelMesh.Draw(mDc, subset);
	//	}


	//}






	// Debug view depth buffer.
	//	if( GetAsyncKeyState('Z') & 0x8000 )
	{
		//DrawSceneQuard();
	}
	mCordWorld.Draw(mDc, mCam);

	mSky->Draw(mDc, mCam);
	mModelMgr.DrawInstancedModel(mDc, activeInstanceTech, XMLoadFloat4x4(&mShadowTransform), mCam);

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

void CTestScene::DrawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormalDepthFX->NormalDepthTech;
	ID3DX11EffectTechnique* animatedTech = Effects::SsaoNormalDepthFX->NormalDepthSkinnedTech;


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;
	mDc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mModelMgr.DrawStaticSsaoNormalModels(mDc, tech,
		XMLoadFloat4x4(&mShadowTransform), mCam);



	//	mModelMgr.DrawInstanceSsaoModels(mDc, tech, XMLoadFloat4x4(&mShadowTransform), mCam);
	//


	D3DX11_TECHNIQUE_DESC techDesc;


	//
	// Draw the animated characters.
	//

	mDc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	animatedTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Instance 1

		world = XMLoadFloat4x4(&mCharacterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		Effects::SsaoNormalDepthFX->SetBoneTransforms(
			&mCharacterInstance1.FinalTransforms[0],
			mCharacterInstance1.FinalTransforms.size());

		animatedTech->GetPassByIndex(p)->Apply(0, mDc);

		for (UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			mCharacterInstance1.Model->ModelMesh.Draw(mDc, subset);
		}
	}

	//for (int i = 0; i < 70; ++i)
	//{
	//	for (UINT p = 0; p < techDesc.Passes; ++p)
	//	{
	//		// Instance 1

	//		world = XMLoadFloat4x4(&mCharacterInstances[i].World);
	//		worldInvTranspose = MathHelper::InverseTranspose(world);
	//		worldView = world*view;
	//		worldInvTransposeView = worldInvTranspose*view;
	//		worldViewProj = world*view*proj;

	//		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
	//		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
	//		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
	//		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
	//		Effects::SsaoNormalDepthFX->SetBoneTransforms(
	//			&mCharacterInstances[i].FinalTransforms[0],
	//			mCharacterInstances[i].FinalTransforms.size());

	//		animatedTech->GetPassByIndex(p)->Apply(0, mDc);

	//		for (UINT subset = 0; subset < mCharacterInstances[i].Model->SubsetCount; ++subset)
	//		{
	//			mCharacterInstances[i].Model->ModelMesh.Draw(mDc, subset);
	//		}
	//	}
	//}
		// Instance 2

		/*world = XMLoadFloat4x4(&mCharacterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		Effects::SsaoNormalDepthFX->SetBoneTransforms(
		&mCharacterInstance2.FinalTransforms[0],
		mCharacterInstance2.FinalTransforms.size());

		animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for (UINT subset = 0; subset < mCharacterInstance2.Model->SubsetCount; ++subset)
		{
		mCharacterInstance2.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}*/
		//////////////////////////
		/*for (int i = 0; i < 70; ++i)
		{
		world = XMLoadFloat4x4(&mCharacterInstance[i].World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		Effects::SsaoNormalDepthFX->SetBoneTransforms(
		&mCharacterInstance[i].FinalTransforms[0],
		mCharacterInstance[i].FinalTransforms.size());

		animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for (UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
		mCharacterInstance[i].Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}
		}*/
	

	mDc->RSSetState(0);
}
///////////////////////////////////////////////////////////////////

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

	mModelMgr.DrawToShadowMap(mDc, smapTech, mLightView, mLightProj);


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

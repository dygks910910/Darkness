#include "GameScene.h"




GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

bool GameScene::Init()
{
	mCam.SetPosition(0, 0, 0);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));
	mTexMgr.Init(md3dDevice);
	CModelManager::GetInstance()->Init(mTexMgr, &mCam);

	mDirLights[0].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

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

	mSky = new Sky(md3dDevice, L"Textures/Skybox/StarSky.dds", 512.0f);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(2.0f);
	Effects::BasicFX->SetFogRange(15.0f);

	mMinimap.Initialize(mCam.View(),50,50);
	mDrawText.Init();

	return true;
}

void GameScene::OnKeyboardButtonDown(WPARAM wParam, UINT msg)
{
}

int GameScene::UpdateScene(const float dt, MSG & msg)
{
	return 0;
}

void GameScene::Draw()
{
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		ID3D11RenderTargetView* renderTargets[1] = { mRenderTargetView };
		md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);
		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));

		XMMATRIX view = mCam.View();
		XMMATRIX proj = mCam.Proj();
		XMMATRIX viewProj = mCam.ViewProj();
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		// Set per frame constants.


}

void GameScene::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void GameScene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void GameScene::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void GameScene::OnResize()
{
}

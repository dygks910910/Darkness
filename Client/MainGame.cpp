//***************************************************************************************
// ParticlesDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates GPU based particle systems.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************
#include "MainGame.h"




CMainGame::CMainGame(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	mMainWndCaption = L"Darkness";
	mEnable4xMsaa = false;

	mainGameScene = new CSceneMainGame;

}

CMainGame::~CMainGame()
{
	md3dImmediateContext->ClearState();

	SafeDelete(mainGameScene);
	
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool CMainGame::Init()
{
	
	//기본세팅
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	mainGameScene->Init(md3dDevice, md3dImmediateContext,
		mSwapChain, mRenderTargetView, mDepthStencilView);

	return true;
}

void CMainGame::OnResize()
{
	D3DApp::OnResize();
	mainGameScene->OnResize(AspectRatio());
	
}

void CMainGame::UpdateScene(float dt)
{
	mainGameScene->UpdateScene(dt);
}

void CMainGame::DrawScene()
{
	mainGameScene->Draw();
	

}


void CMainGame::OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd)
{
	mainGameScene->OnMouseDown(btnState, x, y,mhMainWnd);
}

void CMainGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	mainGameScene->OnMouseUp(btnState, x, y);
}

void CMainGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	mainGameScene->OnMouseMove(btnState, x, y);
}


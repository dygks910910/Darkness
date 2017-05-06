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

	mSceneManager = new CSceneManager;

}
#include "ModelManager.h"
CMainGame::~CMainGame()
{
	md3dImmediateContext->ClearState();

	SafeDelete(mSceneManager);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
	NetworkMgr::GetInstance()->DestroyInstance();
}

bool CMainGame::Init()
{

	//기본세팅
	if (!D3DApp::Init())
		return false;


	// Must init Effects first since InputLayouts depend on shader signatures.

	NetworkMgr::GetInstance()->SetWindowHandle(D3DApp::MainWnd());

	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	mSceneManager->Init(md3dDevice, md3dImmediateContext,
		mSwapChain, mRenderTargetView, &mScreenViewport, mDepthStencilView,
		mClientWidth, mClientHeight);


	return true;
}

void CMainGame::OnResize()
{
	D3DApp::OnResize();
	mSceneManager->OnResize();

}

void CMainGame::UpdateScene(float dt, MSG& msg)
{
// 	if (GetAsyncKeyState('C') & 0x8000)
// 	{
// 		mSceneManager->ChangeScene(SceneName::test, dt,
// 			md3dDevice, md3dImmediateContext);
// 		mSceneManager->SetSceneKey(SceneName::test);
// 	}
// 	if (GetAsyncKeyState('X') & 0x8000)
// 	{
// 		mSceneManager->ChangeScene(SceneName::MainScene, dt,
// 			md3dDevice, md3dImmediateContext);
// 		mSceneManager->SetSceneKey(SceneName::MainScene);
// 	}
	//mSceneManager->UpdateScene(dt);
	// 	if (GetAsyncKeyState('X') & 0x8000)
	// 	{
	// 		mSceneManager->ChangeScene(SceneName::MainScene, dt,
	// 			md3dDevice, md3dImmediateContext);
	// 		mSceneManager->SetSceneKey(SceneName::MainScene);
	// 	}
	mSceneManager->UpdateScene(dt, msg);
}

void CMainGame::DrawScene()
{
	mSceneManager->Draw(md3dDevice, md3dImmediateContext,
		mSwapChain, mRenderTargetView, mDepthStencilView, &mScreenViewport);


}

void CMainGame::Packet(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
// 	if (mSceneManager->GetSceneKey() == SceneName::gameScene)
// 	{
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			exit(-1);
		}

		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			NetworkMgr::GetInstance()->ReadPacket((SOCKET)wParam);
			break;

		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
	//}
}

void CMainGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	mSceneManager->OnMouseDown(btnState, x, y, mhMainWnd);
}

void CMainGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	mSceneManager->OnMouseUp(btnState, x, y);
}

void CMainGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	mSceneManager->OnMouseMove(btnState, x, y);
}

void CMainGame::OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	mSceneManager->OnKeyBoardButtonDown(hWnd,msg,wparam,lparam);
}


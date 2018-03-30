#include "SceneManager.h"
#include"Common\d3dApp.h"


CSceneManager::CSceneManager()
{
	scene = new CTestScene();
}


CSceneManager::~CSceneManager()
{
	SafeDelete(scene);
}

bool CSceneManager::Init()
{
	scene->Init();
	return true;
}

void CSceneManager::UpdateScene(const float & dt, MSG & msg)
{
	scene->UpdateScene(dt, msg);
}

void CSceneManager::Draw()
{
	scene->Draw();
}

void CSceneManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	scene->OnMouseDown(btnState, x, y);
}

void CSceneManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	scene->OnMouseUp(btnState, x, y);
}

void CSceneManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	scene->OnMouseMove(btnState, x, y);
}

void CSceneManager::OnResize()
{
	scene->OnResize();
}

void CSceneManager::OnKeyboardButtonDown(const WPARAM & wParam)
{
	if (VK_ESCAPE == wParam)
		PostMessage(mhMainWnd,WM_DESTROY,0,0);
	scene->OnKeyboardButtonDown( wParam);
}

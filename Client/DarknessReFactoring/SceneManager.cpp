#include "SceneManager.h"


CSceneManager::CSceneManager()
{
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	scene = new CRoomScene();
}


CSceneManager::~CSceneManager()
{
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
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

void CSceneManager::OnKeyboardButtonDown(const WPARAM & wParam, UINT msg)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (VK_ESCAPE == wParam)
			PostMessage(mhMainWnd, WM_DESTROY, 0, 0);
		break;
	default:
		break;
	}
	scene->OnKeyboardButtonDown(wParam,msg);
}

#include "SceneManager.h"



CSceneManager::CSceneManager()
{
}
CSceneManager::~CSceneManager()
{
	for (auto p = mScenes.begin(); p != mScenes.end(); ++p)
	{
		delete p->second;
	}
}
bool CSceneManager::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView,
	ID3D11DepthStencilView * depthStencilView)
{
	md3dDevice = device;
	md3dImmediateContext = dc;
	mSwapChain = swapChain;
	mRenderTargetView = renderTargetView;
	mDepthStencilView = depthStencilView;
	/*
	2017 / 1 / 12 / 20:07
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:���ξ� ����.
	*/
	mSceneKey = SceneName::MainScene;
	mScenes.insert(make_pair(SceneName::MainScene, new CSceneMainGame));
	mScenes[mSceneKey]->Init(device, dc, swapChain, renderTargetView, depthStencilView);

	/*
	2017 / 1 / 12 / 20:07
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:TestScene����.
	*/

	mScenes.insert(make_pair(SceneName::test, new CBoxScene));
	mScenes[SceneName::test]->Init(device, dc, swapChain,
		renderTargetView, depthStencilView);

	return true;
}

void CSceneManager::UpdateScene(const float & dt)
{
	/*
	2017 / 1 / 12 / 22:06
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:�� ����.Test
	*/
	if (GetAsyncKeyState('C') & 0x8000) {
		mSceneKey = SceneName::MainScene;
		ChangeScene(SceneName::MainScene, dt);
	}


	mScenes[mSceneKey]->UpdateScene(dt);
}
void CSceneManager::Draw()
{
	mScenes[mSceneKey]->Draw();
}
void CSceneManager::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
	mScenes[mSceneKey]->OnMouseDown(btnState, x, y, mhMainWnd);
}
void CSceneManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	mScenes[mSceneKey]->OnMouseUp(btnState, x, y);
}
void CSceneManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	mScenes[mSceneKey]->OnMouseMove(btnState, x, y);
}
void CSceneManager::OnResize(const float & aspectRatio)
{
	//mScenes[mSceneKey]->OnResize(aspectRatio);
}

void CSceneManager::ChangeScene(std::string sceneName,const float& dt)
{
	mSceneKey = sceneName;
	mScenes[mSceneKey]->Init(md3dDevice, md3dImmediateContext, mSwapChain,
		mRenderTargetView, mDepthStencilView);
	//mScenes[mSceneKey]->UpdateScene(dt);
}
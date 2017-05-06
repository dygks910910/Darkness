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
// 	NetworkMgr::DestroyInstance();
// 	NetworkMgr::Release();
}
bool CSceneManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	D3D11_VIEWPORT* viewport, ID3D11DepthStencilView* dsv,
	const int& clientWidth, const int& clientHeight)
{
	/*
	2017 / 1 / 12 / 20:07
	작성자:박요한(dygks910910@daum.net)
	설명:메인씬 생성.
	*/
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	mRenderTargetView = renderTargetView;
	mViewport = viewport;
	mDsv = dsv;

	mSceneKey = SceneName::gameScene;
	mScenes.insert(make_pair(SceneName::gameScene, new CGameScene));
	mScenes[mSceneKey]->Init(device, dc, swapChain,
		*viewport, clientWidth, clientHeight);
	/*
	2017 / 1 / 12 / 20:07
	작성자:박요한(dygks910910@daum.net)
	설명:TestScene생성.
	*/

	// 	mScenes.insert(make_pair(SceneName::test, new CBoxScene));
	// 	mScenes[SceneName::test]->Init(device, dc,swapChain,renderTargetView
	// 	,  viewport);
	//OnResize();
	return true;
}
void CSceneManager::UpdateScene(const float dt, MSG& msg)
{

	/*
	2017 / 1 / 12 / 22:06
	작성자:박요한(dygks910910@daum.net)
	설명:씬 변경.Test
	*/
	/*if (GetAsyncKeyState('C') & 0x8000) {
	mSceneKey = SceneName::MainScene;
	ChangeScene(SceneName::MainScene, dt);
	}*/
// <<<<<<< HEAD
// 	mScenes[mSceneKey]->UpdateScene(dt);
// =======
	std::string changeSceneName = mScenes[mSceneKey]->UpdateScene(dt, msg); 
	if (changeSceneName != "")
	{
		ChangeScene(changeSceneName, dt);
	}
// >>>>>>> SpotLight
}
void CSceneManager::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* depthStencilView, D3D11_VIEWPORT* viewport)
{
	if(mScenes[mSceneKey] != nullptr)
	mScenes[mSceneKey]->Draw(renderTargetView, depthStencilView, viewport);
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
void CSceneManager::OnResize()
{
	if (mSceneKey != "")
		mScenes[mSceneKey]->OnResize();
}

void CSceneManager::ChangeScene(std::string sceneName, const float& dt)
{
	mSceneKey = sceneName;
	if (mScenes.find(mSceneKey) != mScenes.end())
	{
		//mScenes.insert(make_pair(sceneName, new CTestScene));
		mScenes[sceneName]->Init(mDevice,mDc,mSwapChain,*mViewport,mClientWidth,mClientHeight);
	}
	else
	{
		mScenes.insert(make_pair(sceneName, new CGameScene));
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);
	}
}

void CSceneManager::OnKeyBoardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(mSceneKey!= "")
	mScenes[mSceneKey]->OnKeyboardButtonDown(hWnd, msg,wparam, lparam);
}




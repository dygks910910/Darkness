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

	mSceneKey = SceneName::mainScene;
	mScenes.insert(make_pair(SceneName::mainScene, new CMainScene));

	//mSceneKey = SceneName::gameScene;
	//mScenes.insert(make_pair(SceneName::gameScene, new CGameScene));

	mScenes[mSceneKey]->Init(device, dc, swapChain,
		*viewport, clientWidth, clientHeight);
	return true;
}
void CSceneManager::UpdateScene(const float dt, MSG& msg)
{

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
	mScenes[mSceneKey]->Draw(device,dc,swapChain,renderTargetView, depthStencilView, viewport);
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
	for (auto p = mScenes.begin(); p != mScenes.end(); ++p)
	{
		delete p->second;
	}
	mScenes.clear();
	if (mScenes.find(mSceneKey) != mScenes.end())
	{
		if (mSceneKey == SceneName::mainScene)
		{
			mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);
			CModelManager::GetInstance()->DestroyInstance();
		}
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);
	}
	else if (mSceneKey == SceneName::gameScene)
	{
		
		mScenes.insert(make_pair(sceneName, new CGameScene));
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);
	}
	else if (mSceneKey == SceneName::endingScene)
	{
		mScenes.insert(make_pair(sceneName, new CEndingScene));
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);

	}
	else if (mSceneKey == SceneName::roomScene)
	{
		mScenes.insert(make_pair(sceneName, new CRoomScene));
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);

	}
	else if (mSceneKey == SceneName::mainScene)
	{
		mScenes.insert(make_pair(sceneName, new CMainScene));
		mScenes[sceneName]->Init(mDevice, mDc, mSwapChain, *mViewport, mClientWidth, mClientHeight);

	}
	else
		return;

}

void CSceneManager::OnKeyBoardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(mSceneKey!= "")
	mScenes[mSceneKey]->OnKeyboardButtonDown(hWnd, msg,wparam, lparam);
}

void CSceneManager::UpdateD3DValue(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView, 
	D3D11_VIEWPORT * viewport, ID3D11DepthStencilView * dsv)
{
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	mRenderTargetView = renderTargetView;
	mViewport = viewport;
	mDsv = dsv;

}




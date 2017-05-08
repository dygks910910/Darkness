#pragma once
#include"GameScene.h"
#include<map>
#include<string.h>
#include "MainScene.h"
#include "EndingScene.h"
class CSceneManager
{
private:
	//////////////////////////////////////////////////////////////////////////
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDc;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	D3D11_VIEWPORT* mViewport;
	ID3D11DepthStencilView* mDsv;
	int mClientWidth;
	int mClientHeight;
	//////////////////////////////////////////////////////////////////////////
	/*
	2017 / 1 / 12 / 20:14
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:Scene���� ��Ƴ��� �����̳�
	*/
	std::map<std::string, CScene*> mScenes;
	/*
	2017 / 1 / 12 / 20:14
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:Scene��ȯ�� ���� Ű.
	*/
	std::string mSceneKey;
	
public:
	CSceneManager();
	~CSceneManager();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		D3D11_VIEWPORT* viewport, ID3D11DepthStencilView* dsv, const int& clientWidth, const int& clientHeight);
	void UpdateScene(const float dt, MSG& msg);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView, D3D11_VIEWPORT* viewport);
	void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void OnResize();
	void ChangeScene(std::string sceneName, const float& dt);
	void SetSceneKey(const std::string& sceneName) { mSceneKey = sceneName; }
	void OnKeyBoardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	std::string GetSceneKey()const { return mSceneKey; }
	void UpdateD3DValue(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		D3D11_VIEWPORT* viewport, ID3D11DepthStencilView* dsv);
private:

};

#pragma once
#include "BoxScene.h"
#include "SceneMainGame.h"
#include "BoxScene.h"
#include<map>
#include<string.h>
class CSceneManager
{
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	
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
		ID3D11DepthStencilView* depthStencilView);
	void UpdateScene(const float& dt);
	void Draw();
	void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnResize(const float& aspectRatio);
	void ChangeScene(std::string sceneName, const float& dt);
};

namespace SceneName
{
	const std::string MainScene = "MainGameScene";
	const std::string test = "test";

}
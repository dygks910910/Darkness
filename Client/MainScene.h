#pragma once
#include "Scene.h"
#include "BitMap.h"
class CMainScene :
	public CScene
{
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDc;
	IDXGISwapChain* mSwapChain;
	Camera mCam;

	XMMATRIX mWorldMtx;
	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11DepthStencilState* mDepthStencilState;

	CBitMap mConnectButton;
	CBitMap mConnectCursorOnButton;
	CBitMap mConnectClickedButton;

	CBitMap mExitButton;
	CBitMap mExitCursorButton;
	CBitMap mExitClickedButton;

	CBitMap mBackgroundPicture;

public:
	CMainScene();
	virtual ~CMainScene();

	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual void UpdateScene(const float& dt);
	virtual void Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};
	virtual void OnResize(const float& aspectRatio);
};

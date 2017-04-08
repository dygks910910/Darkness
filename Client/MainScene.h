#pragma once
#include "Scene.h"
#include "ButtonClass.h"
class CMainScene :
	public CScene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11DepthStencilState* mDepthStencilState;
	//////////////////////////////////////////////////////////////////////////
	//메인 화면
	CButtonClass mConnectButton;
	CButtonClass mExitButton;
	CBitMap mBackgroundPicture;
	CBitMap mLogo;

	//////////////////////////////////////////////////////////////////////////
	///inputBoard;
	CBitMap mInputBoard;
	CBitMap mInputIP;
	CBitMap mInputPort;
	CBitMap mInputNickname;
	CButtonClass mLobbyConnectButton;
	CButtonClass mReturnButton;

	bool bActivedInputBoard;


public:
	CMainScene();
	virtual ~CMainScene();

	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual void UpdateScene(const float& dt);
	virtual void Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize();
};


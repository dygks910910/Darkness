#pragma once
#include "NetworkMgr.h"
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "DrawText.h"
#include "GameTimer.h"

class CMainScene :
	public CScene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11DepthStencilState* mDepthStencilState;
	//////////////////////////////////////////////////////////////////////////
	//로고화면
	CBitMap mMainLogo;
	GameTimer mTimeForLogo;
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

	std::wstring mIpString;
	std::wstring mPortString;
	std::wstring mNicknameString;

	bool bActivedInputBoard;
	bool m_bFocusOnIP;
	bool m_bFocusOnPort;
	bool m_bFocusOnNickName;


	wchar_t Text[255];     // 텍스트를 저장하기위한 변수
	wchar_t Cstr[10];      // 조합중인 문자!!
	const int CONNECT_BUTTON_X = 1000;
	const int CONNECT_BUTTON_Y = 400;

	const int BUTTON_SIZE_X = 200;
	const int BUTTON_SIZE_Y = 100;

	const int EXIT_BUTTON_Y = 500;
	const int RETURN_BUTTON_X = 870;
	const int RETURN_BUTTON_Y = 600;

	const int LOBBY_CONNECT_BUTTON_X = 520;
	const int LOBBY_CONNECT_BUTTON_Y = 600;

	const int INPUT_BOARD_X = 420;
	const int INPUT_BOARD_Y = 100;

	const int INPUT_IP_X = 420;
	const int INPUT_IP_Y = 150;
	const int INPUT_BAR_WIDTH = 700;
	const int INPUT_BAR_HEIGHT = 150;
	const int INPUT_PORT_X = 420;
	const int INPUT_PORT_Y = 300;

	const int INPUT_NICKNAME_X = 420;
	const int INPUT_NICKNAME_Y = 450;

	const int FONT_SIZE = 40;
	//////////////////////////////////////////////////////////////////////////
	//LogoFullScreen
	bool m_bLogoTime;

	//////////////////////////////////////////////////////////////////////////
	//Lobby
	CDrawText DrawText;

public:
	CMainScene();
	virtual ~CMainScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual std::string UpdateScene(const float dt, MSG& msg);
	virtual void Draw(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize();
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	int GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	
};


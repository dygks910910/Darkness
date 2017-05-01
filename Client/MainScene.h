#pragma once
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "DrawText.h"

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

	std::wstring mIpString;
	std::wstring mPortString;
	std::wstring mNicknameString;

	bool bActivedInputBoard;
	bool m_bFocusOnIP;
	bool m_bFocusOnPort;
	bool m_bFocusOnNickName;

// 	IFW1Factory* m_factory;
// 	IFW1FontWrapper* mFontWrapper;

	wchar_t Text[255];     // 텍스트를 저장하기위한 변수
	wchar_t Cstr[10];      // 조합중인 문자!!
	const int CONNECT_BUTTON_X = 1000;
	const int CONNECT_BUTTON_Y = 450;
	const int EXIT_BUTTON_Y = 500;
	const int RETURN_BUTTON_X = 900;
	const int RETURN_BUTTON_Y = 650;
	const int LOBBY_CONNECT_BUTTON_X = 550;
	const int LOBBY_CONNECT_BUTTON_Y = 650;
	const int INPUT_BOARD_X = 400;
	const int INPUT_BOARD_Y = 200;
	const int INPUT_IP_X = 650;
	const int INPUT_IP_Y = 450;
	const int INPUT_PORT_X = 650;
	const int INPUT_PORT_Y = 500;
	const int INPUT_NICKNAME_X = 650;
	const int INPUT_NICKNAME_Y = 550;
	const int FONT_SIZE = 20;

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
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	int GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//함수객체
	CDrawText DrawText;
// 	wchar_t* ConverCtoWC(char* str)
// 	{
// 		//wchar_t형 변수 선언
// 		wchar_t* pStr;
// 		//멀티 바이트 크기 계산 길이 반환
// 		int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
// 		//wchar_t 메모리 할당
// 		pStr = new WCHAR[strSize];
// 		//형 변환
// 		MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
// 		return pStr;
// 	}
};


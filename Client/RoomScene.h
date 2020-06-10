#pragma once
#include "NetworkMgr.h"
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "Common/xnacollision.h"
#include "DrawText.h"
#include "Common/GameTimer.h"
#include "Sound.h"


const int CS_PACKET_CREATER_START_CLICK = 8;

struct cs_packet_start_button_click
{
	BYTE size;
	BYTE type;
};

class CRoomScene :
	public CScene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	// 	ID3D11DepthStencilState* mDepthDisableState;
	// 	ID3D11DepthStencilState* mDepthStencilState;
	//////////////////////////////////////////////////////////////////////////
	//로고화면
	CBitMap mMainLogo;
// 	GameTimer mTimeForLogo;
	//////////////////////////////////////////////////////////////////////////
	//메인 화면
// 	CButtonClass mConnectButton;
// 	CButtonClass mExitButton;
	CBitMap mBackgroundPicture;
	CBitMap mKingLogo;

	CButtonClass mStartButton;


	//닉네임입력
	CDrawText mDrawText;
	std::wstring nickname;

	//////////////////////////////////////////////////////////////////////////
	///inputBoard;
// 	CBitMap mInputBoard;
// 	CBitMap mInputIP;
// 	CBitMap mInputPort;
// 	CBitMap mInputNickname;
// 	CButtonClass mLobbyConnectButton;
// 	CButtonClass mReturnButton;

// 	std::wstring mIpString;
// 	std::wstring mPortString;
	std::wstring mNicknameString;

	bool bActivedInputBoard;
	bool m_bFocusOnIP;
	bool m_bFocusOnPort;
	bool m_bFocusOnNickName;

// 
// 	const int LOGO_X = 100;
// 	const int LOGO_Y = -70;
// 
// 	wchar_t Text[255];     // 텍스트를 저장하기위한 변수
// 	wchar_t Cstr[10];      // 조합중인 문자!!
// 						   //x 0.8 , y 1.13

	//닉네임
	const int PLAYER1_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER1_NICKNAME_LOCATION_Y = 85;

	const int PLAYER2_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER2_NICKNAME_LOCATION_Y = 170;

	const int PLAYER3_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER3_NICKNAME_LOCATION_Y = 245;

	const int PLAYER4_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER4_NICKNAME_LOCATION_Y = 320;

	const int PLAYER5_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER5_NICKNAME_LOCATION_Y = 400;

	const int PLAYER6_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER6_NICKNAME_LOCATION_Y = 475;

	const int PLAYER7_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER7_NICKNAME_LOCATION_Y = 550;

	const int PLAYER8_NICKNAME_LOCATION_X = 390 * 0.8f;
	const int PLAYER8_NICKNAME_LOCATION_Y = 630;

	const int OUTPUT_Y_OFFSET = 80;

	//스타트버튼
	const int START_BUTTON_X = 930 * 0.8f;
	const int START_BUTTON_Y = 500 * 1.13777f;

	const int BUTTON_SIZE_X = 580 * 0.8f;
	const int BUTTON_SIZE_Y = 160 * 1.13777f;

	//킹로고
	const int KINGLOGO_SIZE_X = 50 * 0.8f;
	const int KINGLOGO_SIZE_Y = 30 * 1.13777f;

	const int KINGLOGO_LOCATION_X = 150 * 0.8f;
	const int KINGLOGO_LOCATION_Y = 180 * 0.9f;

	//화살표버튼 맨왼쪽
	const int LEFT1_BUTTON_X = 970 * 0.8f;
	const int LEFT1_BUTTON_Y = 400 * 1.13777f;

	const int LEFT1_BUTTON_SIZE_X = 50 * 0.8f;
	const int LEFT1_BUTTON_SIZE_Y = 40 * 1.13777f;

	//화살표버튼 맨왼쪽에서 두번째
	const int RIGHT1_BUTTON_X = 1120 * 0.8f;
	const int RIGHT1_BUTTON_Y = 400 * 1.13777f;

	const int RIGHT1_BUTTON_SIZE_X = 50 * 0.8f;
	const int RIGHT1_BUTTON_SIZE_Y = 40 * 1.13777f;

	//화살표버튼 맨왼쪽에서 세번째
	const int LEFT2_BUTTON_X = 1280 * 0.8f;
	const int LEFT2_BUTTON_Y = 400 * 1.13777f;

	const int LEFT2_BUTTON_SIZE_X = 50 * 0.8f;
	const int LEFT2_BUTTON_SIZE_Y = 40 * 1.13777f;

	//화살표버튼 맨오른쪽
	const int RIGHT2_BUTTON_X = 1430 * 0.8f;
	const int RIGHT2_BUTTON_Y = 400 * 1.13777f;

	const int RIGHT2_BUTTON_SIZE_X = 50 * 0.8f;
	const int RIGHT2_BUTTON_SIZE_Y = 40 * 1.13777f;

	const int FONT_SIZE = 40;
	//////////////////////////////////////////////////////////////////////////
	//LogoFullScreen
	bool m_bLogoTime;

	//////////////////////////////////////////////////////////////////////////
	bool m_bStartCheck= false;


	//Lobby
	CDrawText DrawText;

	//sendbuf
	BYTE   send_buf1[MAX_BUF_SIZE];
	WSABUF   send_wsa_buf1;

public:
	CRoomScene();
	virtual ~CRoomScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual std::string UpdateScene(const float dt, MSG& msg);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize();
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//int GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

};


#pragma once
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "FW1FontWrapper.h"
#include<tchar.h>
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
// 	IFW1Factory* m_factory;
// 	IFW1FontWrapper* mFontWrapper;


	wchar_t Text[255];     // 텍스트를 저장하기위한 변수
	wchar_t Cstr[10];      // 조합중인 문자!!

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

	void Pick(const int& sx, const int& sy,CButtonClass& button);
	void drawText(ID3D11Device *pDevice, ID3D11DeviceContext *pContext,std::wstring text);

	
	//  	char getKey()
//  	{
//  		if (kbhit()) // kbhit()이용해 입력값이 있는지 확인 
//  		{
//  			char a = getch();
//  			return a;     // 입력값이 getch()로 char를 리턴해줌
//  		}
//  		return '\0'; // 입력값이 없으면 널 문자 리턴
//  	}
// 	wchar_t ConverCtoWC(char str)
// 	{
// 		//wchar_t형 변수 선언
// 		wchar_t pStr;
// 		//멀티 바이트 크기 계산 길이 반환
// 		int strSize = MultiByteToWideChar(CP_ACP, 0, &str, -1, NULL, NULL);
// 		//wchar_t 메모리 할당
// 		//형 변환
// 		MultiByteToWideChar(CP_ACP, 0, &str, strlen(&str) + 1, &pStr, strSize);
// 		return pStr;
// 	}


};


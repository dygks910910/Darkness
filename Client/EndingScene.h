#pragma once
#include "NetworkMgr.h"
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "DrawText.h"
#include "GameTimer.h"

class CEndingScene :
	public CScene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;


	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11DepthStencilState* mDepthStencilState;

	//////////////////////////////////////////////////////////////////////////
	CBitMap mResultBoard;
	wchar_t Text[255];     // 텍스트를 저장하기위한 변수
	wchar_t Cstr[10];      // 조합중인 문자!!

	//////////////////////////////////////////////////////////////////////////
	//Lobby
	CDrawText DrawText;

	// 첫번쨰 닉네임 위치(431, 210)
	// 두번쨰 (431,313)
	const int OUTPUT_NICKNAME_LOCATION_X = 430;
	const int OUTPUT_NICKNAME_LOCATION_Y = 210;

	const int OUTPUT_PLAYER_KILL_LOCATION_X = 921;
	const int OUTPUT_PLAYER_KILL_LOCATION_Y = 210;

	const int OUTPUT_MONSTER_KILL_LOCATION_X = 1397;
	const int OUTPUT_MONSTER_KILL_LOCATION_Y = 210;

	const int OUTPUT_Y_OFFSET = 100;

	//플레이어 킬 위치
	// 첫번째 (921,210)

	//몬스터 킬 위치 (1397,210)

public:
	CEndingScene();
	virtual ~CEndingScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual std::string UpdateScene(const float dt, MSG& msg);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv, 
		ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize();
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	int GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	
	
	
};


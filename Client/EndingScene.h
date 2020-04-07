#pragma once
#include "NetworkMgr.h"
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "Common/xnacollision.h"
#include "DrawText.h"
#include "Common/GameTimer.h"

struct playerScore;

class CEndingScene :
	public CScene
{
protected:
	
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	std::vector<playerScore> mScores;

	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11DepthStencilState* mDepthStencilState;

	//////////////////////////////////////////////////////////////////////////
	CBitMap mResultBoard;
	CButtonClass mHomeButton;

	//////////////////////////////////////////////////////////////////////////
	//Lobby
	CDrawText mDrawText;

	// 첫번쨰 닉네임 위치(431, 210)
	// 두번쨰 (431,313)
	const int OUTPUT_NICKNAME_LOCATION_X = 430*0.8f;
	const int OUTPUT_NICKNAME_LOCATION_Y = 170;

	const int OUTPUT_PLAYER_KILL_LOCATION_X = 921 * 0.8f;
	const int OUTPUT_PLAYER_KILL_LOCATION_Y = 170;

	const int OUTPUT_MONSTER_KILL_LOCATION_X = 1397 * 0.8f;
	const int OUTPUT_MONSTER_KILL_LOCATION_Y = 170;

	const int OUTPUT_Y_OFFSET = 80;

	const int HOME_BUTTON_X = 1000;
	const int HOME_BUTTON_Y = 10;

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
	void DrawAllScore();
	
	
};

struct playerScore
{
	std::wstring nickname;
	UINT playerKill;
	UINT monsterKill;
	UINT NIckposX, NickposY;
	UINT PKposX, PKposY;
	UINT MKposX, MKkposY;
	UINT32 fontColor;
};
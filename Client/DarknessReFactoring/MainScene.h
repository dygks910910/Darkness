#pragma once
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "DrawText.h"
#include "GameTimer.h"
#include"Common\Camera.h"
#include"Global.h"
#include"RenderStates.h"
#include"resource.h"
#include<fstream>

class CMainScene :
	public Scene
{
	const int LOGO_TIME_SEC = 4;
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	//////////////////////////////////////////////////////////////////////////
	//��Ʈ��
	CBitMap mMainLogo;
	bool isLogoTime = true;
	CBitMap mBackground;
	//////////////////////////////////////////////////////////////////////////
	//��ư
	CButtonClass playButton;
	CButtonClass mExitButton;


	//////////////////////////////////////////////////////////////////////////
	///inputBoard;
public:
	CMainScene();
	virtual ~CMainScene();
	
	virtual int UpdateScene(const float dt, MSG& msg);
	
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	// Scene��(��) ���� ��ӵ�
	virtual bool Init() override;
	virtual void OnKeyboardButtonDown(WPARAM wParam, UINT msg) override;
	virtual void OnResize()override;
	virtual void Draw() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	BOOL  MyDlgProcForInputNickname(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


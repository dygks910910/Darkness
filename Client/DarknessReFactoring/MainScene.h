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
#include<fstream>
#define IPADDRESS "127.0.0.1"
//#define IPADDRESS "127.0.0.1"
//#define IPADDRESS "172.20.10.4"

class CMainScene :
	public Scene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	//////////////////////////////////////////////////////////////////////////
	//로고화면
	CBitMap mMainLogo;
	//////////////////////////////////////////////////////////////////////////
	//메인 화면

	//////////////////////////////////////////////////////////////////////////
	///inputBoard;
public:
	CMainScene();
	virtual ~CMainScene();
	
	virtual std::string UpdateScene(const float dt, MSG& msg);
	
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	// Scene을(를) 통해 상속됨
	virtual bool Init() override;
	virtual void OnKeyboardButtonDown(WPARAM wParam, UINT msg) override;
	virtual void OnResize()override;
	virtual void Draw() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;

};


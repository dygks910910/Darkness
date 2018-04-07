#pragma once
#include<conio.h>
#include "Scene.h"
#include "ButtonClass.h"
#include "xnacollision.h"
#include "DrawText.h"
#include "GameTimer.h"
#include"Global.h"
#include"RenderStates.h"

class CRoomScene :
	public Scene
{
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;
	//���� ȭ��
	CBitMap mBackgroundPicture;
	CBitMap mKingLogo;
	CButtonClass starButton;
	//�г����Է�
	CDrawText mDrawText;
	int FONT_SIZE = mClientWidth/50;
public:
	CRoomScene();
	virtual ~CRoomScene();
	// Scene��(��) ���� ��ӵ�
	virtual bool Init() override;

	virtual void OnKeyboardButtonDown(WPARAM wParam, UINT msg) override;

	virtual int UpdateScene(const float dt, MSG & msg) override;

	virtual void Draw() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;

	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;

	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	virtual void OnResize() override;

};


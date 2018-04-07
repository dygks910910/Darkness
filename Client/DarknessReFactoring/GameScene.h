#pragma once
#include "Scene.h"
#include"ParticleSystem.h"
#include "Camera.h"
#include "MiniMap.h"
#include "Sky.h"
#include "DrawText.h"
#include"TextureMgr.h"
#include"ModelManager.h"
class GameScene :
	public Scene
{
private:
	TextureMgr mTexMgr;
	Camera mCam;
	XMFLOAT4X4 mWorldMtx;


	DirectionalLight mDirLights[3];
	XMFLOAT3 mOriginalLightDir[3];
	CMiniMap mMinimap;
	Sky* mSky;
	CDrawText mDrawText;
	XMFLOAT4X4 mShadowTransform;



public:
	GameScene();
	virtual ~GameScene();

	// Scene을(를) 통해 상속됨
	virtual bool Init() override;
	virtual void OnKeyboardButtonDown(WPARAM wParam, UINT msg) override;
	virtual int UpdateScene(const float dt, MSG & msg) override;
	virtual void Draw() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnResize() override;
};


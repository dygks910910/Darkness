#pragma once
#include "Scene.h"
#include "GameTimer.h"
#include "MiniMap.h"
#include "xnacollision.h"
#include "Crown.h"
/*
2017 / 1 / 10 / 6:04
작성자:박요한(dygks910910@daum.net)
설명:메인 게임씬.
*/
class CSceneMainGame :
	public CScene
{
	//미니맵
	CMiniMap mMiniMap;
	Sky* mSky;
	//지형
	Terrain mTerrain;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisableState;
	ID3D11ShaderResourceView* mRandomTexSRV;
	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11Buffer* mInstancedBuffer;

	ParticleSystem mFire;
	ParticleSystem mRain;
	//기본조명.
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	Camera mCam;
	XNA::Frustum mCamFrustum;
	CBox mBox;
	CCrown mCrawler;
	bool mWalkCamMode;

	POINT mLastMousePos;
public:
	CSceneMainGame();
	~CSceneMainGame();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain,ID3D11RenderTargetView* renderTargetView);
	virtual void UpdateScene(const float& dt);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);
};


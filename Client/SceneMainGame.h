#pragma once
#include "Scene.h"
#include "GameTimer.h"
#include "minimapclass.h"
/*
2017 / 1 / 10 / 6:04
작성자:박요한(dygks910910@daum.net)
설명:메인 게임씬.
*/
class CSceneMainGame :
	public CScene
{
	//미니맵
	MiniMapClass mMiniMap;
	Sky* mSky;
	//지형
	Terrain mTerrain;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisableState;
	D3D11_VIEWPORT mMinimapViewport;
	D3D11_VIEWPORT* mMainGameViewport;

/*
	ParticleSystem mFire;
	ParticleSystem mRain;*/
	//기본조명.
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	Camera mCam;
	CBox mBox;
	CBuilding mCrawler;
	bool mWalkCamMode;

	POINT mLastMousePos;
public:
	CSceneMainGame();
	~CSceneMainGame();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain,ID3D11RenderTargetView* renderTargetView
		, D3D11_VIEWPORT* viewPort);
	virtual void UpdateScene(const float& dt);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);
	void DrawScene(const Camera & camera, bool drawCenterSphere);
};


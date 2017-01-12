#pragma once
#include "Scene.h"
#include "GameTimer.h"
/*
2017 / 1 / 10 / 6:04
ÀÛ¼ºÀÚ:¹Ú¿äÇÑ(dygks910910@daum.net)
¼³¸í:¸ÞÀÎ °ÔÀÓ¾À.
*/
class CSceneMainGame :
	public CScene
{

	Sky* mSky;
	//ÁöÇü
	Terrain mTerrain;
	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ParticleSystem mFire;
	ParticleSystem mRain;
	//±âº»Á¶¸í.
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	Camera mCam;
	//¹Ì´Ï¸Ê Ä«¸Þ¶ó
	Camera mMinimapCam;
	//testCube
	CBox mBox;
	//testBuilding
	CBuilding mBuilding;

	bool mWalkCamMode;

	POINT mLastMousePos;
	//D3D11_VIEWPORT mMinimapViewport;
	//D3D11_VIEWPORT* mMainGameViewport;
	////¹Ì´Ï¸Ê ·»´õÅ¸°Ùºä
	//ID3D11RenderTargetView* mRenderTV;
	////¹Ì´Ï¸Ê µö½ºÅÙ½Ç ºä.
	//ID3D11DepthStencilView* mDepstencilV;
	//ID3D11Texture2D* mMinimapDSBuffer;
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
};


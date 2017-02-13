#pragma once
#include"Scene.h"
#include "GameTimer.h"
class CBoxScene:
	public CScene

{

	Sky* mSky;
	//지형
	Terrain mTerrain;
	//ID3D11ShaderResourceView* mFlareTexSRV;
	//ID3D11ShaderResourceView* mRainTexSRV;
	//ID3D11ShaderResourceView* mRandomTexSRV;

	//ParticleSystem mFire;
	//ParticleSystem mRain;
	//기본조명.
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	Camera mCam;
	//testCube
	CBox mBox;
	//testBuilding
	//CBuilding mBuilding;

	bool mWalkCamMode;
	POINT mLastMousePos;
public:
	CBoxScene();
	~CBoxScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView);
	virtual void UpdateScene(const float& dt);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);
};


#pragma once
#include "Scene.h"
#include "GameTimer.h"
#include "MiniMap.h"
/*
2017 / 1 / 10 / 6:04
�ۼ���:�ڿ���(dygks910910@daum.net)
����:���� ���Ӿ�.
*/
class CSceneMainGame :
	public CScene
{
	//�̴ϸ�
	CMiniMap mMiniMap;
	Sky* mSky;
	//����
	Terrain mTerrain;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisableState;
	D3D11_VIEWPORT mMinimapViewport;
	D3D11_VIEWPORT* mMainGameViewport;

/*
	ParticleSystem mFire;
	ParticleSystem mRain;*/
	//�⺻����.
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


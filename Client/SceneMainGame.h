#pragma once
#include "Scene.h"
#include "GameTimer.h"
/*
2017 / 1 / 10 / 6:04
�ۼ���:�ڿ���(dygks910910@daum.net)
����:���� ���Ӿ�.
*/
class CSceneMainGame :
	public CScene
{

	Sky* mSky;
	//����
	Terrain mTerrain;
	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ParticleSystem mFire;
	ParticleSystem mRain;
	//�⺻����.
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	Camera mCam;
	//�̴ϸ� ī�޶�
	Camera mMinimapCam;
	//testCube
	CBox mBox;
	//testBuilding
	CBuilding mBuilding;

	bool mWalkCamMode;

	POINT mLastMousePos;
	//D3D11_VIEWPORT mMinimapViewport;
	//D3D11_VIEWPORT* mMainGameViewport;
	////�̴ϸ� ����Ÿ�ٺ�
	//ID3D11RenderTargetView* mRenderTV;
	////�̴ϸ� �����ٽ� ��.
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


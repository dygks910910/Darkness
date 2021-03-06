#pragma once
#include "NetworkMgr.h"
#include "Scene.h"
#include "Common/Camera.h"
#include "Common/GameTimer.h"
#include<fstream>
#include "Common/GeometryGenerator.h"
#include "Cordinate.h"
#include "Common/TextureMgr.h"
#include "ShadowMap.h"
#include "FbxLoader.h"
#include "RenderStates.h"
#include "Common/xnacollision.h"
#include "Model.h"
#include "ModelManager.h"
#include "ParticleSystem.h"
#include "SkinnedModel.h"
#include "Ssao.h"
#include "MiniMap.h"
#include "DrawText.h"
#include "Sound.h"

const int CS_PACKET_CLIENT_DRAW_START = 7;


struct cs_packet_draw_start
{
	BYTE size;
	BYTE type;
};



struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

class CGameScene :
	public CScene
{
#define GET_SKINED_INSTANCE_MODELS	CModelManager::GetInstance()->GetSkinnedInstanceModels
#define GET_NETWORK_ID NetworkMgr::GetInstance()->getId
	ID3D11Device* mDevice;
	/*
	2017 / 3 / 22 / 16:13
	작성자:박요한(dygks910910@daum.net)
	설명:기본적인 dx11 인터페이스 변수들.여기서 Release해주면 안된다.프로그램 종료시 release하도록
	세팅해놨음.
	*/
	//////////////////////////////////////////////////////////////////////////
	CBitMap mLoadingScene;

	XMFLOAT4X4 mWorldMtx;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisableState;

	TextureMgr mTexMgr;
	GameTimer mTimer;
	float mfEndingTimerCount = 0;
	float mfCountDownMin =8;
	float mfCountDownSec = 0;
	std::wstring timerString;
	//RainParticle
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ID3D11ShaderResourceView* mFlareSRV;
	ParticleSystem mRain;

	ID3D11ShaderResourceView* mLightPillarSRV;
	std::vector<ParticleSystem*> mvFlare;
	std::vector<ParticleSystem*> mvLightPillar;
	//////////////////////////////////////////////////////////////////////////
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	XMFLOAT3 mOriginalLightDir[3];
	CMiniMap mMinimap;
	
	POINT mLastMousePos;
	POINT mUpMousePos;

	Sky* mSky;

	CDrawText mDrawText;
	BoundingSphere mSceneBounds;

	float mLightRotationAngle;


	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;
	ShadowMap* mSmap;
	Ssao* mSsao;
	bool mbTimeOver = false;

	PointLight mPointLight;
	//true 는 증가 , false 는 감소
	bool m_bSizeSwitch = false;
	SpotLight mSpotLight;
public:
	CGameScene();
	virtual ~CGameScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual std::string UpdateScene(const float dt, MSG& msg);
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);

	virtual void OnResize();
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {};

	void DrawSceneToShadowMap(ID3D11DeviceContext* dc);
	void BuildShadowTransform();

	bool yamee = false;
	void CameraRatateForWinner();
	void CreateDepthStencilState(ID3D11Device* device);
	BYTE   send_buf1[MAX_BUF_SIZE];
	WSABUF   send_wsa_buf1;
};

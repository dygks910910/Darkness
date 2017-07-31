#pragma once
#include "NetworkMgr.h"
#include "Scene.h"
#include "Camera.h"
#include "GameTimer.h"
#include<fstream>
#include "GeometryGenerator.h"
#include "Cordinate.h"
#include "TextureMgr.h"
#include "ShadowMap.h"
#include "FbxLoader.h"
#include "RenderStates.h"
#include "xnacollision.h"
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

	/*
	2017 / 3 / 22 / 16:13
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:�⺻���� dx11 �������̽� ������.���⼭ Release���ָ� �ȵȴ�.���α׷� ����� release�ϵ���
	�����س���.
	*/
	//////////////////////////////////////////////////////////////////////////
	CBitMap mLoadingScene;

	XMFLOAT4X4 mWorldMtx;

	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisableState;

	TextureMgr mTexMgr;
	GameTimer mTimer;
	float countDownMin =8;
	float countDownSec = 0;
	std::wstring timerString;
	//RainParticle
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
	ParticleSystem mRain;
	//////////////////////////////////////////////////////////////////////////
// 	CCordinate mCordWorld;
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	XMFLOAT3 mOriginalLightDir[3];
	CMiniMap mMinimap;
	///////////////////////////////Skined Model////////////////////////////////
	// 	SkinnedModel* mCharacterModel;
	// 	SkinnedModelInstance mCharacterInstance1;
	// 	SkinnedModelInstance mCharacterInstance2;
	// 	SkinnedModelInstance mCharacterInstances[70];
	//Terrain mTerrain;
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

	BYTE   send_buf1[MAX_BUF_SIZE];
	WSABUF   send_wsa_buf1;
};

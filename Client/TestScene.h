#pragma once
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
struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

class CTestScene :
	public CScene
{
	/*
	2017 / 3 / 22 / 16:13
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:�⺻���� dx11 �������̽� ������.���⼭ Release���ָ� �ȵȴ�.���α׷� ����� release�ϵ��� 
	�����س���.
	*/
	

	int mClientHeight;
	int mClientWidth;

//////////////////////////////////////////////////////////////////////////
 	TextureMgr mTexMgr;
	CModelManager mModelMgr;
	GameTimer mTimer;
	//RainParticle
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
	ParticleSystem mRain;
	//////////////////////////////////////////////////////////////////////////
	CCordinate mCordWorld;
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	XMFLOAT3 mOriginalLightDir[3];

	///////////////////////////////Skined Model////////////////////////////////
	SkinnedModel* mCharacterModel;
	SkinnedModelInstance mCharacterInstance1;
	SkinnedModelInstance mCharacterInstance2;
	SkinnedModelInstance mCharacterInstances[70];
	//Terrain mTerrain;
	POINT mLastMousePos;
	Sky* mSky;


	BoundingSphere mSceneBounds;

	float mLightRotationAngle;

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;
	ShadowMap* mSmap;
	Ssao* mSsao;

// 	ID3D11Buffer* mScreenQuadVB;
// 	ID3D11Buffer* mScreenQuadIB;
public:
	CTestScene();
	virtual ~CTestScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual void UpdateScene(const float dt) ;
	virtual void Draw(ID3D11RenderTargetView* rtv,ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort) ;
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) ;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize();

	void DrawSceneToShadowMap();
	void BuildShadowTransform();
	void DrawSceneToSsaoNormalDepthMap();
	int total = 0;

	int mAnimTotalCnt[4];
	std::map < std::string, std::vector<XMFLOAT4X4>*> mclipAnimbuf;
	std::string mClipname[4] = { "Idle", "Walk", "Attack1", "Run" };
};

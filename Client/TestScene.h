#pragma once
#include "Scene.h"
#include "Camera.h"
#include "GameTimer.h"
#include<fstream>
#include "GeometryGenerator.h"
#include "StaticObject.h"
#include "Box.h"
#include "Crown.h"
#include "Cordinate.h"
#include "Plane.h"
#include "ModelMgr.h"
#include "Model.h"
#include "TextureMgr.h"
#include "FBXObject.h"
#include "ShadowMap.h"
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
	TextureMgr mTextureMgr;
	CModelMgr mModelMgr;
	CCordinate mCordWorld;
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	D3D11_VIEWPORT mScreenViewport;
	//Terrain mTerrain;
	POINT mLastMousePos;
	Sky* mSky;
	std::vector<CDynamicObject*> mvDynamicObject;
	std::vector<CStaticObject*> mvStaticObject;
	// for Frustum culling
	XNA::Frustum mCamFrustum;
	BoundingSphere mSceneBounds;


	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;
	//Ssao* mSsao;
	//ShadowMap* mSmap;
public:
	CTestScene();
	virtual ~CTestScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView, const D3D11_VIEWPORT& viewPort);
	virtual void UpdateScene(const float& dt) ;
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView) ;
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) ;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);
	void DrawSceneToShadowMap(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView);
	void BuildShadowTransform();
	void DrawSceneToSsaoNormalDepthMap(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView);
};

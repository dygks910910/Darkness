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
class CTestScene :
	public CScene
{
	TextureMgr mTextureMgr;
	CModelMgr mModelMgr;
	CCordinate mCordWorld;
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	GameTimer mTimer;
	//Terrain mTerrain;
	POINT mLastMousePos;
	Sky* mSky;
	std::vector<CDynamicObject*> mvDynamicObject;
	std::vector<CStaticObject*> mvStaticObject;
	// for Frustum culling
	XNA::Frustum mCamFrustum;
public:
	CTestScene();
	virtual ~CTestScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView);
	virtual void UpdateScene(const float& dt) ;
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView) ;
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) ;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);
};

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
	작성자:박요한(dygks910910@daum.net)
	설명:기본적인 dx11 인터페이스 변수들.여기서 Release해주면 안된다.프로그램 종료시 release하도록 
	세팅해놨음.
	*/
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDc;
	IDXGISwapChain* mSwapChain;

	int mClientHeight;
	int mClientWidth;

//////////////////////////////////////////////////////////////////////////
 	TextureMgr mTexMgr;
	CModelManager mModelMgr;
// 	CModelMgr mModelMgr;
// 	ID3D11Buffer* mStaticNormalMappingObjectVB;
// 	ID3D11Buffer* mStaticNormalMappingObjectIB;
// 	ID3D11Buffer* mStaticBasicObjectVB;
// 	ID3D11Buffer* mStaticBasicObjectIB;
// 
// 	ID3D11Buffer* mInstanceBuffer;
// 	std::vector<XMFLOAT4X4> mInstancedModelWorld;
// 	//fenceSRV
// 	ID3D11ShaderResourceView* mFenceSRV;
// 
// 	std::vector<CStaticNomalModel> mStaticNormalModels;
// 	std::vector<CStaticBasicModel> mStaticBasicModels;

	//////////////////////////////////////////////////////////////////////////
	//Draw세팅
// 	int gridVertexOffset;
// 	int gridIndexOffset;
// 	int gidIndexCount;
// 
// 	int boxVertexOffset;
// 	int boxIndexOffset;
// 	int boxIndexCount;
// 
// 	int clownIndexOffset;
// 	int clownVertexOffset;
// 	int clownIndexCount;
// 	int gridIndexCount;
// 
// 	int fenceIndexOffset, fenceVertexOffset, fenceIndexCount;
// 	int house1IndexOffset, house1VertexOffset, house1IndexCount;
// 	int house2IndexOffset, house2VertexOffset, house2IndexCount;
// 	int house3IndexOffset, house3VertexOffset, house3IndexCount;
// 	int house4IndexOffset, house4VertexOffset, house4IndexCount;
// 	int house5IndexOffset, house5VertexOffset, house5IndexCount;
// 	int house6IndexOffset, house6VertexOffset, house6IndexCount;


	//ID3D11Buffer* mInstanceBuffer;


	//////////////////////////////////////////////////////////////////////////
	CCordinate mCordWorld;
	Camera	 mCam;
	DirectionalLight mDirLights[3];
	XMFLOAT3 mOriginalLightDir[3];

	//GameTimer mTimer;
	//D3D11_VIEWPORT mScreenViewport;
	//Terrain mTerrain;
	POINT mLastMousePos;
	Sky* mSky;

// 	std::vector<CDynamicObject*> mvDynamicObject;
// 	std::vector<CStaticObject*> mvStaticObject;

	// for Frustum culling
	//XNA::Frustum mCamFrustum;
	BoundingSphere mSceneBounds;

	float mLightRotationAngle;

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;
	//Ssao* mSsao;
	ShadowMap* mSmap;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;
public:
	CTestScene();
	virtual ~CTestScene();
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight);
	virtual void UpdateScene(const float& dt) ;
	virtual void Draw(ID3D11RenderTargetView* rtv,ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort) ;
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) ;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnResize(const float& aspectRatio);

	void DrawSceneToShadowMap();
	void BuildShadowTransform();
	
};

#pragma once

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Camera.h"
#include "Sky.h"
#include "Terrain.h"
#include "ParticleSystem.h"
#include "Box.h"
#include "Building.h"
class CMainGame : public D3DApp
{
public:
	CMainGame(HINSTANCE hInstance);
	~CMainGame();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	/*
	2017 / 1 / 8 / 19:43
	작성자:박요한(dygks910910@daum.net)
	설명:정적 객체와 동적객체를 나눠서 렌더링해줘야 할것같음.
	*/
	//스카이돔.
	Sky* mSky;
	//지형
	Terrain mTerrain;
	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ParticleSystem mFire;
	ParticleSystem mRain;
	//기본조명.
	DirectionalLight mDirLights[3];

	Camera mCam;
	//testCube
	CBox box;
	//testBuilding
	CBuilding building;

	bool mWalkCamMode;

	POINT mLastMousePos;
};

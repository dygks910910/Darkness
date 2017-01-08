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
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:���� ��ü�� ������ü�� ������ ����������� �ҰͰ���.
	*/
	//��ī�̵�.
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

	Camera mCam;
	//testCube
	CBox box;
	//testBuilding
	CBuilding building;

	bool mWalkCamMode;

	POINT mLastMousePos;
};

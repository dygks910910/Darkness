#pragma once

#include "d3dApp.h"
#include"d3dUtil.h"
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
#include "SceneMainGame.h"
#include "BoxScene.h"
#include "SceneManager.h"
class CMainGame : public D3DApp
{
public:
	CMainGame(HINSTANCE hInstance);
	~CMainGame();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	void OnMouseDown(WPARAM btnState, int x, int y,
		const HWND& mhMainWnd);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	CSceneManager* mSceneManager;
	
};

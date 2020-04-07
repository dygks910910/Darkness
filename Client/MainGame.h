#pragma once
#include "SceneManager.h"
#include "Common/d3dApp.h"
#include "Common/d3dUtil.h"
#include "Common/d3dx11Effect.h"
#include "Common/GeometryGenerator.h"
#include "Common/MathHelper.h"
#include "Common/LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Common/Camera.h"
#include "Sky.h"
#include "Terrain.h"
#include "ParticleSystem.h"

#pragma comment(lib, "ws2_32.lib")

class CMainGame : public D3DApp
{
public:
	CMainGame(HINSTANCE hInstance);
	~CMainGame();

	bool Init();
	void OnResize();
	void UpdateScene(float dt, MSG& msg);
	void DrawScene();
	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual void Packet(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	CSceneManager* mSceneManager;

};

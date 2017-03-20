#pragma once
#include"d3dUtil.h"
#include "Camera.h"
#include "Sky.h"
#include "ParticleSystem.h"
#include "Box.h"
#include "Crown.h"
#include "Terrain.h"

/*
2017 / 1 / 10 / 6:03
작성자:박요한(dygks910910@daum.net)
설명:SceneChange를 위한 추상클래스.
*/
class CScene
{
public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView, const D3D11_VIEWPORT& viewPort) = 0;
	virtual void UpdateScene(const float& dt) = 0;
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* renderTargetView,
		ID3D11DepthStencilView* depthStencilView) = 0;
	virtual void OnMouseDown(WPARAM btnState, int x, int y,const HWND& mhMainWnd)=0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)=0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)=0;
	virtual void OnResize(const float& aspectRatio) = 0;
	virtual ~CScene() {};
};


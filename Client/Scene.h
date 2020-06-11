#pragma once
#include"Common/d3dUtil.h"
#include "Common/Camera.h"
#include "Sky.h"
#include "ParticleSystem.h"
#include "Terrain.h"
namespace SceneName
{
	const std::string mainScene = "MainScene";
	const std::string gameScene = "GameScene";
	const std::string endingScene = "EndingScene";
	const std::string roomScene = "RoomScene";
}
/*
2017 / 1 / 10 / 6:03
�ۼ���:�ڿ���(dygks910910@daum.net)
����:SceneChange�� ���� �߻�Ŭ����.
*/
class CScene
{
protected:
	int mClientWidth;
	int mClientHeight;
public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, const D3D11_VIEWPORT& viewPort, const int& clientWidth, const int& clientHeight) = 0;

	virtual void OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)=0;
	virtual std::string UpdateScene(const float dt, MSG& msg) = 0;
	virtual void Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
		IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd) = 0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) = 0;
	virtual void OnResize() = 0;
	virtual ~CScene();
	float AspectRatio() const;

	void TurnZBuffOff(ID3D11DeviceContext* deviceContext);
	void TurnZBuffOn(ID3D11DeviceContext* deviceContext);
	void TurnAlphaOn(ID3D11DeviceContext* deviceContext);
	void TurnAlphaOff(ID3D11DeviceContext* deviceContext);

private:
	static ID3D11DepthStencilState* m_depthDisableStencilState;
	static ID3D11DepthStencilState* m_depthStencilState;
	static ID3D11BlendState* m_alphaEnableBlendingState;
	static ID3D11BlendState* m_alphaDisableBlendingState;
protected:
	void ReleaseZbufferState();
	bool CreateZbufferState(ID3D11Device* device);
	bool CreateAlphaBlendingState(ID3D11Device* device);


};


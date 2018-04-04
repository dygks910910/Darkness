//***************************************************************************************
// d3dApp.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Simple Direct3D demo application class.  
// Make sure you link: d3d11.lib d3dx11d.lib D3DCompiler.lib D3DX11EffectsD.lib 
//                     dxerr.lib dxgi.lib dxguid.lib.
// Link d3dx11.lib and D3DX11Effects.lib for release mode builds instead
//   of d3dx11d.lib and D3DX11EffectsD.lib.
//***************************************************************************************

#ifndef D3DAPP_H
#define D3DAPP_H

#include "d3dUtil.h"
#include "GameTimer.h"
#include <string>
#include"../resource.h"
#include"../SceneManager.h"
#include "../Global.h"
#if defined(DEBUG) | defined(_DEBUG)
#include<iostream>
#endif
namespace
{
	const int FHD_WIDTH = 1920;
	const int FHD_HEIGHT = 1080;
	const int HD_WIDTH = 1600;
	const int HD_HEIGHT = 900;
}
class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	 ~D3DApp();
	 //인터페이스
	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;
	int Run()
	{
		MSG msg = { 0 };
		float temp = 0;
		mTimer.Reset();
		while (msg.message != WM_QUIT)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Otherwise, do animation/game stuff.
			else
 			{
				mTimer.Tick();

				temp += mTimer.DeltaTime();
				if(temp >= 1.0f/60.0f)
  				{
					CalculateFrameStats();
					UpdateScene(temp, msg);
					DrawScene();
					temp = 0;
  				}
				else
				{
					Sleep(1.0f / 60.f - temp);
				}
	
			}
		}

		return (int)msg.wParam;
	}
	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.
	 bool Init();
	 void OnResize();
	 void UpdateScene(const float& dt, MSG& msg) { m_pSceneManager->UpdateScene(dt, msg); };
	 void DrawScene() { m_pSceneManager->Draw(); };
	
	 void OnKeyboardButtonDown(const WPARAM& wParam,UINT msg)
	 {
		 m_pSceneManager->OnKeyboardButtonDown(wParam,msg);
	 };
	 void OnMouseDown(const WPARAM& wParam, const int& x, const int& y) 
	 {
		 m_pSceneManager->OnMouseDown(wParam, x, y);
	 };
	 void OnMouseUp(const WPARAM& wParam, const int& x, const int& y)
	 {
		 m_pSceneManager->OnMouseUp(wParam,x,y);
	 }
	 void OnMouseMove(const WPARAM& wParam, const int& x, const int& y)
	 {
		 m_pSceneManager->OnMouseMove(wParam, x, y);
	 }

	 LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL MyDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	//멤버함수
	void CreateD3DDevice();
	void CreateSwapChain(bool use4xaa);
	void CreateDepStencilView();
	bool InitMainWindow(const bool& fullScreen);
	bool InitDirect3D();
	void CalculateFrameStats();

private:
	//커스텀 변수
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	bool mEnable4xMsaa;
	bool isFullScreen;
	std::string server_ipAddress;
	std::wstring mMainWndCaption;
	UINT      m4xMsaaQuality;
	GameTimer mTimer;
	D3D_DRIVER_TYPE md3dDriverType;
	CSceneManager* m_pSceneManager;

public :
	//정적변수
	
};




#endif // D3DAPP_H